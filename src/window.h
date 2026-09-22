#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "histogram.h"

enum
{
    MAIN_WINDOW_WIDTH = 1024,
    MAIN_WINDOW_HEIGHT = 768,

    INFO_WINDOW_WIDTH = 420,
    INFO_WINDOW_HEIGHT = 650
};

typedef enum
{
    BUTTON_NORMAL,
    BUTTON_HOVER,
    BUTTON_PRESSED
} ButtonState;

typedef struct
{
    SDL_FRect rect;
    ButtonState state;
} Button;

typedef enum
{
    INFO_ACTION_NONE,
    INFO_ACTION_EQUALIZE,
    INFO_ACTION_RESOLUTION
} InfoAction;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *image_texture;

    SDL_FRect image_rect;

    SDL_WindowID id;

    bool original_resolution;

} AppWindow;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_WindowID id;

    Button equalize_button;
    Button resolution_button;

    TTF_Font *font;

    bool ttf_initialized;

} InfoWindow;

bool window_initialize(
    AppWindow *app_window
);

bool window_set_image(
    AppWindow *app_window,
    SDL_Surface *surface
);

bool window_toggle_resolution(
    AppWindow *app_window,
    SDL_Surface *surface
);

void window_render(
    AppWindow *app_window
);

void window_destroy(
    AppWindow *app_window
);

bool info_window_initialize(
    InfoWindow *info_window,
    SDL_Window *parent_window
);

InfoAction info_window_handle_event(
    InfoWindow *info_window,
    const SDL_Event *event
);

void info_window_render(
    InfoWindow *info_window,
    const Histogram *histogram,
    bool image_equalized,
    bool original_resolution
);

void info_window_destroy(
    InfoWindow *info_window
);

#endif
