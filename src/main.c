#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

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
        .surface = NULL,
        .original_surface = NULL,
        .equalized = false
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

    /*
     * Carrega a imagem recebida
     * pela linha de comando.
     */
    if (!image_load(
            argv[1],
            &image))
    {
        SDL_Quit();

        return EXIT_FAILURE;
    }

    /*
     * Verifica se a imagem ja esta
     * em escala de cinza.
     */
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

    /*
     * Preserva uma copia da imagem original
     * ja em escala de cinza.
     *
     * Essa copia sera utilizada posteriormente
     * para restaurar a imagem sem reler o arquivo.
     */
    if (!image_preserve_original(
            &image))
    {
        image_destroy(
            &image
        );

        SDL_Quit();

        return EXIT_FAILURE;
    }

    /*
     * Calcula o histograma inicial.
     */
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

    /*
     * Cria a janela principal.
     */
    if (!window_initialize(
            &main_window))
    {
        image_destroy(
            &image
        );

        SDL_Quit();

        return EXIT_FAILURE;
    }

    /*
     * Cria a textura inicial
     * da imagem em escala de cinza.
     */
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

    /*
     * Cria a janela secundaria
     * como filha da principal.
     */
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
            /*
             * Processa os eventos dos botoes
             * da janela secundaria.
             */
            InfoAction action =
                info_window_handle_event(
                    &info_window,
                    &event
                );

            /*
             * Botao de equalizacao.
             *
             * Neste commit ele equaliza apenas
             * uma vez. A restauracao da imagem
             * original sera implementada
             * no proximo commit.
             */
            if (action ==
                 INFO_ACTION_EQUALIZE)
            {
                /*
                * Se a imagem ainda nao estiver
                * equalizada, realiza a equalizacao.
                */
                if (!image.equalized)
                {
                    if (!image_equalize(
                            &image))
                    {
                        fprintf(
                            stderr,
                            "Erro ao equalizar imagem.\n"
                        );

                        running = false;
                        continue;
                    }
                }

                /*
                * Se ja estiver equalizada,
                * restaura a imagem original
                * preservada na memoria.
                */
                else
                {
                    if (!image_restore_original(
                            &image))
                    {
                        fprintf(
                            stderr,
                            "Erro ao restaurar imagem original.\n"
                        );

                        running = false;
                        continue;
                    }
                }

                /*
                * Independente de termos equalizado
                * ou restaurado, precisamos atualizar
                * a textura da janela principal.
                */
                if (!window_set_image(
                        &main_window,
                        image.surface))
                {
                    fprintf(
                        stderr,
                        "Erro ao atualizar imagem exibida.\n"
                    );

                    running = false;
                    continue;
                }

                /*
                * O histograma tambem precisa representar
                * sempre a imagem atualmente exibida.
                */
                if (!histogram_calculate(
                        &histogram,
                        image.surface))
                {
                    fprintf(
                        stderr,
                        "Erro ao atualizar histograma.\n"
                    );

                    running = false;
                    continue;
                }

                printf(
                    "Imagem e histograma atualizados.\n"
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
            }

            /*
             * O botao de resolucao ainda
             * nao executa a funcionalidade real.
             */
            if (action ==
                INFO_ACTION_RESOLUTION)
            {
                if (!window_toggle_resolution(
                        &main_window,
                        image.surface))
                {
                    fprintf(
                        stderr,
                        "Erro ao alternar resolucao da imagem.\n"
                    );

                    running = false;
                    continue;
                }

                /*
                * Garante novamente a posicao prevista
                * para a janela secundaria.
                */
                if (!SDL_SetWindowPosition(
                        info_window.window,
                        0,
                        0))
                {
                    fprintf(
                        stderr,
                        "Erro ao reposicionar janela secundaria: %s\n",
                        SDL_GetError()
                    );

                    running = false;
                    continue;
                }

                SDL_SyncWindow(
                    info_window.window
                );

                if (main_window.original_resolution)
                {
                    printf(
                        "Exibindo imagem na resolucao original: %d x %d.\n",
                        image.surface->w,
                        image.surface->h
                    );
                }
                else
                {
                    printf(
                        "Exibindo imagem em 1024 x 768.\n"
                    );
                }
            }

            switch (event.type)
            {
                case SDL_EVENT_QUIT:

                    running = false;

                    break;

                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:

                    /*
                     * Fecha a aplicacao se qualquer
                     * uma das janelas for fechada.
                     */
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

                case SDL_EVENT_KEY_DOWN:

                    /*
                    * Ignora repeticoes automaticas caso
                    * o usuario mantenha a tecla pressionada.
                    */
                    if (!event.key.repeat &&
                        event.key.key == SDLK_S)
                    {
                        int save_width;
                        int save_height;

                        /*
                        * Define a resolucao do arquivo
                        * conforme o modo atualmente exibido.
                        */
                        if (main_window.original_resolution)
                        {
                            save_width =
                                image.surface->w;

                            save_height =
                                image.surface->h;
                        }
                        else
                        {
                            save_width =
                                MAIN_WINDOW_WIDTH;

                            save_height =
                                MAIN_WINDOW_HEIGHT;
                        }

                        if (!image_save_png(
                                &image,
                                save_width,
                                save_height,
                                "output_image.png"))
                        {
                            fprintf(
                                stderr,
                                "Falha ao salvar output_image.png.\n"
                            );
                        }
                    }

                    break;
                
            }
        }

        /*
         * Renderizacao da imagem atual.
         */
        window_render(
            &main_window
        );

        /*
         * Renderizacao da janela secundaria:
         * histograma e botoes.
         */
        info_window_render(
            &info_window,
            &histogram,
            image.equalized,
            main_window.original_resolution
        );

        SDL_Delay(16);
    }

    /*
     * Liberacao dos recursos.
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
