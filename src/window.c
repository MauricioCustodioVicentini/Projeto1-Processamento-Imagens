#include "window.h"

#include <stdio.h>

static bool point_inside_rect(
    float x,
    float y,
    const SDL_FRect *rect
)
{
    if (rect == NULL)
    {
        return false;
    }

    return
        x >= rect->x &&
        x <= rect->x + rect->w &&
        y >= rect->y &&
        y <= rect->y + rect->h;
}

static void draw_button(
    SDL_Renderer *renderer,
    const Button *button
)
{
    if (renderer == NULL ||
        button == NULL)
    {
        return;
    }

    /*
     * Cores definidas conforme o estado
     * de interacao do botao.
     */
    switch (button->state)
    {
        case BUTTON_HOVER:

            SDL_SetRenderDrawColor(
                renderer,
                100,
                170,
                240,
                255
            );

            break;

        case BUTTON_PRESSED:

            SDL_SetRenderDrawColor(
                renderer,
                20,
                80,
                150,
                255
            );

            break;

        case BUTTON_NORMAL:
        default:

            SDL_SetRenderDrawColor(
                renderer,
                40,
                120,
                210,
                255
            );

            break;
    }

    SDL_RenderFillRect(
        renderer,
        &button->rect
    );

    /*
     * Contorno do botao.
     */
    SDL_SetRenderDrawColor(
        renderer,
        20,
        60,
        100,
        255
    );

    SDL_RenderRect(
        renderer,
        &button->rect
    );
}

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

    SDL_FRect graph_area = {
        .x = 30.0f,
        .y = 40.0f,
        .w = 360.0f,
        .h = 300.0f
    };

    /*
     * Borda do histograma.
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
        float normalized_frequency =
            (float)histogram->bins[i] /
            (float)histogram->max_count;

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

bool window_initialize(
    AppWindow *app_window
)
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

    app_window->image_rect.w =
        MAIN_WINDOW_WIDTH;

    app_window->image_rect.h =
        MAIN_WINDOW_HEIGHT;

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

void window_render(
    AppWindow *app_window
)
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

void window_destroy(
    AppWindow *app_window
)
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

    /*
     * Botao de equalizacao.
     */
    info_window->equalize_button.rect.x = 60.0f;
    info_window->equalize_button.rect.y = 390.0f;
    info_window->equalize_button.rect.w = 300.0f;
    info_window->equalize_button.rect.h = 60.0f;

    info_window->equalize_button.state =
        BUTTON_NORMAL;

    /*
     * Botao de resolucao.
     */
    info_window->resolution_button.rect.x = 60.0f;
    info_window->resolution_button.rect.y = 480.0f;
    info_window->resolution_button.rect.w = 300.0f;
    info_window->resolution_button.rect.h = 60.0f;

    info_window->resolution_button.state =
        BUTTON_NORMAL;

    return true;
}

InfoAction info_window_handle_event(
    InfoWindow *info_window,
    const SDL_Event *event
)
{
    if (info_window == NULL ||
        event == NULL)
    {
        return INFO_ACTION_NONE;
    }

    /*
     * Movimento do mouse.
     */
    if (event->type == SDL_EVENT_MOUSE_MOTION &&
        event->motion.windowID == info_window->id)
    {
        float mouse_x = event->motion.x;
        float mouse_y = event->motion.y;

        if (point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->equalize_button.rect))
        {
            if (info_window->equalize_button.state !=
                BUTTON_PRESSED)
            {
                info_window->equalize_button.state =
                    BUTTON_HOVER;
            }
        }
        else
        {
            info_window->equalize_button.state =
                BUTTON_NORMAL;
        }

        if (point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->resolution_button.rect))
        {
            if (info_window->resolution_button.state !=
                BUTTON_PRESSED)
            {
                info_window->resolution_button.state =
                    BUTTON_HOVER;
            }
        }
        else
        {
            info_window->resolution_button.state =
                BUTTON_NORMAL;
        }
    }

    /*
     * Botao esquerdo pressionado.
     */
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
        event->button.windowID == info_window->id &&
        event->button.button == SDL_BUTTON_LEFT)
    {
        float mouse_x = event->button.x;
        float mouse_y = event->button.y;

        if (point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->equalize_button.rect))
        {
            info_window->equalize_button.state =
                BUTTON_PRESSED;
        }

        if (point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->resolution_button.rect))
        {
            info_window->resolution_button.state =
                BUTTON_PRESSED;
        }
    }

    /*
     * Botao esquerdo liberado.
     */
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP &&
        event->button.windowID == info_window->id &&
        event->button.button == SDL_BUTTON_LEFT)
    {
        float mouse_x = event->button.x;
        float mouse_y = event->button.y;

        bool equalize_clicked =
            info_window->equalize_button.state ==
                BUTTON_PRESSED &&
            point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->equalize_button.rect
            );

        bool resolution_clicked =
            info_window->resolution_button.state ==
                BUTTON_PRESSED &&
            point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->resolution_button.rect
            );

        info_window->equalize_button.state =
            point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->equalize_button.rect
            )
            ? BUTTON_HOVER
            : BUTTON_NORMAL;

        info_window->resolution_button.state =
            point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->resolution_button.rect
            )
            ? BUTTON_HOVER
            : BUTTON_NORMAL;

        if (equalize_clicked)
        {
            return INFO_ACTION_EQUALIZE;
        }

        if (resolution_clicked)
        {
            return INFO_ACTION_RESOLUTION;
        }
    }

    return INFO_ACTION_NONE;
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

    draw_histogram(
        info_window->renderer,
        histogram
    );

    draw_button(
        info_window->renderer,
        &info_window->equalize_button
    );

    draw_button(
        info_window->renderer,
        &info_window->resolution_button
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
