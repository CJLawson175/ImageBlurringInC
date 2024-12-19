//IMPORTANT!!
//Before using this program ensure that the file paths are correct
//for line 11, 13, and 132


#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "C:/Users/Charl/Downloads/challenge/utils/stb_image.h"        // Path to stb_image.h (edit for own machine)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "C:/Users/Charl/Downloads/challenge/utils/stb_image_write.h"  // Path to stb_image_write.h (edit for own machine)

struct filter {
    int size_x, size_y;
    float **kernel;
};

// Create a multi-dimensional array for storing image data
float *create_multiarray(int w, int h) {
    float *img = malloc(w * h * sizeof(float));  // 1D array to store all pixel values
    return img;
}

// Convert 1D image array to 2D-like processing
void from1Dto2D(float *img, unsigned char *arr, int w, int h, int c) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int ch = 0; ch < c; ch++) {
                img[(i * w + j) * c + ch] = arr[(i * w + j) * c + ch];
            }
        }
    }
}

// Perform the convolution operation on the image for each color channel
void convolution(float *new, float *A, int w, int h, struct filter filter) {
    int filter_offset_x = filter.size_x / 2;
    int filter_offset_y = filter.size_y / 2;

    // Apply convolution
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            float sum = 0;
            for (int fx = 0; fx < filter.size_x; fx++) {
                for (int fy = 0; fy < filter.size_y; fy++) {
                    int px = j + fx - filter_offset_x;
                    int py = i + fy - filter_offset_y;
                    if (px >= 0 && px < w && py >= 0 && py < h) {
                        sum += A[py * w + px] * filter.kernel[fy][fx];
                    }
                }
            }
            new[i * w + j] = sum;
        }
    }
}

// Apply the blurring filter to the image
void blurring(float *new, float *img, int w, int h, int filter_size, int c) {
    struct filter filter;
    filter.size_x = filter_size;
    filter.size_y = filter_size;

    // Create the filter
    filter.kernel = malloc(filter.size_y * sizeof(float *));
    for (int i = 0; i < filter.size_y; i++) {
        filter.kernel[i] = malloc(filter.size_x * sizeof(float));
    }

    // Fill the filter with average values
    for (int i = 0; i < filter_size; i++) {
        for (int j = 0; j < filter_size; j++) {
            filter.kernel[i][j] = 1.0 / (filter_size * filter_size);
        }
    }

    // Apply the convolution to each color channel
    for (int channel = 0; channel < c; channel++) {
        // Create a temporary image for the current channel
        float *channel_img = malloc(w * h * sizeof(float));
        float *channel_out = malloc(w * h * sizeof(float));

        // Extract the channel data from the original image
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                channel_img[i * w + j] = img[(i * w + j) * c + channel];
            }
        }

        // Perform convolution for this channel
        convolution(channel_out, channel_img, w, h, filter);

        // Store the result back into the new image (for each channel)
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                new[(i * w + j) * c + channel] = channel_out[i * w + j];
            }
        }

        free(channel_img);
        free(channel_out);
    }

    // Free the filter memory
    for (int i = 0; i < filter.size_y; i++) {
        free(filter.kernel[i]);
    }
    free(filter.kernel);
}

// Save the image as a JPG file
void save_figure(float *img, int w, int h, int c, char* name) {
    int size = w * h * c;
    unsigned char *arr = calloc(size, sizeof(unsigned char));
    int idx = 0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            for (int temp = 0; temp < c; temp++) {
                arr[idx] = (unsigned char) img[(i * w + j) * c + temp];
                idx++;
            }
        }
    }
    stbi_write_jpg(name, w, h, c, arr, w * c);
    free(arr);
}

int main() {
    int w, h, c;
    char filename[] = "C:/Users/Charl/Downloads/challenge/data/lena.png"; // Path to the image (ensure it is correct for your machine)
    int filter_size;

    printf("Enter the filter size (odd number): ");
    scanf("%d", &filter_size);

    if (filter_size % 2 == 0) {
        printf("Filter size must be an odd number.\n");
        return 1;
    }

    // Load the image
    unsigned char *arr = stbi_load(filename, &w, &h, &c, 0);
    if (!arr) {
        printf("Failed to load image %s\n", filename);
        return 1;
    }

    // Create a 1D array to store the image (for all color channels)
    float *img = malloc(w * h * c * sizeof(float));
    from1Dto2D(img, arr, w, h, c);

    // Create an output image array
    float *blur = malloc(w * h * c * sizeof(float));

    // Apply the blurring filter
    blurring(blur, img, w, h, filter_size, c);

    // Save the filtered image
    save_figure(blur, w, h, c, "blurred_image.jpg");

    // Clean up
    stbi_image_free(arr);
    free(img);
    free(blur);

    printf("Image processed and saved as blurred_image.jpg\n");
    return 0;
}
