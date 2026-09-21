#include "image.h"

#include <stdio.h>

#include <SDL3_image/SDL_image.h>

bool image_load(
    const char *filename,
    Image *image
)
{
    if (filename == NULL ||
        image == NULL)
    {
        fprintf(
            stderr,
            "Erro: parametros invalidos para carregamento da imagem.\n"
        );

        return false;
    }

    image->surface = NULL;
    image->original_surface = NULL;
    image->equalized = false;

    SDL_Surface *loaded_surface =
        IMG_Load(filename);

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

    image->surface =
        SDL_ConvertSurface(
            loaded_surface,
            SDL_PIXELFORMAT_RGBA32
        );

    SDL_DestroySurface(
        loaded_surface
    );

    if (image->surface == NULL)
    {
        fprintf(
            stderr,
            "Erro ao converter a imagem para RGBA32: %s\n",
            SDL_GetError()
        );

        return false;
    }

    printf(
        "Imagem carregada com sucesso: %s\n",
        filename
    );

    printf(
        "Dimensoes: %d x %d pixels\n",
        image->surface->w,
        image->surface->h
    );

    return true;
}

bool image_is_grayscale(
    const Image *image
)
{
    if (image == NULL ||
        image->surface == NULL)
    {
        return false;
    }

    SDL_Surface *surface =
        image->surface;

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

    for (int y = 0;
         y < surface->h &&
         is_grayscale;
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

            Uint8 r = pixel[0];
            Uint8 g = pixel[1];
            Uint8 b = pixel[2];

            if (r != g ||
                g != b)
            {
                is_grayscale = false;
                break;
            }
        }
    }

    SDL_UnlockSurface(
        surface
    );

    return is_grayscale;
}

bool image_convert_to_grayscale(
    Image *image
)
{
    if (image == NULL ||
        image->surface == NULL)
    {
        fprintf(
            stderr,
            "Erro: imagem invalida para conversao.\n"
        );

        return false;
    }

    SDL_Surface *surface =
        image->surface;

    if (!SDL_LockSurface(surface))
    {
        fprintf(
            stderr,
            "Erro ao acessar os pixels da imagem: %s\n",
            SDL_GetError()
        );

        return false;
    }

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

            Uint8 r = pixel[0];
            Uint8 g = pixel[1];
            Uint8 b = pixel[2];

            double luminance =
                0.2125 * r +
                0.7154 * g +
                0.0721 * b;

            Uint8 gray =
                (Uint8)(
                    luminance + 0.5
                );

            pixel[0] = gray;
            pixel[1] = gray;
            pixel[2] = gray;

            /*
             * pixel[3] e o canal alpha.
             * Ele permanece inalterado.
             */
        }
    }

    SDL_UnlockSurface(
        surface
    );

    return true;
}

bool image_preserve_original(
    Image *image
)
{
    if (image == NULL ||
        image->surface == NULL)
    {
        fprintf(
            stderr,
            "Erro: imagem invalida para preservar original.\n"
        );

        return false;
    }

    if (image->original_surface != NULL)
    {
        SDL_DestroySurface(
            image->original_surface
        );

        image->original_surface = NULL;
    }

    /*
     * Cria uma nova superficie independente
     * contendo a imagem atual.
     *
     * Neste ponto do programa, a imagem atual
     * ja deve estar em escala de cinza.
     */
    image->original_surface =
        SDL_ConvertSurface(
            image->surface,
            SDL_PIXELFORMAT_RGBA32
        );

    if (image->original_surface == NULL)
    {
        fprintf(
            stderr,
            "Erro ao preservar imagem original: %s\n",
            SDL_GetError()
        );

        return false;
    }

    return true;
}

bool image_equalize(
    Image *image
)
{
    if (image == NULL ||
        image->surface == NULL)
    {
        fprintf(
            stderr,
            "Erro: imagem invalida para equalizacao.\n"
        );

        return false;
    }

    SDL_Surface *surface =
        image->surface;

    Uint64 histogram[256] = {0};
    Uint64 cdf[256] = {0};
    Uint8 transformation[256] = {0};

    if (!SDL_LockSurface(surface))
    {
        fprintf(
            stderr,
            "Erro ao acessar pixels para equalizacao: %s\n",
            SDL_GetError()
        );

        return false;
    }

    /*
     * Etapa 1:
     * calcula o histograma da imagem atual.
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

            histogram[intensity]++;
        }
    }

    Uint64 total_pixels =
        (Uint64)surface->w *
        (Uint64)surface->h;

    if (total_pixels == 0)
    {
        SDL_UnlockSurface(
            surface
        );

        fprintf(
            stderr,
            "Erro: imagem sem pixels para equalizacao.\n"
        );

        return false;
    }

    /*
     * Etapa 2:
     * calcula a funcao de distribuicao
     * acumulada (CDF).
     */
    cdf[0] =
        histogram[0];

    for (int i = 1;
         i < 256;
         i++)
    {
        cdf[i] =
            cdf[i - 1] +
            histogram[i];
    }

    /*
     * Localiza o primeiro valor nao nulo
     * da distribuicao acumulada.
     */
    Uint64 cdf_min = 0;

    for (int i = 0;
         i < 256;
         i++)
    {
        if (histogram[i] != 0)
        {
            cdf_min = cdf[i];
            break;
        }
    }

    /*
     * Caso especial:
     * uma imagem completamente uniforme
     * possui apenas uma intensidade.
     *
     * Nesse caso nao existe faixa para
     * expandir por equalizacao.
     */
    if (total_pixels == cdf_min)
    {
        SDL_UnlockSurface(
            surface
        );

        image->equalized = true;

        printf(
            "Equalizacao nao alterou a imagem: intensidades uniformes.\n"
        );

        return true;
    }

    /*
     * Etapa 3:
     * monta a tabela de transformacao.
     */
    for (int i = 0;
         i < 256;
         i++)
    {
        if (cdf[i] < cdf_min)
        {
            transformation[i] = 0;
            continue;
        }

        double normalized =
            (double)(
                cdf[i] -
                cdf_min
            ) /
            (double)(
                total_pixels -
                cdf_min
            );

        double new_value =
            normalized * 255.0;

        if (new_value < 0.0)
        {
            new_value = 0.0;
        }

        if (new_value > 255.0)
        {
            new_value = 255.0;
        }

        transformation[i] =
            (Uint8)(
                new_value + 0.5
            );
    }

    /*
     * Etapa 4:
     * aplica a nova intensidade
     * em cada pixel.
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

            Uint8 equalized_value =
                transformation[
                    intensity
                ];

            pixel[0] =
                equalized_value;

            pixel[1] =
                equalized_value;

            pixel[2] =
                equalized_value;

            /*
             * O canal alpha permanece
             * inalterado.
             */
        }
    }

    SDL_UnlockSurface(
        surface
    );

    image->equalized = true;

    printf(
        "Equalizacao do histograma concluida.\n"
    );

    return true;
}

void image_destroy(
    Image *image
)
{
    if (image == NULL)
    {
        return;
    }

    if (image->surface != NULL)
    {
        SDL_DestroySurface(
            image->surface
        );

        image->surface = NULL;
    }

    if (image->original_surface != NULL)
    {
        SDL_DestroySurface(
            image->original_surface
        );

        image->original_surface = NULL;
    }

    image->equalized = false;
}
