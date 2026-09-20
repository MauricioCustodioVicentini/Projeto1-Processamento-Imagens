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

bool image_is_grayscale(const Image *image)
{
    if (image == NULL || image->surface == NULL)
    {
        return false;
    }

    SDL_Surface *surface = image->surface;

    if (!SDL_LockSurface(surface))
    {
        fprintf(
            stderr,
            "Erro ao acessar os pixels da imagem: %s\n",
            SDL_GetError()
        );

        return false;
    }

    bool is_grayscale = true;

    for (int y = 0; y < surface->h && is_grayscale; y++)
    {
        Uint8 *row = (Uint8 *)surface->pixels + y * surface->pitch;

        for (int x = 0; x < surface->w; x++)
        {
            Uint8 *pixel = row + x * 4;

            Uint8 r = pixel[0];
            Uint8 g = pixel[1];
            Uint8 b = pixel[2];

            if (r != g || g != b)
            {
                is_grayscale = false;
                break;
            }
        }
    }

    SDL_UnlockSurface(surface);

    return is_grayscale;
}

bool image_convert_to_grayscale(Image *image)
{
    if (image == NULL || image->surface == NULL)
    {
        fprintf(stderr, "Erro: imagem invalida para conversao.\n");
        return false;
    }

    SDL_Surface *surface = image->surface;

    if (!SDL_LockSurface(surface))
    {
        fprintf(
            stderr,
            "Erro ao acessar os pixels da imagem: %s\n",
            SDL_GetError()
        );

        return false;
    }

    for (int y = 0; y < surface->h; y++)
    {
        Uint8 *row = (Uint8 *)surface->pixels + y * surface->pitch;

        for (int x = 0; x < surface->w; x++)
        {
            Uint8 *pixel = row + x * 4;

            Uint8 r = pixel[0];
            Uint8 g = pixel[1];
            Uint8 b = pixel[2];

            double luminance =
                0.2125 * r +
                0.7154 * g +
                0.0721 * b;

            Uint8 gray = (Uint8)(luminance + 0.5);

            pixel[0] = gray;
            pixel[1] = gray;
            pixel[2] = gray;

            /*
             * pixel[3] corresponde ao canal alpha.
             * Ele é preservado.
             */
        }
    }

    SDL_UnlockSurface(surface);

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
