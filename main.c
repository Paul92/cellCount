#include "image.h"
#include "transform.h"

int main(int argc, char *argv[]) {
    struct Image *image = readImageFromFile(argv[1]);
    grayscaleImage gs = toGrayscaleMatrix(image);
    binaryImage bs = threshold(gs);

    deleteImage(image);
    free(gs);

    return 0;
}
