#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "histogram.h"
#include "image.h"
#include "window.h"

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

    if (!SDL_Init(SDL_INIT_VIDEO))
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

    AppWindow main_window = {
        .window = NULL,
        .renderer = NULL,
        .image_texture = NULL,
        .image_rect = {
            .x = 0.0f,
            .y = 0.0f,
            .w = 0.0f,
            .h = 0.0f
        },
        .id = 0
    };

    InfoWindow info_window = {
        .window = NULL,
        .renderer = NULL,
        .id = 0
    };

    if (!image_load(
            argv[1],
            &image))
    {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (image_is_grayscale(&image))
    {
        printf(
            "Imagem de entrada: escala de cinza.\n"
        );

        printf(
            "Conversao para escala de cinza nao necessaria.\n"
        );
    }
    else
    {
        printf(
            "Imagem de entrada: colorida.\n"
        );

        printf(
            "Convertendo para escala de cinza...\n"
        );

        if (!image_convert_to_grayscale(
                &image))
        {
            image_destroy(
                &image
            );

            SDL_Quit();

            return EXIT_FAILURE;
        }

        printf(
            "Conversao para escala de cinza concluida.\n"
        );
    }

    Histogram histogram;

    if (!histogram_calculate(
            &histogram,
            image.surface))
    {
        image_destroy(
            &image
        );

        SDL_Quit();

        return EXIT_FAILURE;
    }

    printf(
        "Histograma calculado com sucesso.\n"
    );

    printf(
        "Total de pixels: %llu\n",
        (unsigned long long)
        histogram.total_pixels
    );

    printf(
        "Maior frequencia do histograma: %llu\n",
        (unsigned long long)
        histogram.max_count
    );

    printf(
        "Media de intensidade: %.2f\n",
        histogram.mean
    );

    printf(
        "Classificacao da imagem: %s\n",
        histogram_brightness_classification(
            &histogram
        )
    );

    printf(
        "Desvio padrao: %.2f\n",
        histogram.standard_deviation
    );

    printf(
        "Classificacao do contraste: %s\n",
        histogram_contrast_classification(
            &histogram
        )
    );

    if (!window_initialize(
            &main_window))
    {
        image_destroy(
            &image
        );

        SDL_Quit();

        return EXIT_FAILURE;
    }

    if (!window_set_image(
            &main_window,
            image.surface))
    {
        window_destroy(
            &main_window
        );

        image_destroy(
            &image
        );

        SDL_Quit();

        return EXIT_FAILURE;
    }

    if (!info_window_initialize(
            &info_window,
            main_window.window))
    {
        window_destroy(
            &main_window
        );

        image_destroy(
            &image
        );

        SDL_Quit();

        return EXIT_FAILURE;
    }

    bool running = true;

    while (running)
    {
        SDL_Event event;

        while (SDL_PollEvent(
            &event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:

                    if (
                        event.window.windowID ==
                        main_window.id
                    )
                    {
                        running = false;
                    }
                    else if (
                        event.window.windowID ==
                        info_window.id
                    )
                    {
                        running = false;
                    }

                    break;

                default:
                    break;
            }
        }

        window_render(
            &main_window
        );

        info_window_render(
            &info_window
        );

        /*
         * Aproximadamente 60 ciclos por segundo.
         */
        SDL_Delay(16);
    }

    /*
     * A janela filha e destruida
     * antes da janela principal.
     */
    info_window_destroy(
        &info_window
    );

    window_destroy(
        &main_window
    );

    image_destroy(
        &image
    );

    SDL_Quit();

    return EXIT_SUCCESS;
}
