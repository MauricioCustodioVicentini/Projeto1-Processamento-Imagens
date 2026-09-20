#include "image.h"

#include <stdio.h>
#include <SDL3_image/SDL_image.h>

bool image_load(const char *filename, Image *image)
{
    if (filename == NULL || image == NULL)
    {
        fprintf(stderr, "Erro: parametros invalidos para carregamento da imagem.\n");
        return false;
    }

    image->surface = NULL;

    SDL_Surface *loaded_surface = IMG_Load(filename);

    if (loaded_surface == NULL)
    {
        fprintf(
            stderr,
            "Erro ao carregar a imagem '%s': %s\n",
            filename,
            SDL_GetError()
        );

        return false;
    }

    image->surface = SDL_ConvertSurface(
        loaded_surface,
        SDL_PIXELFORMAT_RGBA32
    );

    SDL_DestroySurface(loaded_surface);

    if (image->surface == NULL)
    {
        fprintf(
            stderr,
            "Erro ao converter a imagem para RGBA32: %s\n",
            SDL_GetError()
        );

        return false;
    }

    printf("Imagem carregada com sucesso: %s\n", filename);
    printf(
        "Dimensoes: %d x %d pixels\n",
        image->surface->w,
        image->surface->h
    );

    return true;
}

void image_destroy(Image *image)
{
    if (image == NULL)
    {
        return;
    }

    if (image->surface != NULL)
    {
        SDL_DestroySurface(image->surface);
        image->surface = NULL;
    }
}
