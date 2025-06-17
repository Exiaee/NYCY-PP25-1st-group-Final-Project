#include <iostream>
#include <opencv2/opencv.hpp>
#include <bitset>
#include <chrono>
#include <string>
#include <filesystem>
#include <algorithm>

using namespace cv;
using namespace std;
using namespace chrono;
namespace fs = std::filesystem;

// Function to compute 64-bit perceptual hash
uint64_t computePerceptualHash(const Mat& inputImage) {
    // Resize to 32x32
    Mat resized;
    cv::resize(inputImage, resized, Size(32, 32), 0, 0, INTER_CUBIC);

    // Convert to grayscale
    Mat gray;
    if (resized.channels() == 3) {
        cv::cvtColor(resized, gray, COLOR_BGR2GRAY);
    } else {
        gray = resized;
    }

    Mat floatImg;
    gray.convertTo(floatImg, CV_32F);

    // Apply DCT
    Mat dctImage;
    cv::dct(floatImg, dctImage);

    // Extract top-left 8x8 DCT block
    Mat dctLowFreq = dctImage(Rect(0, 0, 8, 8));

    // Compute mean excluding DC component
    vector<float> dctValues;
    for (int i = 0; i < dctLowFreq.rows; ++i) {
        for (int j = 0; j < dctLowFreq.cols; ++j) {
            dctValues.push_back(dctLowFreq.at<float>(i, j));
        }
    }

    dctValues.erase(dctValues.begin()); // remove DC component
    nth_element(dctValues.begin(), dctValues.begin() + dctValues.size()/2, dctValues.end());
    float median = dctValues[dctValues.size() / 2];

    // Generate hash
    uint64_t hash = 0;
    int bitIndex = 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (i == 0 && j == 0) continue;
            float val = dctLowFreq.at<float>(i, j);
            if (val > median) {
                hash |= (uint64_t(1) << bitIndex);
            }
            ++bitIndex;
        }
    }

    return hash;
}

Mat rotateImage(const Mat& src, double angle) {
    Point2f center(src.cols / 2.0f, src.rows / 2.0f);
    Mat rot = getRotationMatrix2D(center, angle, 1.0);
    Mat dst;
    warpAffine(src, dst, rot, src.size(), INTER_LINEAR);
    return dst;
}

int hammingDistance(uint64_t hash1, uint64_t hash2) {
    return bitset<64>(hash1 ^ hash2).count();
}

bool matchWithRotation(const Mat& img1, const Mat& img2, int threshold=20, int step=30) {
    uint64_t hash1 = computePerceptualHash(img1);
    for (int angle = 0; angle < 360; angle += step) {
        Mat rotated = rotateImage(img2, angle);
        uint64_t hash2 = computePerceptualHash(rotated);

        int dist = hammingDistance(hash1, hash2);
        // float similarity = 1 - float(dist)/64;
        if (dist <= threshold) {
            cout << "Match found at angle " << angle << "° with Hamming distance: " << dist << endl;
            return true;
        }
    }

    cout << "No match found" << endl;
    return false;
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

    vector<Mat> img_vec;
    for (auto f: file_vec) {
        const char* file = f.c_str();
        Mat img = imread(file, IMREAD_COLOR);
        img_vec.push_back(img);
    }

    vector<pair<string, string>> results;
    auto start = steady_clock::now();
    for (int i = 0; i < img_vec.size(); i++) {
        string filename = fs::path(file_vec[i]).filename();
        cout << "Image: " << filename << endl;

        uint64_t hash = computePerceptualHash(img_vec[i]);
        bitset<64> bitHash(hash);
        cout << "pHash: " << bitHash << endl;

        results.push_back({filename, bitHash.to_string()});
    }
    auto end = steady_clock::now();
    double duration = duration_cast<microseconds>(end - start).count();
    printf("[OpenCV]:\t\t[%.3f] ms\n", duration/1000);

    // matchWithRotation(img1, img2);

    return 0;
}
