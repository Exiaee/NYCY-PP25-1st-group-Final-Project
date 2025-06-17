# pHash_Serial_OpenMP_GCC_O3

This project benchmarks perceptual hash (pHash) similarity comparisons using different parallel processing approaches in C++:

- Serial implementation with `-O3` optimization
- OpenMP-based parallelism with `-O3` optimization
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
- stb_image for image loading (depending on version)

## Build Instructions
- This project is tested and runs successfully on NYCU CS departmental servers using Makefile-based build flow.

### Linux

```bash
# Build the serial version
make all
# Build the OpenMP version
make omp
```

## Usage

```bash
./phash_compare <image_folder> <comparison_mode>
run -c [cpu thread] ./phash_dir_omp 
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
- Generated pHash values for all images in the dataset

## License

This project is for academic or benchmarking use only. Please cite if reused.

