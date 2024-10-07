cmake_minimum_required(VERSION 3.10)
project(RawToRgbOpenCL)

# Specify the directory for source files
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# Find OpenCL package
find_package(OpenCL REQUIRED)

# Set the sources (assuming your C++ code is in src/main.cpp)
set(SOURCES "${SRC_DIR}/main.cpp")

# Create the executable
add_executable(raw_to_rgb ${SOURCES})

# Include OpenCL libraries
target_include_directories(raw_to_rgb PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(raw_to_rgb PRIVATE ${OpenCL_LIBRARIES})

# Copy the .cl file to the build directory
configure_file("${SRC_DIR}/raw_to_rgb.cl" "${CMAKE_BINARY_DIR}/raw_to_rgb.cl" COPYONLY)
