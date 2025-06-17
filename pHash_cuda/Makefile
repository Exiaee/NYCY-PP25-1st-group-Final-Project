.PHONY: all cv clean

COMPUTE_CAPABILITY ?= 61
NVCC = nvcc
CUDAFLAGS = -rdc=true -arch=sm_$(COMPUTE_CAPABILITY) -Wno-deprecated-gpu-targets

all:
	g++ phash.cpp -o phash

cv:
	g++ phash_cv.cpp -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -o phash_cv

cuda:
	$(NVCC) $(CUDAFLAGS) phash_cuda2.cu -o phash_cu
