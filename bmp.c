#include <stdio.h>
#include <stdint.h>

typedef struct __attribute__((__packed__)) {
    unsigned char file_marker_1;
    unsigned char file_marker_2;
    unsigned int bmp_size;
    uint16_t unused_1;
    uint16_t unused_2;
    unsigned int image_data_offset;
} FILE_HEADER;

typedef struct __attribute__((__packed__)) {
    unsigned int header_size;
    int width;
    int heigth;
    uint16_t planes;
    uint16_t bits_per_pixel;
    unsigned int compression_type;
    unsigned int image_size;
    int pixels_per_meter_x;
    int pixels_per_meter_y;
    unsigned int used_color_map_entries;
    unsigned int significant_colors;
} IMAGE_HEADER;

struct Pixel {
    int red;
    int green;
    int blue;
};

void get_pixel_table(int heigth, int width, struct Pixel pixel_table[heigth][width], FILE *input) {
    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--)
        for (int width_index = 0; width_index <= width; width_index++) {
            if (width_index == width) {
                int padding = 0;

                while ((width_index + padding) % 4 != 0) {
                    padding++;

                    getc(input);
                }
            } else {
                pixel_table[heigth_index][width_index].red = getc(input);
                pixel_table[heigth_index][width_index].green = getc(input);
                pixel_table[heigth_index][width_index].blue = getc(input);
            }
        }
}

float get_pixel_val(struct Pixel pixel) {
    return (0.9 * pixel.red + 0.05 * pixel.green + 0.05 * pixel.blue) / 3;
}

float get_img_average(int heigth, int width, struct Pixel pixel_table[heigth][width]) {
    float sum = 0;

    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--)
        for (int width_index = 0; width_index < width; width_index++)
            sum += get_pixel_val(pixel_table[heigth_index][width_index]);

    return sum / (heigth * width);
}
            

void treshold(int heigth, int width, struct Pixel pixel_table[heigth][width], float img_average) {
    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--)
        for (int width_index = 0; width_index < width; width_index++) {
            if (get_pixel_val(pixel_table[heigth_index][width_index]) < img_average) {
                pixel_table[heigth_index][width_index].red = 255;
                pixel_table[heigth_index][width_index].green = 255;
                pixel_table[heigth_index][width_index].blue = 255;
            } else {
                pixel_table[heigth_index][width_index].red = 0;
                pixel_table[heigth_index][width_index].green = 0;
                pixel_table[heigth_index][width_index].blue = 0;
            }
        }
} 

void create_image(int heigth, int width, struct Pixel pixel_table[heigth][width], FILE *output) {
    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--)
        for (int width_index = 0; width_index <= width; width_index++) {
            if (width_index == width) {
                int padding = 0;
                
                while ((width_index + padding) % 4 != 0) {
                    padding++;

                    fputc(0, output);
                }
            } else {
                fputc(pixel_table[heigth_index][width_index].red, output);
                fputc(pixel_table[heigth_index][width_index].green, output);
                fputc(pixel_table[heigth_index][width_index].blue, output);
            }
        }
}

int main(int argc, char *argv[]) {
    FILE *input = fopen(argv[1], "rb");
    FILE *output = fopen(argv[2], "wb");

    FILE_HEADER file_header;
    IMAGE_HEADER image_header;

    fread(&file_header, sizeof(unsigned char), sizeof(FILE_HEADER), input);
    fread(&image_header, sizeof(unsigned char), sizeof(IMAGE_HEADER), input);
   
    fwrite(&file_header, sizeof(unsigned char), sizeof(FILE_HEADER), output);
    fwrite(&image_header, sizeof(unsigned char), sizeof(IMAGE_HEADER), output);

    struct Pixel pixel_table[image_header.heigth][image_header.width];

    get_pixel_table(image_header.heigth, image_header.width, pixel_table, input);

//    float img_average = get_img_average(image_header.heigth, image_header.width, pixel_table);

//   treshold(image_header.heigth, image_header.width, pixel_table, img_average);

    create_image(image_header.heigth, image_header.width, pixel_table, output);

    fclose(input);
    fclose(output);

    return 0;
}
