#include "adminWindow.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

#include <stdio.h>
#include <stdbool.h>

void handleEventsAdmin(SDL_Event* event, bool* quit){
    while (SDL_PollEvent(event)) {

            if (event->type == SDL_QUIT) {
                *quit = true;
                return;
            }

            if (event->type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                printf("clic de souris : x=%d, y=%d\n", x, y);
            }
        }
}