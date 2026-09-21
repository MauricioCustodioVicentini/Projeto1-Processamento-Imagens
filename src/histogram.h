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

    double mean;
    double standard_deviation;

} Histogram;

bool histogram_calculate(
    Histogram *histogram,
    SDL_Surface *surface
);

const char *histogram_brightness_classification(
    const Histogram *histogram
);

const char *histogram_contrast_classification(
    const Histogram *histogram
);

#endif
