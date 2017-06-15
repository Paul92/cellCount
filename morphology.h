#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include "transform.h"
#include "colors.h"
#include "image.h"

struct Kernel {
    char *kernel;
    int width;
    int height;
}

enum kernelState{FIT, HIT, OUT};

struct Kernel *squareKernel(int size);
void deleteKernel(struct Kernel kernel);

enum kernelState checkKernel(binaryImage image, struct Kernel *krn, int x, int y);

void binaryImage erosion(binaryImage image, struct Kernel *krn);
void binaryImage dilation(binaryImage image, struct Kernel *krn);

void binaryImage opening(binaryImage image, struct Kernel *krn);
void binaryImage closing(binaryImage image, struct Kernel *krn);

#endif
