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
