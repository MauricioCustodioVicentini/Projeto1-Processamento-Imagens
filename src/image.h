#ifndef IMAGE_H
#define IMAGE_H

#include <stdbool.h>
#include <SDL3/SDL.h>

typedef struct
{
    SDL_Surface *surface;

    /*
     * Copia preservada da imagem original
     * em escala de cinza.
     *
     * Sera usada posteriormente para
     * retornar a imagem ao estado original
     * sem recarregar o arquivo.
     */
    SDL_Surface *original_surface;

    bool equalized;

} Image;

bool image_load(
    const char *filename,
    Image *image
);

bool image_is_grayscale(
    const Image *image
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

void image_destroy(
    Image *image
);

#endif
