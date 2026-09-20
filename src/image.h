#ifndef IMAGE_H
#define IMAGE_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct
{
    SDL_Surface *surface;
} Image;

bool image_load(const char *filename, Image *image);
void image_destroy(Image *image);

#endif
