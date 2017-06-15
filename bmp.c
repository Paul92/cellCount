#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

double get_pixel_val(PIXEL pixel) {
    return (0.9 * pixel.red + 0.05 * pixel.green + 0.05 * pixel.blue) / 3;
}

void convert_to_binary(int heigth, int width, PIXEL image[heigth][width], char binary[heigth][width]) {
    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--)
        for (int width_index = 0; width_index < width; width_index++)
            binary[heigth_index][width_index] = get_pixel_val(image[heigth_index][width_index]);
}

enum binary_colors {BLACK, WHITE};

void write_image(int heigth, int width, char image[heigth][width], unsigned int image_offset, FILE *output) {
    int subpixels_per_line = width * 3;
    int required_padding = (subpixels_per_line % 4) ? 4 - subpixels_per_line % 4 : 0;
    int padding_dump = 0;

    int gap = image_offset - 54;

    fwrite(&padding_dump, sizeof(unsigned char), gap, output);

    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--) {
        for (int width_index = 0; width_index < width; width_index++) { 
            if (image[heigth_index][width_index] == BLACK) {
                fputc(0, output);
                fputc(0, output);
                fputc(0, output);
            } else {
                fputc(255, output);
                fputc(255, output);
                fputc(255, output);
            }
        }

        fwrite(&padding_dump, sizeof(unsigned char), required_padding, output);
    }
}

double get_img_average(int heigth, int width, char image[heigth][width]) {
    double sum = 0;

    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--)
        for (int width_index = 0; width_index < width; width_index++)
            sum += (int)image[heigth_index][width_index];

    return sum / (heigth * width);
}

void treshold(int heigth, int width, char image[heigth][width], float img_average) {
    for (int heigth_index = heigth - 1; heigth_index >= 0; heigth_index--)
        for (int width_index = 0; width_index < width; width_index++) 
            if (image[heigth_index][width_index] < img_average) 
                image[heigth_index][width_index] = BLACK;
            else
                image[heigth_index][width_index] = WHITE;
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

enum struct_element_stats {FIT, HIT, OUT};

/*
#define FIT 2
#define HIT 1
#define OUT 0
*/

int get_struct_element_status(int y, int x, int heigth, int width, char image[heigth][width], int struct_element_size) {
    if (!(y < heigth && y > 0 && x < width && x > 0))
        return OUT;

    int count = 0;

    for (int y_index = y; y_index < y + struct_element_size; y_index++)
        for (int x_index = x; x_index < x + struct_element_size; x_index++) 
            if (image[y_index][x_index] == BLACK)
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

void erosion(int heigth, int width, char input[heigth][width], char output[heigth][width], int struct_element_size) {
    for (int heigth_index = 0; heigth_index < heigth; heigth_index++)
        for (int width_index = 0; width_index < width; width_index++)
            if (input[heigth_index][width_index] == BLACK) 
                if (get_struct_element_status(heigth_index, width_index, heigth, width, input, struct_element_size) != FIT) 
                    output[heigth_index][width_index] = WHITE;      
}

void dilation(int heigth, int width, char input[heigth][width], char output[heigth][width], int struct_element_size) {
    for (int heigth_index = 0; heigth_index < heigth; heigth_index++)
        for (int width_index = 0; width_index < width; width_index++) 
            if (input[heigth_index][width_index] == BLACK) 
                if (get_struct_element_status(heigth_index, width_index, heigth, width, input, struct_element_size) == HIT) 
                    for (int y_index = heigth_index - 1; y_index <= heigth_index + 1 && y_index < heigth && y_index >= 0; y_index++)
                        for (int x_index = width_index - 1; x_index <= width_index + 1 && x_index < width && x_index >= 0; x_index++) 
                            output[y_index][x_index] = BLACK;
}

void closing(int heigth, int width, char input[heigth][width], char output[heigth][width], int struct_element_size) {
    char dilation_output[heigth][width];
    memcpy(dilation_output, input, heigth * width);

    dilation(heigth, width, input, dilation_output, struct_element_size);

    erosion(heigth, width, dilation_output, output, struct_element_size);
}    

void opening(int heigth, int width, char input[heigth][width], char output[heigth][width], int struct_element_size) {
    char erosion_output[heigth][width];
    memcpy(erosion_output, input, heigth * width);

    erosion(heigth, width, input, erosion_output, struct_element_size);

    dilation(heigth, width, erosion_output, output, struct_element_size);
}

void debug(int heigth, int width, char input[heigth][width]) {
    for (int i = heigth - 1; i >= 0; i--)
        for (int j = 0; j < width; j++)
            printf("Pixel (%d, %d) : %d\n", i, j, input[i][j]);
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

    PIXEL read_output[image_header.heigth][image_header.width];
    char image[image_header.heigth][image_header.width];
    char image_copy[image_header.heigth][image_header.width];
    char copy[image_header.heigth][image_header.width];
    

    read_image(image_header.heigth, image_header.width, read_output, file_header.image_data_offset, input);

    convert_to_binary(image_header.heigth, image_header.width, read_output, image); 
//    debug(image_header.heigth, image_header.width, image);

    float img_average = get_img_average(image_header.heigth, image_header.width, image);

    treshold(image_header.heigth, image_header.width, image, img_average);

    char i1[image_header.heigth][image_header.width];
    char i2[image_header.heigth][image_header.width];
    char i3[image_header.heigth][image_header.width];
    char i4[image_header.heigth][image_header.width];

    memcpy(i1, image, image_header.heigth * image_header.width);
    memcpy(i2, image, image_header.heigth * image_header.width);
    memcpy(i3, image, image_header.heigth * image_header.width);
    memcpy(i4, image, image_header.heigth * image_header.width);

    dilation(image_header.heigth, image_header.width, i1, i2, 6);
    erosion(image_header.heigth, image_header.width, i2, i3, 6);
    erosion(image_header.heigth, image_header.width, i3, i4, 6);
    dilation(image_header.heigth, image_header.width, i4, image, 6);

    memcpy(image_copy, image, image_header.heigth * image_header.width);
    memcpy(copy, image, image_header.heigth * image_header.width);

//    closing(image_header.heigth, image_header.width, image_copy, copy, 6);
//    erosion(image_header.heigth, image_header.width, copy, image, 3);

//    flood_fill(200, 30, image_header.width, image_header.heigth, 100, 0, image); 

/*    for (int x_index = 0; x_index < image_header.width; x_index++)
        for (int y_index = 0; y_index < image_header.heigth; y_index++)
            if (get_pixel_val(image[y_index][x_index]) == 0) {
                image[y_index][x_index].red = 255;
                image[y_index][x_index].green = 255;
                image[y_index][x_index].blue = 255;
            }
*/
    write_image(image_header.heigth, image_header.width, image, file_header.image_data_offset, output);

//    debug(image_header.heigth, image_header.width, image);


    fclose(input);
    fclose(output);

    return 0;
}
