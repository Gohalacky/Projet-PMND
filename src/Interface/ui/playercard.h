#ifndef UI_PLAYERCARD_H
#define UI_PLAYERCARD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../Struct.h"

typedef struct{
    int x,y,w,h;
}CardRef;

extern const CardRef CARDS_2J[2];
extern const CardRef CARDS_3J[3];
extern const CardRef CARDS_4J[4];

void renderPlayerCard(SDL_Renderer* renderer, TTF_Font* font, const Joueur* j, int numero, SDL_Rect rect);



#endif