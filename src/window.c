#include "window.h"

#include <stdio.h>

#define FONT_PATH "assets/fonts/DejaVuSans.ttf"
#define FONT_SIZE 18.0f


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


static SDL_Texture *create_text_texture(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *text,
    SDL_Color color,
    float *width,
    float *height
)
{
    if (renderer == NULL ||
        font == NULL ||
        text == NULL)
    {
        return NULL;
    }

    SDL_Surface *text_surface =
        TTF_RenderText_Blended(
            font,
            text,
            0,
            color
        );

    if (text_surface == NULL)
    {
        fprintf(
            stderr,
            "Erro ao renderizar texto '%s': %s\n",
            text,
            SDL_GetError()
        );

        return NULL;
    }

    SDL_Texture *text_texture =
        SDL_CreateTextureFromSurface(
            renderer,
            text_surface
        );

    if (text_texture == NULL)
    {
        fprintf(
            stderr,
            "Erro ao criar textura do texto: %s\n",
            SDL_GetError()
        );

        SDL_DestroySurface(
            text_surface
        );

        return NULL;
    }

    if (width != NULL)
    {
        *width =
            (float)text_surface->w;
    }

    if (height != NULL)
    {
        *height =
            (float)text_surface->h;
    }

    SDL_DestroySurface(
        text_surface
    );

    return text_texture;
}


static bool draw_text(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *text,
    float x,
    float y,
    SDL_Color color
)
{
    float width = 0.0f;
    float height = 0.0f;

    SDL_Texture *texture =
        create_text_texture(
            renderer,
            font,
            text,
            color,
            &width,
            &height
        );

    if (texture == NULL)
    {
        return false;
    }

    SDL_FRect destination = {
        .x = x,
        .y = y,
        .w = width,
        .h = height
    };

    SDL_RenderTexture(
        renderer,
        texture,
        NULL,
        &destination
    );

    SDL_DestroyTexture(
        texture
    );

    return true;
}


static bool draw_text_centered(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *text,
    const SDL_FRect *rect,
    SDL_Color color
)
{
    if (rect == NULL)
    {
        return false;
    }

    float width = 0.0f;
    float height = 0.0f;

    SDL_Texture *texture =
        create_text_texture(
            renderer,
            font,
            text,
            color,
            &width,
            &height
        );

    if (texture == NULL)
    {
        return false;
    }

    SDL_FRect destination = {
        .x =
            rect->x +
            (rect->w - width) / 2.0f,

        .y =
            rect->y +
            (rect->h - height) / 2.0f,

        .w = width,
        .h = height
    };

    SDL_RenderTexture(
        renderer,
        texture,
        NULL,
        &destination
    );

    SDL_DestroyTexture(
        texture
    );

    return true;
}


static void draw_button(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const Button *button,
    const char *text
)
{
    if (renderer == NULL ||
        button == NULL)
    {
        return;
    }

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

    /*
     * Texto branco centralizado
     * dentro do botao.
     */
    SDL_Color text_color = {
        255,
        255,
        255,
        255
    };

    draw_text_centered(
        renderer,
        font,
        text,
        &button->rect,
        text_color
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
     * Diminuimos um pouco a altura para
     * liberar espaco para as informacoes.
     */
    SDL_FRect graph_area = {
        .x = 30.0f,
        .y = 45.0f,
        .w = 360.0f,
        .h = 245.0f
    };

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

            .w =
                bar_width,

            .h =
                bar_height
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
    app_window->original_resolution = false;

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

    bool use_original_resolution =
        !app_window->original_resolution;

    int new_width;
    int new_height;

    if (use_original_resolution)
    {
        new_width =
            surface->w;

        new_height =
            surface->h;
    }
    else
    {
        new_width =
            MAIN_WINDOW_WIDTH;

        new_height =
            MAIN_WINDOW_HEIGHT;
    }

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

    app_window->original_resolution =
        use_original_resolution;

    app_window->image_rect.x = 0.0f;
    app_window->image_rect.y = 0.0f;

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
    app_window->original_resolution = false;
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
    info_window->font = NULL;
    info_window->ttf_initialized = false;

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
     * Inicializa SDL_ttf.
     */
    if (!TTF_Init())
    {
        fprintf(
            stderr,
            "Erro ao inicializar SDL_ttf: %s\n",
            SDL_GetError()
        );

        info_window_destroy(
            info_window
        );

        return false;
    }

    info_window->ttf_initialized =
        true;

    /*
     * Carrega a fonte que acompanha
     * o projeto.
     */
    info_window->font =
        TTF_OpenFont(
            FONT_PATH,
            FONT_SIZE
        );

    if (info_window->font == NULL)
    {
        fprintf(
            stderr,
            "Erro ao carregar fonte '%s': %s\n",
            FONT_PATH,
            SDL_GetError()
        );

        info_window_destroy(
            info_window
        );

        return false;
    }

    /*
     * Botao Equalizar.
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
     * Botao Resolucao.
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

    if (event->type ==
            SDL_EVENT_MOUSE_MOTION &&
        event->motion.windowID ==
            info_window->id)
    {
        float mouse_x =
            event->motion.x;

        float mouse_y =
            event->motion.y;

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
    const Histogram *histogram,
    bool image_equalized,
    bool original_resolution
)
{
    if (info_window == NULL ||
        info_window->renderer == NULL ||
        info_window->font == NULL ||
        histogram == NULL)
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

    /*
     * Histograma.
     */
    draw_histogram(
        info_window->renderer,
        histogram
    );

    SDL_Color text_color = {
        30,
        30,
        30,
        255
    };

    /*
     * Titulo.
     */
    draw_text(
        info_window->renderer,
        info_window->font,
        "Histograma",
        30.0f,
        15.0f,
        text_color
    );

    /*
     * Informacao da media.
     */
    char mean_text[128];

    snprintf(
        mean_text,
        sizeof(mean_text),
        "Media: %.2f - %s",
        histogram->mean,
        histogram_brightness_classification(
            histogram
        )
    );

    draw_text(
        info_window->renderer,
        info_window->font,
        mean_text,
        30.0f,
        310.0f,
        text_color
    );

    /*
     * Informacao do desvio padrao
     * e classificacao de contraste.
     */
    char deviation_text[128];

    snprintf(
        deviation_text,
        sizeof(deviation_text),
        "Desvio padrao: %.2f - %s",
        histogram->standard_deviation,
        histogram_contrast_classification(
            histogram
        )
    );

    draw_text(
        info_window->renderer,
        info_window->font,
        deviation_text,
        30.0f,
        340.0f,
        text_color
    );

    /*
     * O texto do primeiro botao
     * depende do estado atual da imagem.
     */
    const char *equalize_text =
        image_equalized
        ? "Ver original"
        : "Equalizar";

    /*
     * O texto mostra a acao que ocorrera
     * no proximo clique.
     */
    const char *resolution_text =
        original_resolution
        ? "1024x768"
        : "Resolucao original";

    draw_button(
        info_window->renderer,
        info_window->font,
        &info_window->equalize_button,
        equalize_text
    );

    draw_button(
        info_window->renderer,
        info_window->font,
        &info_window->resolution_button,
        resolution_text
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

    /*
     * Libera a fonte antes de finalizar
     * o SDL_ttf.
     */
    if (info_window->font != NULL)
    {
        TTF_CloseFont(
            info_window->font
        );

        info_window->font = NULL;
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

    if (info_window->ttf_initialized)
    {
        TTF_Quit();

        info_window->ttf_initialized =
            false;
    }

    info_window->id =
        0;
}
