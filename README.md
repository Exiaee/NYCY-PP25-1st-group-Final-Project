# NYCY PP25 1st group Final Project

This project provides tools to compute and compare perceptual hashes (pHash) for image similarity detection. It includes both a graphical interface and a command-line benchmarking suite.

## Subprojects

### 📊 [pHash_Serial_OpenMP_GCC_O3](./pHash_Generation_Serial_OpenMP_GCC_O3/)
A command-line tool to generate pHash values for images using serial and OpenMP implementations, with GCC `-O3` optimization.

Features:
- Serial version with GCC `-O3` optimization
- OpenMP acceleration GCC `-O3` optimization
- Elapsed time for each method
- Generate pHash values for all images in the dataset
### 📊 [pHash_Serial_OpenMP_GCC_O3](./pHash_Generation_Serial_OpenMP_GCC_O3/)
A command-line tool to generate pHash values for images using serial and OpenMP implementations, with GCC `-O3` optimization.

Features:
- Serial version with GCC `-O3` optimization
- OpenMP acceleration GCC `-O3` optimization
- Elapsed time for each method
- Generate pHash values for all images in the dataset

### 🖼️ [pHash_Comparision_QT](./pHash_Comparison_QT/)
A Qt-based GUI application to search for similar images using perceptual hashes and multiple comparison methods.

Features:
- GUI for image import/search
- Supports Sequential, OpenMP and PThread comparison
- Load SQLite
- Python OpenCV support for pHash

## Dataset Recommendation

- [UKBench dataset](https://www.kaggle.com/datasets/sunghoshim/ukbench100)

## License

This repository is intended for academic and educational use. Please cite if used in research or coursework.
