#include "histogram.h"

#include <math.h>
#include <stdio.h>
#include <string.h>


bool histogram_calculate(
    Histogram *histogram,
    SDL_Surface *surface
)
{
    if (histogram == NULL ||
        surface == NULL)
    {
        fprintf(
            stderr,
            "Erro: dados invalidos para calcular o histograma.\n"
        );

        return false;
    }

    memset(
        histogram->bins,
        0,
        sizeof(histogram->bins)
    );

    histogram->total_pixels =
        0;

    histogram->max_count =
        0;

    histogram->mean =
        0.0;

    histogram->standard_deviation =
        0.0;

    if (surface->w <= 0 ||
        surface->h <= 0)
    {
        fprintf(
            stderr,
            "Erro: superficie possui dimensoes invalidas.\n"
        );

        return false;
    }

    if (!SDL_LockSurface(
            surface))
    {
        fprintf(
            stderr,
            "Erro ao acessar os pixels para calcular o histograma: %s\n",
            SDL_GetError()
        );

        return false;
    }

    /*
     * Calcula as 256 frequencias.
     *
     * Como a imagem ja esta em escala
     * de cinza, basta utilizar um dos
     * canais RGB.
     */
    for (int y = 0;
         y < surface->h;
         y++)
    {
        Uint8 *row =
            (Uint8 *)surface->pixels +
            y * surface->pitch;

        for (int x = 0;
             x < surface->w;
             x++)
        {
            Uint8 *pixel =
                row + x * 4;

            Uint8 intensity =
                pixel[0];

            histogram->bins[
                intensity
            ]++;

            histogram->total_pixels++;
        }
    }

    SDL_UnlockSurface(
        surface
    );

    /*
     * Evita qualquer possibilidade
     * de divisao por zero.
     */
    if (histogram->total_pixels == 0)
    {
        fprintf(
            stderr,
            "Erro: nao existem pixels para analisar.\n"
        );

        return false;
    }

    /*
     * Calcula a maior frequencia.
     *
     * Esse valor e usado para normalizar
     * a exibicao visual do histograma.
     */
    for (int i = 0;
         i < HISTOGRAM_LEVELS;
         i++)
    {
        if (histogram->bins[i] >
            histogram->max_count)
        {
            histogram->max_count =
                histogram->bins[i];
        }
    }

    /*
     * Media das intensidades.
     */
    double intensity_sum =
        0.0;

    for (int i = 0;
         i < HISTOGRAM_LEVELS;
         i++)
    {
        intensity_sum +=
            (double)i *
            (double)histogram->bins[i];
    }

    histogram->mean =
        intensity_sum /
        (double)histogram->total_pixels;

    /*
     * Variancia.
     */
    double variance_sum =
        0.0;

    for (int i = 0;
         i < HISTOGRAM_LEVELS;
         i++)
    {
        double difference =
            (double)i -
            histogram->mean;

        variance_sum +=
            difference *
            difference *
            (double)histogram->bins[i];
    }

    double variance =
        variance_sum /
        (double)histogram->total_pixels;

    /*
     * Desvio padrao.
     */
    histogram->standard_deviation =
        sqrt(
            variance
        );

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
     * Faixa 0 a 255 dividida
     * aproximadamente em tres partes.
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
     * Limites definidos pelo grupo
     * para classificar o desvio padrao.
     */
    if (histogram->standard_deviation <
        42.5)
    {
        return "baixo";
    }

    if (histogram->standard_deviation <
        85.0)
    {
        return "medio";
    }

    return "alto";
}
