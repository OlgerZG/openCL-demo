#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstddef>  // For std::size_t

// Namespace for local configuration constants
namespace LocalConfig {
    constexpr std::size_t Bins = 256;
    constexpr std::size_t ImgWidth = 1920;
    constexpr std::size_t ImgHeight = 1080;
    constexpr std::size_t ImgSize = ImgWidth * ImgHeight;
}

// Function to load RAW image data from a file
std::vector<uint8_t> loadRawImage(const std::string& filename, std::size_t imageSize) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<uint8_t> buffer(imageSize);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(buffer.data()), imageSize);
        file.close();
    } else {
        std::cerr << "Failed to open the RAW image file!" << std::endl;
    }
    return buffer;
}

// Function to load OpenCL kernel from a file
std::string loadKernel(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the kernel file!" << std::endl;
    }
    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}

// Function to save the RGB image as a PPM file
void saveImageAsPPM(const std::string& filename, const std::vector<uint8_t>& rgbImage, std::size_t width, std::size_t height) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to save the RGB image!" << std::endl;
        return;
    }

    // Write PPM header
    file << "P6\n" << width << " " << height << "\n255\n";

    // Write pixel data
    file.write(reinterpret_cast<const char*>(rgbImage.data()), rgbImage.size());
    file.close();
    std::cout << "Image saved as " << filename << std::endl;
}

int main() {
    using namespace LocalConfig;

    // Load RAW image data using constexpr image size
    std::vector<uint8_t> rawImage = loadRawImage("image.raw", ImgSize);

    // Load OpenCL kernel code from file
    std::string kernelSource = loadKernel("raw_to_rgb.cl");

    // Initialize OpenCL
    cl_int err;
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, nullptr);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, 0, &err);

    // Create buffers using constexpr size
    cl_mem rawBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, ImgSize, rawImage.data(), &err);
    cl_mem rgbBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, ImgSize * 3, nullptr, &err);

    // Compile OpenCL program
    const char* kernelSourceCStr = kernelSource.c_str();
    size_t kernelLength = kernelSource.length();
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSourceCStr, &kernelLength, &err);
    clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, "rawToRgb", &err);

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &rawBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &rgbBuffer);
    clSetKernelArg(kernel, 2, sizeof(int), &ImgWidth);
    clSetKernelArg(kernel, 3, sizeof(int), &ImgHeight);

    // Execute kernel
    size_t globalSize[] = { ImgWidth, ImgHeight };
    clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalSize, nullptr, 0, nullptr, nullptr);

    // Read back the RGB image
    std::vector<uint8_t> rgbImage(ImgSize * 3);
    clEnqueueReadBuffer(queue, rgbBuffer, CL_TRUE, 0, rgbImage.size(), rgbImage.data(), 0, nullptr, nullptr);

    // Save the image as a PPM file
    saveImageAsPPM("output_image.ppm", rgbImage, ImgWidth, ImgHeight);

    // Release resources
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(rawBuffer);
    clReleaseMemObject(rgbBuffer);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    std::cout << "Image processing complete!" << std::endl;
    return 0;
}
