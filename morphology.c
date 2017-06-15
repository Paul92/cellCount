#include "colors.h"
#include "morphology.h"

struct Kernel *squareKernel(int size) {
    struct Kernel *kernel = malloc(size * size);
    for (int index = 0; index < size; index++)
        kernel->kernel[index] = 1;
    kernel->width = size;
    kernel->height = size;
    return kernel;
}

void deleteKernel(struct Kernel *kernel) {
    free(kernel);
}


void binaryImage erosion(binaryImage image, struct Kernel *krn) {
// TODO
}

void binaryImage dilation(binaryImage image, struct Kernel *krn) {
// TODO
}

void binaryImage opening(binaryImage image, struct Kernel *krn) {
    dilation(image, kernel);
    erosion(image, kernel);
}

void binaryImage closing(binaryImage image, struct Kernel *krn) {
    erosion(image, kernel);
    dilation(image, kernel);
}
