#include "transform.h"

unsigned char brightness(struct PIXEL *pixel) {
    return (0.9 * pixel->red + 0.05 * pixel->green + 0.05 * pixel->blue) / 3;
}

unsigned char* toGrayscaleMatrix(struct Image *image) {
    int imageSize = image->image_header.heigth * image->image_header.width;
    unsigned char *grayscaleMatrix = malloc(imageSize * sizeof(unsigned char));

    for (int height = 0; height < image->image_header.heigth; height++) {
        for (int width = 0; width < image->image_header.heigth; width++) {
            int pixelPosition = height * width + width;
            *(grayscaleMatrix + pixelPosition) = brightness(image->pixels + pixelPosition);
        }
    }
    return grayscaleMatrix;
}

// Todo
struct Image* fromGrayscaleMatrix(unsigned char *grayscaleMatrix) {

}


enum BinaryColor* treshold(unsigned char *grayscaleMatrix, int size, int threshold) {
    enum BinaryColor *binaryColorMatrix = malloc(size * sizeof(enum BinaryColor));
    for (int index = 0; index < size; index++) {
        if (grayscaleMatrix[index] < threshold) 
            binaryColorMatrix[index] = BLACK;
        else
            binaryColorMatrix[index] = WHITE;
    }
    return binaryColorMatrix;
}


