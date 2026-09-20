
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
        fprintf(stderr, "Erro: dados invalidos para exibir a imagem.\n");
        return false;
    }

    if (app_window->image_texture != NULL)
    {
        SDL_DestroyTexture(app_window->image_texture);
        app_window->image_texture = NULL;
    }

    app_window->image_texture = SDL_CreateTextureFromSurface(
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
     * Neste momento a imagem deve ocupar 1024x768,
     * conforme a resolucao inicial solicitada no projeto.
     */
    app_window->image_rect.x = 0.0f;
    app_window->image_rect.y = 0.0f;
    app_window->image_rect.w = MAIN_WINDOW_WIDTH;
    app_window->image_rect.h = MAIN_WINDOW_HEIGHT;

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

    SDL_RenderClear(app_window->renderer);

    if (app_window->image_texture != NULL)
    {
        SDL_RenderTexture(
            app_window->renderer,
            app_window->image_texture,
            NULL,
            &app_window->image_rect
        );
    }

    SDL_RenderPresent(app_window->renderer);
}

void window_destroy(AppWindow *app_window)
{
    if (app_window == NULL)
    {
        return;
    }

    if (app_window->image_texture != NULL)
    {
        SDL_DestroyTexture(app_window->image_texture);
        app_window->image_texture = NULL;
    }

    if (app_window->renderer != NULL)
    {
        SDL_DestroyRenderer(app_window->renderer);
        app_window->renderer = NULL;
    }

    if (app_window->window != NULL)
    {
        SDL_DestroyWindow(app_window->window);
        app_window->window = NULL;
    }
}
