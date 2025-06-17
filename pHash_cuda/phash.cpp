#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <bitset>
#include <chrono>
#include <string>
#include <filesystem>
#include "stb_image.h"
#include "cycle_timer.h"

using namespace std;
using namespace chrono;
namespace fs = std::filesystem;

const int RESIZE_W = 32;
const int RESIZE_H = 32;
const double PI = 3.14159265358979323846;


// 實作 2D DCT-II
void computeDCT2D(vector<float>& input, vector<float>& output) {
    int N = RESIZE_W;
    output.resize(N * N);
    
    for (int u = 0; u < N; ++u) {
        for (int v = 0; v < N; ++v) {
            double sum = 0.0;
            for (int x = 0; x < N; ++x) {
                for (int y = 0; y < N; ++y) {
                    float val = input[x * N + y];
                    sum += val *
                           cos((PI * (2 * x + 1) * u) / (2.0 * N)) *
                           cos((PI * (2 * y + 1) * v) / (2.0 * N));
                }
            }
            double Cu = (u == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);
            double Cv = (v == 0) ? sqrt(1.0 / N) : sqrt(2.0 / N);
            output[u * N + v] = static_cast<float>(Cu * Cv * sum);
        }
    }
}

// Bicubic kernel
float cubic(float x) {
    const float a = -0.5f;
    x = fabs(x);
    if (x <= 1.0f)
        return (a + 2) * x * x * x - (a + 3) * x * x + 1;
    else if (x < 2.0f)
        return a * x * x * x - 5 * a * x * x + 8 * a * x - 4 * a;
    else
        return 0.0f;
}

// Clamp utility
int clamp(int v, int low, int high) {
    return std::max(low, std::min(high, v));
}

// Resize using bicubic interpolation
void resizeBicubic(const vector<float>& input, int inW, int inH,
                   vector<float>& output, int outW, int outH, int channels) {
    output.resize(outW * outH * channels);

    float scaleX = static_cast<float>(inW) / outW;
    float scaleY = static_cast<float>(inH) / outH;

    for (int y = 0; y < outH; ++y) {
        float srcY = (y + 0.5f) * scaleY - 0.5f;
        int yInt = static_cast<int>(floor(srcY));
        float yDiff = srcY - yInt;

        for (int x = 0; x < outW; ++x) {
            float srcX = (x + 0.5f) * scaleX - 0.5f;
            int xInt = static_cast<int>(floor(srcX));
            float xDiff = srcX - xInt;

            for (int c = 0; c < channels; ++c) {
                float value = 0.0f;
                for (int m = -1; m <= 2; ++m) {
                    for (int n = -1; n <= 2; ++n) {
                        int ix = clamp(xInt + n, 0, inW - 1);
                        int iy = clamp(yInt + m, 0, inH - 1);
                        float coeff = cubic(n - xDiff) * cubic(m - yDiff);
                        value += coeff * input[(iy * inW + ix) * channels + c];
                    }
                }
                output[(y * outW + x) * channels + c] = value;
            }
        }
    }
}

// Convert RGB to Grayscale
void rgbToGrayscale(const vector<float>& rgb, int width, int height,
                    vector<float>& gray) {
    gray.resize(width * height);
    for (int i = 0; i < width * height; ++i) {
        float r = rgb[i * 3 + 0];
        float g = rgb[i * 3 + 1];
        float b = rgb[i * 3 + 2];
        gray[i] = 0.299f * r + 0.587f * g + 0.114f * b;
    }
}

// Convert uchar to float
void ucharToFloat(const vector<unsigned char>& input, vector<float>& output) {
    output.resize(input.size());
    for (size_t i = 0; i < input.size(); ++i)
        output[i] = static_cast<float>(input[i]);
}

void processImage(const vector<unsigned char>& inputImage, int width, int height, int channels,
                  vector<float>& resizedGray) {
    vector<float> floatImage;
    ucharToFloat(inputImage, floatImage);

    vector<float> resized;
    resizeBicubic(floatImage, width, height, resized, 32, 32, channels);

    if (channels == 3) {
        rgbToGrayscale(resized, 32, 32, resizedGray);
    } else {
        resizedGray = resized; // Already grayscale
    }
}

uint64_t computePerceptualHash(unsigned char* imgData, int width, int height, int channels) {
    vector<unsigned char> imageVec(imgData, imgData + width * height * channels);

    // 處理成 32x32 灰階 float
    vector<float> resizedGray;
    processImage(imageVec, width, height, channels, resizedGray);

    stbi_image_free(imgData);

    // Apply DCT
    vector<float> dctOutput;
    computeDCT2D(resizedGray, dctOutput);

    // Extract 8x8 top-left DCT block
    vector<float> dctValues;
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            dctValues.push_back(dctOutput[i * RESIZE_W + j]);

    // Compute mean excluding DC component
    dctValues.erase(dctValues.begin()); // remove DC component
    nth_element(dctValues.begin(), dctValues.begin() + dctValues.size()/2, dctValues.end());
    float median = dctValues[dctValues.size() / 2];
    // cout << "median: " << median << endl;

    // Generate hash
    uint64_t hash = 0;
    int bitIndex = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (i == 0 && j == 0) continue; // skip DC
            float val = dctOutput[i * RESIZE_W + j];
            if (val > median) {
                hash |= (uint64_t(1) << bitIndex);
            }
            ++bitIndex;
        }
    }

    return hash;
}

int hammingDistance(uint64_t hash1, uint64_t hash2) {
    return bitset<64>(hash1 ^ hash2).count();
}

int main(int argc, char** argv) {
    int img_count = 5000;

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

    vector<pair<unsigned char*, tuple<int, int, int>>> img_vec;
    for (auto f: file_vec) {
        const char* file = f.c_str();
        int width, height, channels;
        unsigned char* img = stbi_load(file, &width, &height, &channels, 0);
        img_vec.push_back({img, {width, height, channels}});
    }

    vector<pair<string, string>> results;
    auto start = steady_clock::now();
    for (int i = 0; i < img_vec.size(); i++) {
        string filename = fs::path(file_vec[i]).filename();
        cout << "Image: " << filename << endl;

        auto img = img_vec[i].first;
        auto [width, height, channels] = img_vec[i].second;
        uint64_t hash = computePerceptualHash(img, width, height, channels);
        bitset<64> bitHash(hash);
        cout << "pHash: " << bitHash << endl;

        results.push_back({filename, bitHash.to_string()});
    }
    auto end = steady_clock::now();
    double duration = duration_cast<milliseconds>(end - start).count();
    printf("[Serial]:\t\t[%.3f] ms\n", duration);

    return 0;
}
