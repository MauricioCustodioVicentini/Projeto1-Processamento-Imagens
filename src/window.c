#include "window.h"

#include <stdio.h>

bool window_initialize(AppWindow *app_window)
{
    if (app_window == NULL)
    {
        fprintf(stderr, "Erro: estrutura da janela invalida.\n");
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

    SDL_SyncWindow(app_window->window);

    app_window->id =
        SDL_GetWindowID(app_window->window);

    if (app_window->id == 0)
    {
        fprintf(
            stderr,
            "Erro ao obter ID da janela principal: %s\n",
            SDL_GetError()
        );

        window_destroy(app_window);
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

    /*
     * Visualizacao inicial em 1024x768,
     * conforme solicitado no projeto.
     */
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
    InfoWindow *info_window
)
{
    if (info_window == NULL ||
        info_window->renderer == NULL)
    {
        return;
    }

    /*
     * Fundo temporario.
     * Histograma, textos e botoes
     * serao adicionados nos proximos commits.
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
