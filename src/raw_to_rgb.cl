__kernel void rawToRgb(__global const uchar* rawImage, __global uchar* rgbImage, const int width, const int height) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x < width && y < height) {
        int idx = y * width + x;
        int rgbIdx = idx * 3;

        // Simple conversion, can implement more complex debayering methods
        uchar rawValue = rawImage[idx];

        // Assign RGB values (example with grayscale mapping)
        rgbImage[rgbIdx + 0] = rawValue; // Red
        rgbImage[rgbIdx + 1] = rawValue; // Green
        rgbImage[rgbIdx + 2] = rawValue; // Blue
    }
}
