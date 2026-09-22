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

    /*
     * Converte para RGBA32 para garantir
     * um formato conhecido durante todo
     * o processamento dos pixels.
     */
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

    /*
     * Protecao contra dimensoes invalidas.
     */
    if (image->surface->w <= 0 ||
        image->surface->h <= 0)
    {
        fprintf(
            stderr,
            "Erro: a imagem possui dimensoes invalidas.\n"
        );

        SDL_DestroySurface(
            image->surface
        );

        image->surface = NULL;

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


bool image_check_grayscale(
    const Image *image,
    bool *is_grayscale
)
{
    if (image == NULL ||
        image->surface == NULL ||
        is_grayscale == NULL)
    {
        fprintf(
            stderr,
            "Erro: dados invalidos para verificar escala de cinza.\n"
        );

        return false;
    }

    SDL_Surface *surface =
        image->surface;

    if (!SDL_LockSurface(surface))
    {
        fprintf(
            stderr,
            "Erro ao acessar pixels para verificar escala de cinza: %s\n",
            SDL_GetError()
        );

        return false;
    }

    /*
     * Assume inicialmente que a imagem
     * esta em escala de cinza.
     */
    *is_grayscale = true;

    for (int y = 0;
         y < surface->h &&
         *is_grayscale;
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

            /*
             * Em uma imagem em escala
             * de cinza:
             *
             * R == G == B
             */
            if (r != g ||
                g != b)
            {
                *is_grayscale = false;
                break;
            }
        }
    }

    SDL_UnlockSurface(
        surface
    );

    return true;
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

            /*
             * Formula definida no enunciado:
             *
             * Y = 0.2125R +
             *     0.7154G +
             *     0.0721B
             */
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
             * pixel[3] corresponde ao alpha
             * e permanece inalterado.
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
     * Cria uma copia independente da imagem
     * original em escala de cinza.
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
     * Calcula o histograma atual.
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

    /*
     * Protecao contra divisao por zero.
     */
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
     * Calcula a CDF.
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
            cdf_min =
                cdf[i];

            break;
        }
    }

    /*
     * Uma imagem uniforme possui somente
     * uma intensidade e nao precisa ser
     * modificada pela equalizacao.
     */
    if (total_pixels == cdf_min)
    {
        SDL_UnlockSurface(
            surface
        );

        image->equalized =
            true;

        printf(
            "Equalizacao nao alterou a imagem: intensidades uniformes.\n"
        );

        return true;
    }

    /*
     * Calcula a tabela de transformacao.
     */
    for (int i = 0;
         i < 256;
         i++)
    {
        if (cdf[i] < cdf_min)
        {
            transformation[i] =
                0;

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
            normalized *
            255.0;

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
     * Aplica a transformacao.
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
        }
    }

    SDL_UnlockSurface(
        surface
    );

    image->equalized =
        true;

    printf(
        "Equalizacao do histograma concluida.\n"
    );

    return true;
}


bool image_restore_original(
    Image *image
)
{
    if (image == NULL ||
        image->original_surface == NULL)
    {
        fprintf(
            stderr,
            "Erro: imagem original nao disponivel para restauracao.\n"
        );

        return false;
    }

    /*
     * Cria uma nova copia da original,
     * preservando original_surface.
     */
    SDL_Surface *restored_surface =
        SDL_ConvertSurface(
            image->original_surface,
            SDL_PIXELFORMAT_RGBA32
        );

    if (restored_surface == NULL)
    {
        fprintf(
            stderr,
            "Erro ao restaurar imagem original: %s\n",
            SDL_GetError()
        );

        return false;
    }

    /*
     * Somente depois de conseguir criar
     * a nova superficie destruimos a atual.
     *
     * Assim evitamos perder a imagem caso
     * a copia falhe.
     */
    if (image->surface != NULL)
    {
        SDL_DestroySurface(
            image->surface
        );
    }

    image->surface =
        restored_surface;

    image->equalized =
        false;

    printf(
        "Imagem original restaurada.\n"
    );

    return true;
}


bool image_save_png(
    Image *image,
    int output_width,
    int output_height,
    const char *filename
)
{
    if (image == NULL ||
        image->surface == NULL ||
        filename == NULL ||
        output_width <= 0 ||
        output_height <= 0)
    {
        fprintf(
            stderr,
            "Erro: dados invalidos para salvar imagem.\n"
        );

        return false;
    }

    /*
     * Verifica se o arquivo ja existia.
     */
    bool file_already_exists =
        false;

    FILE *existing_file =
        fopen(
            filename,
            "rb"
        );

    if (existing_file != NULL)
    {
        file_already_exists =
            true;

        fclose(
            existing_file
        );
    }

    SDL_Surface *surface_to_save =
        image->surface;

    SDL_Surface *resized_surface =
        NULL;

    /*
     * Caso a resolucao exibida seja
     * diferente da resolucao real da imagem,
     * cria uma superficie temporaria.
     */
    if (image->surface->w != output_width ||
        image->surface->h != output_height)
    {
        resized_surface =
            SDL_CreateSurface(
                output_width,
                output_height,
                SDL_PIXELFORMAT_RGBA32
            );

        if (resized_surface == NULL)
        {
            fprintf(
                stderr,
                "Erro ao criar superficie para salvamento: %s\n",
                SDL_GetError()
            );

            return false;
        }

        SDL_Rect destination_rect = {
            .x = 0,
            .y = 0,
            .w = output_width,
            .h = output_height
        };

        if (!SDL_BlitSurfaceScaled(
                image->surface,
                NULL,
                resized_surface,
                &destination_rect,
                SDL_SCALEMODE_LINEAR))
        {
            fprintf(
                stderr,
                "Erro ao redimensionar imagem para salvamento: %s\n",
                SDL_GetError()
            );

            SDL_DestroySurface(
                resized_surface
            );

            return false;
        }

        surface_to_save =
            resized_surface;
    }

    /*
     * Salva a imagem PNG.
     */
    if (!IMG_SavePNG(
            surface_to_save,
            filename))
    {
        fprintf(
            stderr,
            "Erro ao salvar '%s': %s\n",
            filename,
            SDL_GetError()
        );

        if (resized_surface != NULL)
        {
            SDL_DestroySurface(
                resized_surface
            );
        }

        return false;
    }

    /*
     * Libera a superficie temporaria,
     * caso ela tenha sido criada.
     */
    if (resized_surface != NULL)
    {
        SDL_DestroySurface(
            resized_surface
        );
    }

    if (file_already_exists)
    {
        printf(
            "Arquivo %s sobrescrito com sucesso.\n",
            filename
        );
    }
    else
    {
        printf(
            "Arquivo %s criado com sucesso.\n",
            filename
        );
    }

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

        image->surface =
            NULL;
    }

    if (image->original_surface != NULL)
    {
        SDL_DestroySurface(
            image->original_surface
        );

        image->original_surface =
            NULL;
    }

    image->equalized =
        false;
}
