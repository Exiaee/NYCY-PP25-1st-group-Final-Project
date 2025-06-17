# pHash_Serial_OpenMP_GCC_O3

This project benchmarks perceptual hash (pHash) similarity comparisons using different parallel processing approaches in C++:

- Serial implementation
- OpenMP-based parallelism
- Compiled with `-O3` optimization using GCC

## Overview

The goal is to measure and compare the performance of Hamming distance-based image similarity search using pHash under different execution models.

## Features

- Compute perceptual hash (pHash) of images
- Compare hashes using Hamming distance
- Evaluate performance with:
  - Serial C++ loop
  - OpenMP parallel loop
  - GCC with `-O3` optimization
- Batch test on image datasets
- Time measurement for benchmarking

## Dependencies

- GCC with OpenMP support
- C++17 or later
- stb_image or OpenCV for image loading (depending on version)

## Build Instructions

### Linux / WSL / macOS

```bash
g++ -O3 -fopenmp -std=c++17 main.cpp imagephash.cpp -o phash_compare
```

### Windows (MSYS2 / MinGW)

```bash
g++ -O3 -fopenmp -std=c++17 main.cpp imagephash.cpp -o phash_compare.exe
```

## Usage

```bash
./phash_compare <image_folder> <comparison_mode>
```

Where `<comparison_mode>` is one of:
- `serial`
- `openmp`

Example:

```bash
./phash_compare ./dataset openmp
```

## Output

- Elapsed time for each method
- Speedup of OpenMP vs Serial
- List of similar image pairs (optional)

## License

This project is for academic or benchmarking use only. Please cite if reused.

