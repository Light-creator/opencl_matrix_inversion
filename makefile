CC = gcc

OUT_CPU = ./main_cpu
SRC_CPU = ./cpu/main.c

OUT_GPU = ./main_gpu
SRC_GPU = ./gpu/main.c
FLAGS_GPU = -framework OpenCL

build_cpu:
	$(CC) $(SRC_CPU) -o $(OUT_CPU)

build_gpu:
	$(CC) $(SRC_GPU) -o $(OUT_GPU) $(FLAGS_GPU)

build_all: build_cpu build_gpu

run_cpu:
	$(OUT_CPU) $(SIZE)

run_gpu:
	$(OUT_GPU) $(SIZE)

run_tests_cpu:
	$(OUT_CPU) 16
	$(OUT_CPU) 32
	$(OUT_CPU) 128
	$(OUT_CPU) 256
	$(OUT_CPU) 512
	$(OUT_CPU) 1024
	$(OUT_CPU) 2048
	$(OUT_CPU) 4096

run_tests_gpu:
	$(OUT_GPU) 16
	$(OUT_GPU) 32
	$(OUT_GPU) 128
	$(OUT_GPU) 256
	$(OUT_GPU) 512
	$(OUT_GPU) 1024
	$(OUT_GPU) 2048
	$(OUT_GPU) 4096
