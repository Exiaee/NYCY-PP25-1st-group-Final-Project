.PHONY: all cv cuda omp clean

COMPUTE_CAPABILITY ?= 61
NVCC = nvcc
CUDAFLAGS = -rdc=true -arch=sm_$(COMPUTE_CAPABILITY) -Wno-deprecated-gpu-targets

# OpenMP flags
CXX = g++
CXXFLAGS = -fopenmp 

all:
	$(CXX) -O3 phash_dir_serial.cpp -o phash_dir

cv:
	$(CXX) phash_cv.cpp -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lsqlite3 -o phash_cv

cuda:
	$(NVCC) $(CUDAFLAGS) phash_cuda.cu -o phash_cu

omp:
	$(CXX) $(CXXFLAGS) -O3 phash_dir.cpp -o phash_dir_omp

clean:
	rm -f phash_dir phash_cv phash_cu phash_dir_omp *.o
