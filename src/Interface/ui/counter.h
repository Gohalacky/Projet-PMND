#ifndef UI_COUNTER_H
#define UI_COUNTER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../Struct.h"

void initCounter(Counter* c, int min, int max, int startValue,int xPlus, int yPlus, 
                 int xMoins, int yMoins,int w, int h, int xText, int yText); 

void handleCounter(Counter* c, int mouseX, int mouseY);
void renderCounter(SDL_Renderer* renderer, Counter* c, TTF_Font* font, int xText, int yText) ;

#endif