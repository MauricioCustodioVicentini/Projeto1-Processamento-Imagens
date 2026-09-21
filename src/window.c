#include "window.h"

#include <stdio.h>

static void draw_histogram(
    SDL_Renderer *renderer,
    const Histogram *histogram
)
{
    if (renderer == NULL ||
        histogram == NULL ||
        histogram->max_count == 0)
    {
        return;
    }

    /*
     * Area reservada para o histograma
     * dentro da janela secundaria.
     */
    SDL_FRect graph_area = {
        .x = 30.0f,
        .y = 40.0f,
        .w = 360.0f,
        .h = 300.0f
    };

    /*
     * Borda do grafico.
     */
    SDL_SetRenderDrawColor(
        renderer,
        60,
        60,
        60,
        255
    );

    SDL_RenderRect(
        renderer,
        &graph_area
    );

    /*
     * Cada uma das 256 intensidades ocupa
     * uma parte da largura disponivel.
     */
    float bar_width =
        graph_area.w /
        (float)HISTOGRAM_LEVELS;

    SDL_SetRenderDrawColor(
        renderer,
        30,
        30,
        30,
        255
    );

    for (int i = 0;
         i < HISTOGRAM_LEVELS;
         i++)
    {
        /*
         * Frequencia normalizada entre 0 e 1.
         */
        float normalized_frequency =
            (float)histogram->bins[i] /
            (float)histogram->max_count;

        /*
         * Altura proporcional ao maior valor
         * existente no histograma.
         */
        float bar_height =
            normalized_frequency *
            graph_area.h;

        SDL_FRect bar = {
            .x =
                graph_area.x +
                i * bar_width,

            .y =
                graph_area.y +
                graph_area.h -
                bar_height,

            .w = bar_width,

            .h = bar_height
        };

        SDL_RenderFillRect(
            renderer,
            &bar
        );
    }
}

bool window_initialize(AppWindow *app_window)
{
    if (app_window == NULL)
    {
        fprintf(
            stderr,
            "Erro: estrutura da janela invalida.\n"
        );

        return false;
    }

    app_window->window = NULL;
    app_window->renderer = NULL;
    app_window->image_texture = NULL;
    app_window->id = 0;

    app_window->image_rect.x = 0.0f;
    app_window->image_rect.y = 0.0f;
    app_window->image_rect.w = MAIN_WINDOW_WIDTH;
    app_window->image_rect.h = MAIN_WINDOW_HEIGHT;

    if (!SDL_CreateWindowAndRenderer(
            "Projeto 1 - Processamento de Imagens",
            MAIN_WINDOW_WIDTH,
            MAIN_WINDOW_HEIGHT,
            0,
            &app_window->window,
            &app_window->renderer))
    {
        fprintf(
            stderr,
            "Erro ao criar janela ou renderer: %s\n",
            SDL_GetError()
        );

        return false;
    }

    SDL_SetWindowPosition(
        app_window->window,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED
    );

    SDL_SyncWindow(
        app_window->window
    );

    app_window->id =
        SDL_GetWindowID(
            app_window->window
        );

    if (app_window->id == 0)
    {
        fprintf(
            stderr,
            "Erro ao obter ID da janela principal: %s\n",
            SDL_GetError()
        );

        window_destroy(
            app_window
        );

        return false;
    }

    return true;
}

bool window_set_image(
    AppWindow *app_window,
    SDL_Surface *surface
)
{
    if (app_window == NULL ||
        app_window->renderer == NULL ||
        surface == NULL)
    {
        fprintf(
            stderr,
            "Erro: dados invalidos para exibir a imagem.\n"
        );

        return false;
    }

    if (app_window->image_texture != NULL)
    {
        SDL_DestroyTexture(
            app_window->image_texture
        );

        app_window->image_texture = NULL;
    }

    app_window->image_texture =
        SDL_CreateTextureFromSurface(
            app_window->renderer,
            surface
        );

    if (app_window->image_texture == NULL)
    {
        fprintf(
            stderr,
            "Erro ao criar textura da imagem: %s\n",
            SDL_GetError()
        );

        return false;
    }

    app_window->image_rect.x = 0.0f;
    app_window->image_rect.y = 0.0f;

    app_window->image_rect.w =
        MAIN_WINDOW_WIDTH;

    app_window->image_rect.h =
        MAIN_WINDOW_HEIGHT;

    return true;
}

void window_render(AppWindow *app_window)
{
    if (app_window == NULL ||
        app_window->renderer == NULL)
    {
        return;
    }

    SDL_SetRenderDrawColor(
        app_window->renderer,
        0,
        0,
        0,
        255
    );

    SDL_RenderClear(
        app_window->renderer
    );

    if (app_window->image_texture != NULL)
    {
        SDL_RenderTexture(
            app_window->renderer,
            app_window->image_texture,
            NULL,
            &app_window->image_rect
        );
    }

    SDL_RenderPresent(
        app_window->renderer
    );
}

void window_destroy(AppWindow *app_window)
{
    if (app_window == NULL)
    {
        return;
    }

    if (app_window->image_texture != NULL)
    {
        SDL_DestroyTexture(
            app_window->image_texture
        );

        app_window->image_texture = NULL;
    }

    if (app_window->renderer != NULL)
    {
        SDL_DestroyRenderer(
            app_window->renderer
        );

        app_window->renderer = NULL;
    }

    if (app_window->window != NULL)
    {
        SDL_DestroyWindow(
            app_window->window
        );

        app_window->window = NULL;
    }

    app_window->id = 0;
}

bool info_window_initialize(
    InfoWindow *info_window,
    SDL_Window *parent_window
)
{
    if (info_window == NULL ||
        parent_window == NULL)
    {
        fprintf(
            stderr,
            "Erro: dados invalidos para criar janela secundaria.\n"
        );

        return false;
    }

    info_window->window = NULL;
    info_window->renderer = NULL;
    info_window->id = 0;

    if (!SDL_CreateWindowAndRenderer(
            "Informacoes da Imagem",
            INFO_WINDOW_WIDTH,
            INFO_WINDOW_HEIGHT,
            0,
            &info_window->window,
            &info_window->renderer))
    {
        fprintf(
            stderr,
            "Erro ao criar janela secundaria: %s\n",
            SDL_GetError()
        );

        return false;
    }

    if (!SDL_SetWindowParent(
            info_window->window,
            parent_window))
    {
        fprintf(
            stderr,
            "Erro ao definir janela secundaria como filha: %s\n",
            SDL_GetError()
        );

        info_window_destroy(
            info_window
        );

        return false;
    }

    if (!SDL_SetWindowPosition(
            info_window->window,
            0,
            0))
    {
        fprintf(
            stderr,
            "Erro ao posicionar janela secundaria: %s\n",
            SDL_GetError()
        );

        info_window_destroy(
            info_window
        );

        return false;
    }

    SDL_SyncWindow(
        info_window->window
    );

    info_window->id =
        SDL_GetWindowID(
            info_window->window
        );

    if (info_window->id == 0)
    {
        fprintf(
            stderr,
            "Erro ao obter ID da janela secundaria: %s\n",
            SDL_GetError()
        );

        info_window_destroy(
            info_window
        );

        return false;
    }

    return true;
}

void info_window_render(
    InfoWindow *info_window,
    const Histogram *histogram
)
{
    if (info_window == NULL ||
        info_window->renderer == NULL)
    {
        return;
    }

    /*
     * Fundo claro da janela secundaria.
     */
    SDL_SetRenderDrawColor(
        info_window->renderer,
        245,
        245,
        245,
        255
    );

    SDL_RenderClear(
        info_window->renderer
    );

    /*
     * Desenha o histograma atual.
     */
    draw_histogram(
        info_window->renderer,
        histogram
    );

    SDL_RenderPresent(
        info_window->renderer
    );
}

void info_window_destroy(
    InfoWindow *info_window
)
{
    if (info_window == NULL)
    {
        return;
    }

    if (info_window->renderer != NULL)
    {
        SDL_DestroyRenderer(
            info_window->renderer
        );

        info_window->renderer = NULL;
    }

    if (info_window->window != NULL)
    {
        SDL_DestroyWindow(
            info_window->window
        );

        info_window->window = NULL;
    }

    info_window->id = 0;
}
