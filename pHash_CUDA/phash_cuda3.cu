#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <bitset>
#include <string>
#include <filesystem>
#include "stb_image.h"

using namespace std;
using namespace chrono;
namespace fs = std::filesystem;

#define RESIZE_W 32
#define RESIZE_H 32
#define DCT_SIZE 32
#define HASH_SIZE 8
#define MAX_STREAMS 8

#define CHECK_CUDA(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            std::cerr << "CUDA error: " << cudaGetErrorString(err) << std::endl; \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

__device__ float dct_coeff(int k, int N) {
    return (k == 0) ? sqrtf(1.0f / N) : sqrtf(2.0f / N);
}

__global__ void ucharToGrayscaleKernel(const unsigned char* input, float* output, int size, int channels) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= size) return;
    if (channels == 3) {
        float r = input[i * 3 + 0];
        float g = input[i * 3 + 1];
        float b = input[i * 3 + 2];
        output[i] = 0.299f * r + 0.587f * g + 0.114f * b;
    } else {
        output[i] = input[i];
    }
}

__global__ void resizeBilinearKernel(const float* input, float* output, int inW, int inH, int outW, int outH) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= outW || y >= outH) return;

    float scaleX = static_cast<float>(inW) / outW;
    float scaleY = static_cast<float>(inH) / outH;

    float srcX = (x + 0.5f) * scaleX - 0.5f;
    float srcY = (y + 0.5f) * scaleY - 0.5f;

    int x0 = max(0, min(inW - 2, static_cast<int>(floorf(srcX))));
    int y0 = max(0, min(inH - 2, static_cast<int>(floorf(srcY))));

    float dx = srcX - x0;
    float dy = srcY - y0;

    float val =
        (1 - dx) * (1 - dy) * input[y0 * inW + x0] +
        dx * (1 - dy) * input[y0 * inW + (x0 + 1)] +
        (1 - dx) * dy * input[(y0 + 1) * inW + x0] +
        dx * dy * input[(y0 + 1) * inW + (x0 + 1)];

    output[y * outW + x] = val;
}

__global__ void dctRowsKernel(const float* input, float* output, int N) {
    int row = blockIdx.x;
    int u = threadIdx.x;

    if (row < N && u < N) {
        float sum = 0.0f;
        for (int x = 0; x < N; ++x) {
            sum += input[row * N + x] * cosf((M_PI * (2 * x + 1) * u) / (2.0f * N));
        }
        output[row * N + u] = dct_coeff(u, N) * sum;
    }
}

__global__ void dctColsKernel(const float* input, float* output, int N) {
    int col = blockIdx.x;
    int v = threadIdx.x;

    if (col < N && v < N) {
        float sum = 0.0f;
        for (int y = 0; y < N; ++y) {
            sum += input[y * N + col] * cosf((M_PI * (2 * y + 1) * v) / (2.0f * N));
        }
        output[v * N + col] = dct_coeff(v, N) * sum;
    }
}

__global__ void generateHashKernel(const float* dct, uint64_t* hashOut) {
    __shared__ float block[HASH_SIZE * HASH_SIZE];
    int idx = threadIdx.x;
    if (idx < HASH_SIZE * HASH_SIZE) {
        int i = idx / HASH_SIZE;
        int j = idx % HASH_SIZE;
        block[idx] = dct[i * DCT_SIZE + j];
    }
    __syncthreads();

    if (idx == 0) {
        float sum = 0;
        for (int i = 1; i < HASH_SIZE * HASH_SIZE; ++i) // skip DC
            sum += block[i];
        float avg = sum / (HASH_SIZE * HASH_SIZE - 1);

        uint64_t hash = 0;
        for (int i = 1; i < HASH_SIZE * HASH_SIZE; ++i)
            if (block[i] > avg)
                hash |= (1ULL << (i - 1));
        *hashOut = hash;
    }
}

void computeHashAsync(unsigned char* img, int width, int height, int channels, cudaStream_t stream, uint64_t* h_hash) {
    int imgSize = width * height;
    size_t imgBytes = imgSize * channels * sizeof(unsigned char);
    size_t grayBytes = imgSize * sizeof(float);
    size_t resizedBytes = RESIZE_W * RESIZE_H * sizeof(float);

    unsigned char* d_img;
    float *d_gray, *d_resized, *d_tempDCT, *d_finalDCT;
    uint64_t* d_hash;

    CHECK_CUDA(cudaMallocAsync(&d_img, imgBytes, stream));
    CHECK_CUDA(cudaMallocAsync(&d_gray, grayBytes, stream));
    CHECK_CUDA(cudaMallocAsync(&d_resized, resizedBytes, stream));
    CHECK_CUDA(cudaMallocAsync(&d_tempDCT, resizedBytes, stream));
    CHECK_CUDA(cudaMallocAsync(&d_finalDCT, resizedBytes, stream));
    CHECK_CUDA(cudaMallocAsync(&d_hash, sizeof(uint64_t), stream));

    CHECK_CUDA(cudaMemcpyAsync(d_img, img, imgBytes, cudaMemcpyHostToDevice, stream));

    int threads = 256;
    int blocks = (imgSize + threads - 1) / threads;
    ucharToGrayscaleKernel<<<blocks, threads, 0, stream>>>(d_img, d_gray, imgSize, channels);

    dim3 t2D(16, 16);
    dim3 b2D((RESIZE_W + 15) / 16, (RESIZE_H + 15) / 16);
    resizeBilinearKernel<<<b2D, t2D, 0, stream>>>(d_gray, d_resized, width, height, RESIZE_W, RESIZE_H);

    dctRowsKernel<<<RESIZE_H, RESIZE_W, 0, stream>>>(d_resized, d_tempDCT, RESIZE_W);
    dctColsKernel<<<RESIZE_W, RESIZE_H, 0, stream>>>(d_tempDCT, d_finalDCT, RESIZE_W);

    generateHashKernel<<<1, HASH_SIZE * HASH_SIZE, 0, stream>>>(d_finalDCT, d_hash);
    CHECK_CUDA(cudaMemcpyAsync(h_hash, d_hash, sizeof(uint64_t), cudaMemcpyDeviceToHost, stream));

    CHECK_CUDA(cudaFreeAsync(d_img, stream));
    CHECK_CUDA(cudaFreeAsync(d_gray, stream));
    CHECK_CUDA(cudaFreeAsync(d_resized, stream));
    CHECK_CUDA(cudaFreeAsync(d_tempDCT, stream));
    CHECK_CUDA(cudaFreeAsync(d_finalDCT, stream));
    CHECK_CUDA(cudaFreeAsync(d_hash, stream));
}

uint64_t computeHashGPU(unsigned char* img, int width, int height, int channels) {
    int imgSize = width * height;

    float* d_grayInput;
    float* d_resized;
    float* d_dctTemp;
    float* d_dctFinal;
    uint64_t* d_hash;

    CHECK_CUDA(cudaMalloc(&d_grayInput, imgSize * sizeof(float)));
    CHECK_CUDA(cudaMalloc(&d_resized, RESIZE_W * RESIZE_H * sizeof(float)));
    CHECK_CUDA(cudaMalloc(&d_dctTemp, RESIZE_W * RESIZE_H * sizeof(float)));
    CHECK_CUDA(cudaMalloc(&d_dctFinal, RESIZE_W * RESIZE_H * sizeof(float)));
    CHECK_CUDA(cudaMalloc(&d_hash, sizeof(uint64_t)));

    unsigned char* d_img;
    CHECK_CUDA(cudaMalloc(&d_img, imgSize * channels * sizeof(unsigned char)));
    CHECK_CUDA(cudaMemcpy(d_img, img, imgSize * channels * sizeof(unsigned char), cudaMemcpyHostToDevice));

    int threads = 256;
    int blocks = (imgSize + threads - 1) / threads;
    ucharToGrayscaleKernel<<<blocks, threads>>>(d_img, d_grayInput, imgSize, channels);

    dim3 threads2D(16, 16);
    dim3 blocks2D((RESIZE_W + 15) / 16, (RESIZE_H + 15) / 16);
    resizeBilinearKernel<<<blocks2D, threads2D>>>(d_grayInput, d_resized, width, height, RESIZE_W, RESIZE_H);

    dctRowsKernel<<<RESIZE_H, RESIZE_W>>>(d_resized, d_dctTemp, RESIZE_W);
    dctColsKernel<<<RESIZE_W, RESIZE_H>>>(d_dctTemp, d_dctFinal, RESIZE_W);

    generateHashKernel<<<1, HASH_SIZE * HASH_SIZE>>>(d_dctFinal, d_hash);

    uint64_t hash;
    CHECK_CUDA(cudaMemcpy(&hash, d_hash, sizeof(uint64_t), cudaMemcpyDeviceToHost));

    CHECK_CUDA(cudaFree(d_img));
    CHECK_CUDA(cudaFree(d_grayInput));
    CHECK_CUDA(cudaFree(d_resized));
    CHECK_CUDA(cudaFree(d_dctTemp));
    CHECK_CUDA(cudaFree(d_dctFinal));
    CHECK_CUDA(cudaFree(d_hash));

    return hash;
}

int main(int argc, char** argv) {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    printf("SMs: %d\n", prop.multiProcessorCount);

    int img_count = 1000;

    vector<string> file_vec;
    try {
        int i = 0;
        for (const auto& entry : fs::directory_iterator(argv[1])) {
            i++;
            if (entry.is_regular_file() && entry.path().extension() == ".jpg")
                file_vec.push_back(entry.path());
            if (i == img_count) break;
        }
    } catch (const fs::filesystem_error& e) {
        cout << "Filesystem error: " << e.what() << endl;
        return 1;
    }
    cout << "Total images: " << file_vec.size() << endl;

    sort(file_vec.begin(), file_vec.end());


    const int batchSize = min(MAX_STREAMS, static_cast<int>(file_vec.size()));
    cudaStream_t streams[MAX_STREAMS];
    for (int i = 0; i < batchSize; ++i)
        CHECK_CUDA(cudaStreamCreate(&streams[i]));

    vector<uint64_t> hashes(file_vec.size());
    vector<unsigned char*> hostImgs(batchSize);
    vector<tuple<int, int, int>> imgShapes(batchSize);

    double duration = 0;
    for (size_t i = 0; i < file_vec.size(); i += batchSize) {
        int actualBatch = min((int)batchSize, (int)(file_vec.size() - i));

        for (int j = 0; j < actualBatch; ++j) {
            int width, height, channels;
            hostImgs[j] = stbi_load(file_vec[i + j].c_str(), &width, &height, &channels, 0);
            imgShapes[j] = {width, height, channels};
        }

        auto start = steady_clock::now();
        for (int j = 0; j < actualBatch; ++j) {
            auto [w, h, c] = imgShapes[j];
            computeHashAsync(hostImgs[j], w, h, c, streams[j], &hashes[i + j]);
        }

        auto end = steady_clock::now();
        double d = duration_cast<milliseconds>(end - start).count();
        duration += d;

        for (int j = 0; j < actualBatch; ++j) {
            CHECK_CUDA(cudaStreamSynchronize(streams[j]));
            stbi_image_free(hostImgs[j]);
        }
    }

    for (int i = 0; i < batchSize; ++i)
        CHECK_CUDA(cudaStreamDestroy(streams[i]));
    
    for (size_t i = 0; i < file_vec.size(); ++i) {
        string filename = fs::path(file_vec[i]).filename();
        cout << "Image: " << filename << endl;

        bitset<64> bitHash(hashes[i]);
        cout << "pHash: " << bitHash << endl;
    }
    
    printf("[CUDA Stream]:\t\t[%.3f] ms\n", duration);

    return 0;
}
