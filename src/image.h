#ifndef IMAGE_H
#define IMAGE_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct
{
    SDL_Surface *surface;
    SDL_Surface *original_surface;

    bool equalized;

} Image;

bool image_load(
    const char *filename,
    Image *image
);

bool image_check_grayscale(
    const Image *image,
    bool *is_grayscale
);

bool image_convert_to_grayscale(
    Image *image
);

bool image_preserve_original(
    Image *image
);

bool image_equalize(
    Image *image
);

bool image_restore_original(
    Image *image
);

bool image_save_png(
    Image *image,
    int output_width,
    int output_height,
    const char *filename
);

void image_destroy(
    Image *image
);

#endif
