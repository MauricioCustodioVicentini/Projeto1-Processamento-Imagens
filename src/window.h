#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <SDL3/SDL.h>

enum
{
    MAIN_WINDOW_WIDTH = 1024,
    MAIN_WINDOW_HEIGHT = 768,

    INFO_WINDOW_WIDTH = 420,
    INFO_WINDOW_HEIGHT = 650
};

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *image_texture;
    SDL_FRect image_rect;
    SDL_WindowID id;
} AppWindow;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_WindowID id;
} InfoWindow;

bool window_initialize(AppWindow *app_window);

bool window_set_image(
    AppWindow *app_window,
    SDL_Surface *surface
);

void window_render(AppWindow *app_window);

void window_destroy(AppWindow *app_window);

bool info_window_initialize(
    InfoWindow *info_window,
    SDL_Window *parent_window
);

void info_window_render(InfoWindow *info_window);

void info_window_destroy(InfoWindow *info_window);

#endif
