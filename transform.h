#ifndef TRANSFORM_H
#define TRANSOFRM_H

#include "image.h"
#include "colors.h"

#include <stdlib.h>

typedef unsigned char* grayscaleImage;
typedef enum BinaryColor* binaryImage;

unsigned char* toGrayscaleMatrix(struct Image *image);
struct Image* fromGrayscaleMatrix(unsigned char *grayscaleMatrix);
enum BinaryColor* treshold(unsigned char *grayscaleMatrix, int size, int threshold);

#endif
