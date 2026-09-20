#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "image.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(
            stderr,
            "Uso: %s caminho_da_imagem.ext\n",
            argv[0]
        );

        return EXIT_FAILURE;
    }

    if (!SDL_Init(0))
    {
        fprintf(
            stderr,
            "Erro ao inicializar a SDL: %s\n",
            SDL_GetError()
        );

        return EXIT_FAILURE;
    }

    Image image = {
        .surface = NULL
    };

    if (!image_load(argv[1], &image))
    {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (image_is_grayscale(&image))
    {
        printf("Imagem de entrada: escala de cinza.\n");
        printf("Conversao para escala de cinza nao necessaria.\n");
    }
    else
    {
        printf("Imagem de entrada: colorida.\n");
        printf("Convertendo para escala de cinza...\n");

        if (!image_convert_to_grayscale(&image))
        {
            image_destroy(&image);
            SDL_Quit();

            return EXIT_FAILURE;
        }

        printf("Conversao para escala de cinza concluida.\n");
    }

    image_destroy(&image);

    SDL_Quit();

    return EXIT_SUCCESS;
}
