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

typedef struct __attribute__((__packed__)) {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} PIXEL;

void read_image(int heigth, int width, PIXEL image[heigth][width], unsigned int image_offset, FILE *input) {
    int subpixels_per_line = width * 3;
    int required_padding = (subpixels_per_line % 4) ? 4 - subpixels_per_line % 4 : 0;
    int padding_dump = 0;

    fseek(input, image_offset, SEEK_SET);

    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--) {
        fread(&(image[heigth_index]), sizeof(PIXEL), width, input);
        fread(&padding_dump, sizeof(unsigned char), required_padding, input);
    }
}

void write_image(int heigth, int width, PIXEL image[heigth][width], unsigned int image_offset, FILE *output) {
    int subpixels_per_line = width * 3;
    int required_padding = (subpixels_per_line % 4) ? 4 - subpixels_per_line % 4 : 0;
    int padding_dump = 0;

    int gap = image_offset - 54;

    fwrite(&padding_dump, sizeof(unsigned char), gap, output);

    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--) {
        fwrite(&(image[heigth_index]), sizeof(PIXEL), width, output);
        fwrite(&padding_dump, sizeof(unsigned char), required_padding, output);
    }
}

void copy_image(int heigth, int width, PIXEL input[heigth][width], PIXEL output[heigth][width]) {
    for (int heigth_index = 0; heigth_index < heigth; heigth_index++)
        for (int width_index = 0; width_index < width; width_index++) {
            output[heigth_index][width_index].red = input[heigth_index][width_index].red;
            output[heigth_index][width_index].green = input[heigth_index][width_index].green;
            output[heigth_index][width_index].blue = input[heigth_index][width_index].blue;
        }
}

float get_pixel_val(PIXEL pixel) {
    return (0.9 * pixel.red + 0.05 * pixel.green + 0.05 * pixel.blue) / 3;
}

float get_img_average(int heigth, int width, PIXEL pixel_table[heigth][width]) {
    float sum = 0;

    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--)
        for (int width_index = 0; width_index < width; width_index++)
            sum += get_pixel_val(pixel_table[heigth_index][width_index]);

    return sum / (heigth * width);
}
            

void treshold(int heigth, int width, PIXEL pixel_table[heigth][width], float img_average) {
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


void flood_fill(int y, int x, int width, int heigth, unsigned char new_color, unsigned char old_color, PIXEL image[heigth][width]) {
    if (y >= 0 && y < heigth && x >= 0 && y < width && image[y][x].red == old_color) {
        image[y][x].red = new_color;
        image[y][x].green = new_color;
        image[y][x].blue = new_color;

        flood_fill(y - 1, x, width, heigth, new_color, old_color, image);
        flood_fill(y + 1, x, width, heigth, new_color, old_color, image);
        flood_fill(y, x - 1, width, heigth, new_color, old_color, image);
        flood_fill(y, x + 1, width, heigth, new_color, old_color, image);
    }
}

#define FIT 2
#define HIT 1
#define OUT 0

int get_struct_element_status(int y, int x, int heigth, int width, PIXEL image[heigth][width], int struct_element_size) {
    if (!(y < heigth && y > 0 && x < width && x > 0))
        return OUT;

    int count = 0;

    for (int y_index = y; y_index < y + struct_element_size; y_index++)
        for (int x_index = x; x_index < x + struct_element_size; x_index++) 
            if (image[y_index][x_index].red == 255)
                count++;

    if (count == struct_element_size * struct_element_size)
        return FIT;
    else if (count > 0)
        return HIT;
    else 
        return OUT;

    /*

    if (image[y][x].red == struct_element[1][1] && image[y][x - 1].red == struct_element[1][0] && image[y][x + 1].red == struct_element[1][2] && 
        image[y - 1][x].red == struct_element[0][1] && image[y - 1][x - 1].red == struct_element[0][0] && image[y - 1][x + 1].red == struct_element[0][2] &&
        image[y + 1][x].red == struct_element[2][1] && image[y + 1][x - 1].red == struct_element[2][0] && image[y + 1][x + 1].red == struct_element[2][2])
        return FIT;
    else if (image[y][x].red == struct_element[1][1] || image[y][x - 1].red == struct_element[1][0] || image[y][x + 1].red == struct_element[1][2] || 
        image[y - 1][x].red == struct_element[0][1] || image[y - 1][x - 1].red == struct_element[0][0] || image[y - 1][x + 1].red == struct_element[0][2] ||
        image[y + 1][x].red == struct_element[2][1] || image[y + 1][x - 1].red == struct_element[2][0] || image[y + 1][x + 1].red == struct_element[2][2])
        return HIT;
    else
        return OUT;
        */
}

void erosion(int heigth, int width, PIXEL input[heigth][width], PIXEL output[heigth][width], int struct_element_size) {
    for (int heigth_index = 0; heigth_index < heigth; heigth_index++)
        for (int width_index = 0; width_index < width; width_index++) {
            if (input[heigth_index][width_index].red == 255) {
                if (get_struct_element_status(heigth_index, width_index, heigth, width, input, struct_element_size) != FIT) {
                    output[heigth_index][width_index].red = 0;      
                    output[heigth_index][width_index].green = 0;      
                    output[heigth_index][width_index].blue = 0;      
                }
            }
        }
}

void dilation(int heigth, int width, PIXEL input[heigth][width], PIXEL output[heigth][width], int struct_element_size) {
    for (int heigth_index = 0; heigth_index < heigth; heigth_index++)
        for (int width_index = 0; width_index < width; width_index++) {
            if (input[heigth_index][width_index].red == 255) {
                if (get_struct_element_status(heigth_index, width_index, heigth, width, input, struct_element_size) == HIT) {
                    for (int y_index = heigth_index - 1; y_index <= heigth_index + 1 && y_index < heigth && y_index >= 0; y_index++)
                        for (int x_index = width_index - 1; x_index <= width_index + 1 && x_index < width && x_index >= 0; x_index++) {
                            output[y_index][x_index].red = 255;
                            output[y_index][x_index].green = 255;
                            output[y_index][x_index].blue = 255;
                        }
                }
            }
        }
}

void extra_dilation(int heigth, int width, PIXEL input[heigth][width], PIXEL output[heigth][width], int struct_element_size) {
    PIXEL output_1[heigth][width];
    PIXEL output_2[heigth][width];

    dilation(heigth, width, input, output_1, struct_element_size);
    dilation(heigth, width, output_1, output_2, struct_element_size);
    dilation(heigth, width, output_2, output, struct_element_size);
}

void closing(int heigth, int width, PIXEL input[heigth][width], PIXEL output[heigth][width], int struct_element_size) {
    PIXEL dilation_output[heigth][width];
    copy_image(heigth, width, input, dilation_output);

    dilation(heigth, width, input, dilation_output, struct_element_size);

    erosion(heigth, width, dilation_output, output, struct_element_size);
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

    PIXEL image[image_header.heigth][image_header.width];
    PIXEL image_copy[image_header.heigth][image_header.width];

    unsigned int image_offset = file_header.image_data_offset;

    read_image(image_header.heigth, image_header.width, image, image_offset, input);

    float img_average = get_img_average(image_header.heigth, image_header.width, image);

    treshold(image_header.heigth, image_header.width, image, img_average);

    copy_image(image_header.heigth, image_header.width, image, image_copy);

    closing(image_header.heigth, image_header.width, image_copy, image, 15);

//    flood_fill(200, 30, image_header.width, image_header.heigth, 100, 0, image); 

//    dilation(image_header.heigth, image_header.width, image_copy, image, 3);
/*    for (int x_index = 0; x_index < image_header.width; x_index++)
        for (int y_index = 0; y_index < image_header.heigth; y_index++)
            if (get_pixel_val(image[y_index][x_index]) == 0) {
                image[y_index][x_index].red = 255;
                image[y_index][x_index].green = 255;
                image[y_index][x_index].blue = 255;
            }
*/
    write_image(image_header.heigth, image_header.width, image, image_offset, output);

    fclose(input);
    fclose(output);

    return 0;
}