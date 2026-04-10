#ifndef BUILDINGS_H
#define BUILDINGS_H

#include <SDL2/SDL.h>
#include "assets.h"
#include "bridge.h"
#include "view.h"

void renderBuildings(SDL_Renderer* renderer, const ContexteJeu* ctx, const TexturesPack* tex, int winW, int winH);

#endif