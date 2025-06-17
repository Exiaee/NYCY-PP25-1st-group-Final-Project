# NYCY PP25 1st group Final Project

This project provides tools to compute and compare perceptual hashes (pHash) for image similarity detection. It includes both a graphical interface and a command-line benchmarking suite.

## Subprojects

### 📊 [pHash_Serial_OpenMP_GCC_O3](./pHash_Generation_Serial_OpenMP_GCC_O3/)
A command-line tool to generate and compare pHashes using serial and OpenMP implementations, with optional GCC `-O3` optimization.

Features:
- Serial pHash 
- OpenMP acceleration
- GCC `-O3` performance benchmarking
- Dataset batch testing and runtime profiling

### 🖼️ [pHash_Comparision_QT](./pHash_Comparision_QT/)
A Qt-based GUI application to insert, manage, and search for similar images using perceptual hashes and multiple comparison methods.

Features:
- GUI for image import/search
- Supports OpenMP and PThread comparison
- Load SQLite
- Python OpenCV support for pHash

## Dataset Recommendation

- [UKBench dataset](https://www.kaggle.com/datasets/sunghoshim/ukbench100)

## License

This repository is intended for academic and educational use. Please cite if used in research or coursework.
