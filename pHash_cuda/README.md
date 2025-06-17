# pHash_cuda

This project generates pHash values using 3 processing approaches in C++:

- cuda1
- cuda2
- cuda3

## Overview

The goal is to measure the elapsed time of pHash generation and comparison using different execution models, 
and evaluate performance and speedup benefits of cuda processing.

## Features

- Compute pHash of images
- Evaluate performance with:
  - cuda1
  - cuda2
  - cuda3

## Dependencies

- GCC 12.4.0 or later
- CUDA 12.8
- C++17 or later
- stb_image for image loading (depending on version)

## Build Instructions
- This project is tested and runs successfully on NYCU CS departmental servers using Makefile-based build flow.

### Linux

```bash
# Build the OpenCV version
make cv
# Build the cuda version
> Note: `phash_cuda2.cu` in Makefile can be manually replaced if testing different CUDA implementations before running `make cuda`.
make cuda
# Clean up build artifacts
make clean
```
## Usage

```bash
# Run cuda version
run [-c] [cpu thread] ./phash_cu <image_folder>
```

### Options
- `-c [cpu thread]`: (Optional) Pin to specific CPU core/thread
- `image_folder`: Folder containing input images

Example:

```bash
run -c 8 ./phash_cu ./dataset
```

## Output

- Elapsed time for each method
- Generated pHash values for all images in the dataset

## License

This project is for academic or benchmarking use only. Please cite if reused.

