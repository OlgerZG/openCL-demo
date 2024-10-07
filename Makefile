# Makefile for building and running the GSLTransposeExample using CMake

# Variables
BUILD_DIR = build
EXECUTABLE = $(BUILD_DIR)/raw_to_rgb

# Default target: build and run
all: clear build run
mem: build memtest
test: clear run_test

# Configure and generate the build system with CMake
clear: 
	clear
cmake:
	cmake . -B $(BUILD_DIR)

# Build the project using CMake
build: cmake
	cmake --build $(BUILD_DIR)

# Run the executable
run: build
	$(EXECUTABLE)
	
# Clean the build directory
clean:
	rm -rf $(BUILD_DIR)

