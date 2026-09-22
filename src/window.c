#include "window.h"

#include <stdio.h>


static bool position_main_window(
    AppWindow *app_window,
    int window_width,
    int window_height
)
{
    if (app_window == NULL ||
        app_window->window == NULL)
    {
        return false;
    }

    /*
     * Obtem o monitor principal.
     */
    SDL_DisplayID primary_display =
        SDL_GetPrimaryDisplay();

    if (primary_display == 0)
    {
        fprintf(
            stderr,
            "Erro ao obter monitor principal: %s\n",
            SDL_GetError()
        );

        return false;
    }

    /*
     * Obtem os limites do monitor principal.
     */
    SDL_Rect display_bounds;

    if (!SDL_GetDisplayBounds(
            primary_display,
            &display_bounds))
    {
        fprintf(
            stderr,
            "Erro ao obter resolucao do monitor: %s\n",
            SDL_GetError()
        );

        return false;
    }

    /*
     * Se a janela ultrapassar o tamanho
     * do monitor, posiciona em (0,0).
     */
    if (window_width > display_bounds.w ||
        window_height > display_bounds.h)
    {
        if (!SDL_SetWindowPosition(
                app_window->window,
                0,
                0))
        {
            fprintf(
                stderr,
                "Erro ao posicionar janela em (0,0): %s\n",
                SDL_GetError()
            );

            return false;
        }

        return true;
    }

    /*
     * Caso a janela caiba no monitor,
     * calcula sua posicao central.
     */
    int position_x =
        display_bounds.x +
        (display_bounds.w - window_width) / 2;

    int position_y =
        display_bounds.y +
        (display_bounds.h - window_height) / 2;

    if (!SDL_SetWindowPosition(
            app_window->window,
            position_x,
            position_y))
    {
        fprintf(
            stderr,
            "Erro ao centralizar janela: %s\n",
            SDL_GetError()
        );

        return false;
    }

    return true;
}


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
     * Define a cor do botao conforme
     * o estado de interacao.
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

    /*
     * Preenche o botao.
     */
    SDL_RenderFillRect(
        renderer,
        &button->rect
    );

    /*
     * Desenha o contorno.
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

    /*
     * Area utilizada pelo histograma
     * na janela secundaria.
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
     * Cada intensidade ocupa uma parte
     * da largura total do histograma.
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

    /*
     * O programa inicia exibindo
     * a imagem em 1024x768.
     */
    app_window->original_resolution =
        false;

    app_window->image_rect.x =
        0.0f;

    app_window->image_rect.y =
        0.0f;

    app_window->image_rect.w =
        MAIN_WINDOW_WIDTH;

    app_window->image_rect.h =
        MAIN_WINDOW_HEIGHT;

    /*
     * Cria janela principal e renderer.
     */
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

    /*
     * Posiciona a janela inicialmente.
     */
    if (!position_main_window(
            app_window,
            MAIN_WINDOW_WIDTH,
            MAIN_WINDOW_HEIGHT))
    {
        window_destroy(
            app_window
        );

        return false;
    }

    SDL_SyncWindow(
        app_window->window
    );

    /*
     * Guarda o ID da janela para
     * identificar seus eventos.
     */
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

    /*
     * Destrui a textura anterior,
     * caso exista.
     */
    if (app_window->image_texture != NULL)
    {
        SDL_DestroyTexture(
            app_window->image_texture
        );

        app_window->image_texture =
            NULL;
    }

    /*
     * Cria uma nova textura a partir
     * da superficie atual.
     */
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

    app_window->image_rect.x =
        0.0f;

    app_window->image_rect.y =
        0.0f;

    /*
     * Mantem o modo de resolucao
     * atualmente selecionado.
     *
     * Isso e importante quando a imagem
     * e equalizada ou restaurada.
     */
    if (app_window->original_resolution)
    {
        app_window->image_rect.w =
            (float)surface->w;

        app_window->image_rect.h =
            (float)surface->h;
    }
    else
    {
        app_window->image_rect.w =
            MAIN_WINDOW_WIDTH;

        app_window->image_rect.h =
            MAIN_WINDOW_HEIGHT;
    }

    return true;
}


bool window_toggle_resolution(
    AppWindow *app_window,
    SDL_Surface *surface
)
{
    if (app_window == NULL ||
        app_window->window == NULL ||
        surface == NULL)
    {
        fprintf(
            stderr,
            "Erro: dados invalidos para alterar resolucao.\n"
        );

        return false;
    }

    /*
     * Determina qual sera o novo modo.
     */
    bool use_original_resolution =
        !app_window->original_resolution;

    int new_width;
    int new_height;

    /*
     * Se o novo modo for a resolucao
     * original, usa as dimensoes reais
     * da imagem.
     */
    if (use_original_resolution)
    {
        new_width =
            surface->w;

        new_height =
            surface->h;
    }
    else
    {
        /*
         * Caso contrario, volta para
         * 1024x768.
         */
        new_width =
            MAIN_WINDOW_WIDTH;

        new_height =
            MAIN_WINDOW_HEIGHT;
    }

    /*
     * Altera o tamanho da janela.
     */
    if (!SDL_SetWindowSize(
            app_window->window,
            new_width,
            new_height))
    {
        fprintf(
            stderr,
            "Erro ao alterar tamanho da janela: %s\n",
            SDL_GetError()
        );

        return false;
    }

    SDL_SyncWindow(
        app_window->window
    );

    /*
     * Posiciona a janela conforme
     * o tamanho do monitor.
     */
    if (!position_main_window(
            app_window,
            new_width,
            new_height))
    {
        return false;
    }

    SDL_SyncWindow(
        app_window->window
    );

    /*
     * Atualiza o estado somente depois
     * das operacoes anteriores funcionarem.
     */
    app_window->original_resolution =
        use_original_resolution;

    /*
     * Atualiza a area utilizada para
     * desenhar a textura.
     */
    app_window->image_rect.x =
        0.0f;

    app_window->image_rect.y =
        0.0f;

    app_window->image_rect.w =
        (float)new_width;

    app_window->image_rect.h =
        (float)new_height;

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

    /*
     * Fundo preto da janela principal.
     */
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

    /*
     * Desenha a imagem atual.
     */
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

        app_window->image_texture =
            NULL;
    }

    if (app_window->renderer != NULL)
    {
        SDL_DestroyRenderer(
            app_window->renderer
        );

        app_window->renderer =
            NULL;
    }

    if (app_window->window != NULL)
    {
        SDL_DestroyWindow(
            app_window->window
        );

        app_window->window =
            NULL;
    }

    app_window->id = 0;

    app_window->original_resolution =
        false;
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

    info_window->window =
        NULL;

    info_window->renderer =
        NULL;

    info_window->id =
        0;

    /*
     * Cria a janela secundaria.
     */
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

    /*
     * Define a janela secundaria
     * como filha da janela principal.
     */
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

    /*
     * Posiciona a janela secundaria.
     */
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

    /*
     * Guarda o ID da janela secundaria.
     */
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
     * Configuracao do botao de equalizacao.
     */
    info_window->equalize_button.rect.x =
        60.0f;

    info_window->equalize_button.rect.y =
        390.0f;

    info_window->equalize_button.rect.w =
        300.0f;

    info_window->equalize_button.rect.h =
        60.0f;

    info_window->equalize_button.state =
        BUTTON_NORMAL;

    /*
     * Configuracao do botao de resolucao.
     */
    info_window->resolution_button.rect.x =
        60.0f;

    info_window->resolution_button.rect.y =
        480.0f;

    info_window->resolution_button.rect.w =
        300.0f;

    info_window->resolution_button.rect.h =
        60.0f;

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
    if (event->type ==
            SDL_EVENT_MOUSE_MOTION &&
        event->motion.windowID ==
            info_window->id)
    {
        float mouse_x =
            event->motion.x;

        float mouse_y =
            event->motion.y;

        /*
         * Estado do botao de equalizacao.
         */
        if (point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->
                    equalize_button.rect))
        {
            if (info_window->
                    equalize_button.state !=
                BUTTON_PRESSED)
            {
                info_window->
                    equalize_button.state =
                    BUTTON_HOVER;
            }
        }
        else
        {
            info_window->
                equalize_button.state =
                BUTTON_NORMAL;
        }

        /*
         * Estado do botao de resolucao.
         */
        if (point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->
                    resolution_button.rect))
        {
            if (info_window->
                    resolution_button.state !=
                BUTTON_PRESSED)
            {
                info_window->
                    resolution_button.state =
                    BUTTON_HOVER;
            }
        }
        else
        {
            info_window->
                resolution_button.state =
                BUTTON_NORMAL;
        }
    }

    /*
     * Mouse pressionado.
     */
    if (event->type ==
            SDL_EVENT_MOUSE_BUTTON_DOWN &&
        event->button.windowID ==
            info_window->id &&
        event->button.button ==
            SDL_BUTTON_LEFT)
    {
        float mouse_x =
            event->button.x;

        float mouse_y =
            event->button.y;

        if (point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->
                    equalize_button.rect))
        {
            info_window->
                equalize_button.state =
                BUTTON_PRESSED;
        }

        if (point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->
                    resolution_button.rect))
        {
            info_window->
                resolution_button.state =
                BUTTON_PRESSED;
        }
    }

    /*
     * Mouse liberado.
     */
    if (event->type ==
            SDL_EVENT_MOUSE_BUTTON_UP &&
        event->button.windowID ==
            info_window->id &&
        event->button.button ==
            SDL_BUTTON_LEFT)
    {
        float mouse_x =
            event->button.x;

        float mouse_y =
            event->button.y;

        bool equalize_clicked =
            info_window->
                equalize_button.state ==
                    BUTTON_PRESSED &&
            point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->
                    equalize_button.rect
            );

        bool resolution_clicked =
            info_window->
                resolution_button.state ==
                    BUTTON_PRESSED &&
            point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->
                    resolution_button.rect
            );

        /*
         * Depois de soltar o mouse,
         * atualiza os estados dos botoes.
         */
        info_window->
            equalize_button.state =
            point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->
                    equalize_button.rect
            )
            ? BUTTON_HOVER
            : BUTTON_NORMAL;

        info_window->
            resolution_button.state =
            point_inside_rect(
                mouse_x,
                mouse_y,
                &info_window->
                    resolution_button.rect
            )
            ? BUTTON_HOVER
            : BUTTON_NORMAL;

        if (equalize_clicked)
        {
            return
                INFO_ACTION_EQUALIZE;
        }

        if (resolution_clicked)
        {
            return
                INFO_ACTION_RESOLUTION;
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

    /*
     * Fundo da janela secundaria.
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
     * Histograma.
     */
    draw_histogram(
        info_window->renderer,
        histogram
    );

    /*
     * Botao de equalizacao.
     */
    draw_button(
        info_window->renderer,
        &info_window->
            equalize_button
    );

    /*
     * Botao de resolucao.
     */
    draw_button(
        info_window->renderer,
        &info_window->
            resolution_button
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

        info_window->renderer =
            NULL;
    }

    if (info_window->window != NULL)
    {
        SDL_DestroyWindow(
            info_window->window
        );

        info_window->window =
            NULL;
    }

    info_window->id =
        0;
}
