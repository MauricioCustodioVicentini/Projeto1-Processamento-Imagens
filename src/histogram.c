#include "histogram.h"

#include <stdio.h>
#include <string.h>

bool histogram_calculate(
    Histogram *histogram,
    SDL_Surface *surface
)
{
    if (histogram == NULL || surface == NULL)
    {
        fprintf(
            stderr,
            "Erro: dados invalidos para calcular o histograma.\n"
        );

        return false;
    }

    memset(histogram->bins, 0, sizeof(histogram->bins));

    histogram->total_pixels = 0;
    histogram->max_count = 0;

    if (!SDL_LockSurface(surface))
    {
        fprintf(
            stderr,
            "Erro ao acessar os pixels para calcular o histograma: %s\n",
            SDL_GetError()
        );

        return false;
    }

    for (int y = 0; y < surface->h; y++)
    {
        Uint8 *row =
            (Uint8 *)surface->pixels +
            y * surface->pitch;

        for (int x = 0; x < surface->w; x++)
        {
            Uint8 *pixel = row + x * 4;

            /*
             * A imagem ja esta em escala de cinza.
             * Portanto R = G = B.
             */
            Uint8 intensity = pixel[0];

            histogram->bins[intensity]++;
            histogram->total_pixels++;
        }
    }

    SDL_UnlockSurface(surface);

    for (int i = 0; i < HISTOGRAM_LEVELS; i++)
    {
        if (histogram->bins[i] > histogram->max_count)
        {
            histogram->max_count = histogram->bins[i];
        }
    }

    return true;
}
