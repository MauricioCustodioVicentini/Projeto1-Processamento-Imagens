#include "histogram.h"

#include <math.h>
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
    histogram->mean = 0.0;
    histogram->standard_deviation = 0.0;

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
             * Como a imagem ja esta em escala de cinza,
             * R = G = B.
             */
            Uint8 intensity = pixel[0];

            histogram->bins[intensity]++;
            histogram->total_pixels++;
        }
    }

    SDL_UnlockSurface(surface);

    if (histogram->total_pixels == 0)
    {
        fprintf(
            stderr,
            "Erro: imagem sem pixels para analise.\n"
        );

        return false;
    }

    /*
     * Descobre a maior frequencia do histograma
     * e calcula a soma ponderada das intensidades.
     */
    double intensity_sum = 0.0;

    for (int i = 0; i < HISTOGRAM_LEVELS; i++)
    {
        if (histogram->bins[i] > histogram->max_count)
        {
            histogram->max_count = histogram->bins[i];
        }

        intensity_sum +=
            (double)i *
            (double)histogram->bins[i];
    }

    /*
     * Media de intensidade.
     */
    histogram->mean =
        intensity_sum /
        (double)histogram->total_pixels;

    /*
     * Variancia baseada no histograma.
     */
    double variance_sum = 0.0;

    for (int i = 0; i < HISTOGRAM_LEVELS; i++)
    {
        double difference =
            (double)i - histogram->mean;

        variance_sum +=
            difference *
            difference *
            (double)histogram->bins[i];
    }

    double variance =
        variance_sum /
        (double)histogram->total_pixels;

    histogram->standard_deviation =
        sqrt(variance);

    return true;
}

const char *histogram_brightness_classification(
    const Histogram *histogram
)
{
    if (histogram == NULL)
    {
        return "indefinida";
    }

    /*
     * O intervalo 0-255 foi dividido
     * aproximadamente em tres partes iguais.
     */
    if (histogram->mean < 85.0)
    {
        return "escura";
    }

    if (histogram->mean < 171.0)
    {
        return "media";
    }

    return "clara";
}

const char *histogram_contrast_classification(
    const Histogram *histogram
)
{
    if (histogram == NULL)
    {
        return "indefinido";
    }

    /*
     * O desvio padrao para intensidades de 8 bits
     * pode chegar aproximadamente a 127,5.
     * O intervalo foi dividido em tres faixas.
     */
    if (histogram->standard_deviation < 42.5)
    {
        return "baixo";
    }

    if (histogram->standard_deviation < 85.0)
    {
        return "medio";
    }

    return "alto";
}
