#include "image.h"

void _read_pixels(int heigth, int width, struct PIXEL *pixels, FILE *input) {
    int subpixels_per_line = width * 3;
    int required_padding = (subpixels_per_line % 4) ? 4 - subpixels_per_line % 4 : 0;
    int padding_dump = 0;

    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--) {
        fread(pixels + heigth_index * width, sizeof(struct PIXEL), width, input);
        fread(&padding_dump, sizeof(unsigned char), required_padding, input);
    }
}

void _write_pixels(int heigth, int width, struct PIXEL *pixels, FILE *output) {
    int subpixels_per_line = width * 3;
    int required_padding = (subpixels_per_line % 4) ? 4 - subpixels_per_line % 4 : 0;
    int padding_dump = 0;

    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--) {
        fwrite(pixels + heigth_index * width, sizeof(struct PIXEL), width, output);
        fwrite(&padding_dump, 1, required_padding, output);
    }
}

struct Image *_readImage(FILE *inputFile) {
    struct Image *image = malloc(sizeof(struct Image));

    fread(&(image->file_header), 1, sizeof(struct FILE_HEADER), inputFile);
    fread(&(image->image_header), 1, sizeof(struct IMAGE_HEADER), inputFile);

    image->pixels = malloc(image->image_header.heigth * image->image_header.width * sizeof(struct PIXEL));

    fseek(inputFile, image->file_header.image_data_offset, SEEK_SET);
    _read_pixels(image->image_header.heigth, image->image_header.width, image->pixels, inputFile);

    return image;
}

void _write_image(struct Image *image, FILE *outputFile) {
    fwrite(&(image->file_header), 1, sizeof(struct FILE_HEADER), outputFile);
    fwrite(&(image->image_header), 1, sizeof(struct IMAGE_HEADER), outputFile);

    int padding_dump = 0;
    int gap = image->file_header.image_data_offset - 54;

    for (int i = 0; i < gap; i++)
        fwrite(&padding_dump, 1, 1, outputFile);

    _write_pixels(image->image_header.heigth, image->image_header.width, image->pixels, outputFile);
}


struct Image *readImageFromFile(char *filename) {
    FILE *input = fopen(filename, "rb");
    struct Image *image = _readImage(input);
    fclose(input);
    return image;
}

struct Image *writeImageToFile(struct Image *image, char* filename) {
    FILE *output = fopen(filename, "wb");
    _write_image(image, output);
    fclose(output);
}

void deleteImage(struct Image *image) {
    free(image->pixels);
    free(image);
}



