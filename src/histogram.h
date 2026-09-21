#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <stdbool.h>
#include <SDL3/SDL.h>

#define HISTOGRAM_LEVELS 256

typedef struct
{
    Uint64 bins[HISTOGRAM_LEVELS];
    Uint64 total_pixels;
    Uint64 max_count;
} Histogram;

bool histogram_calculate(
    Histogram *histogram,
    SDL_Surface *surface
);

#endif
