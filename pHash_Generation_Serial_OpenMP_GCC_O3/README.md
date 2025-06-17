# pHash_Serial_OpenMP_GCC_O3

This project generates pHash values using 2 processing approaches in C++:

- Serial implementation with `-O3` optimization
- OpenMP-based parallelism with `-O3` optimization

## Overview

The goal is to measure the elapsed time of pHash generation and comparison using different execution models, 
and evaluate performance and speedup benefits of parallel processing.

## Features

- Compute pHash of images
- Evaluate performance with:
  - Serial C++ with `-O3` optimization
  - OpenMP with `-O3` optimization

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
# Clean up build artifacts
make clean
```
## Usage

```bash
# Run Serial version
run [-c] [cpu thread] ./phash_dir <image_folder> [image_count]

# Run OpenMP version
run [-c] [cpu thread] ./phash_dir_omp <image_folder> [image_count]
```

### Options
- `-c [cpu thread]`: (Optional) Pin to specific CPU core/thread
- `image_folder`: Folder containing input images
- `image_count`: (Optional) number of images to process; if not set, process all

Example:

```bash
run -c 8 ./phash_dir_omp ./dataset 1000
```

## Output

- Elapsed time for each method
- Generated pHash values for all images in the dataset

## License

This project is for academic or benchmarking use only. Please cite if reused.

