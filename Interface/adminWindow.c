#include "adminWindow.h"

#include <SDL2/SDL.h>
#include <stdio.h>

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