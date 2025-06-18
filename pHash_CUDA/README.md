# pHash_CUDA
This project generates pHash values using NVIDA CUDA library to enable parallel programming technology.

## Overview
The goal is to measure the elapsed time of pHash generation using different execution models, 
and evaluate performance and speedup benefits of parallel processing.

## CUDA Acceleration
Implemented CUDA Kernel with four aspects in pHash calculation
1. uchar → float & grayscale conversion
2. Bilinear interpolation Resize
3. 2D DCT (Performed in Two Stages)
  - Row-wise DCT: (One thread block per row)
  - Column-wise DCT
4. 8×8 DCT block extraction and Median comparison


## CUDA Stram Acceleration
To enhance GPU utilization and reduce idle time, we adopt CUDA streams to enable concurrent and asynchronous processing of images.
Within the same stream, operations are executed in order. Across different streams, operations can run concurrently, and provided hardware resources (e.g., SMs) are available. In this system, the GPU features 10 Streaming Multiprocessors (SMs), allowing for substantial parallelism.

## Dependencies
- GCC 12.4.0 or later
- C++17 or later
- NVIDIA CUDA 12.8
- stb_image for image loading (depending on version)

## Build Instructions
- This project is tested and runs successfully on NYCU CS departmental servers using Makefile-based build flow.

### Linux

```bash
make cuda
```
## Usage

```bash
run -- ./phash_cu <image_folder>
```

### Options
- `image_folder`: Folder containing input images


## Output

- Elapsed time for each method
- Generated pHash values for all images in the dataset

## License

This project is for academic or benchmarking use only. Please cite if reused.

