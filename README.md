# NYCU PP25 Final Project - Image Retrieval Based on Perceptual Hashing and CUDA
This project aims to design a content-based image retrieval (CBIR) system using **Perceptual Hash** to efficiently identify visually similar images. By extracting compact hash representations, the system enables fast ***search by image*** functionality in large-scale datasets. 

To address performance bottlenecks, parallel computing techniques are employed: **NVIDA CUDA** for GPU acceleration, and **OpenMP** for CPU multi-core parallelism have been included. These methods are applied to both hash generation and comparison phases to improve efficiency. The system seeks to deliver real-time image similarity retrieval with enhanced scalability and performance through the integration of perceptual hashing and high-performance parallelization.

- A command-line utility to generate pHash values for images in a dataset
- A graphical interface to compare images using **Hamming distance**

## Subprojects

### 📊 [pHash_Serial_OpenMP_GCC_O3](./pHash_Generation_Serial_OpenMP_GCC_O3/)
A command-line tool to generate pHash values for images using serial and OpenMP implementations, with GCC `-O3` optimization.

Features:
- Serial version with GCC `-O3` optimization
- OpenMP acceleration GCC `-O3` optimization
- Elapsed time for each method
- Generate pHash values for all images in the dataset
### ⚡ [pHash_cuda](./pHash_cuda/)
A command-line tool to generate pHash values for images using 3 cuda implementations.

Features:
- cuda1
- cuda2
- cuda3
- Elapsed time for each method
- Generate pHash values for all images in the dataset

### 🖼️ [pHash_Comparision_QT](./pHash_Comparison_QT/)
A Qt-based GUI application to the SQLite database and search for similar images using perceptual hashes and multiple comparison methods.

Features:
- Load the SQLite database
- GUI for image import/search
- Supports Sequential, OpenMP and PThread comparison
- Python OpenCV support for pHash

## Dataset Recommendation

- [UKBench dataset](https://www.kaggle.com/datasets/sunghoshim/ukbench100)

## License

This repository is intended for academic and educational use. Please cite if used in research or coursework.
