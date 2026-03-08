#include "view.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <string.h>

void UpdateButtonPositionsScaled(int windowW, int windowH, SDL_Rect* bNew, SDL_Rect* bLoad, SDL_Rect* bSave, SDL_Rect* bRank) {
    
    float ratioX = (float)windowW / REF_W;
    float ratioY = (float)windowH / REF_H;

    int newW = (int)(REF_BTN_W * ratioX);
    int newH = (int)(REF_BTN_H * ratioY);

    int posX = (int)(457.0f * ratioX);
    
    /*SCALE*/
    bNew->w = bLoad->w = bSave->w = bRank->w = newW;
    bNew->h = bLoad->h = bSave->h = bRank->h = newH;
    
    /*NEW GAME*/
    bNew->x = posX;
    bNew->y = (int)(255.0f * ratioY);
    
    /*LOAD GAME*/
    bLoad->x = posX;
    bLoad->y = (int)(337.0f * ratioY);

    /*SAVE*/
    bSave->x = posX;
    bSave->y = (int)(417.0f * ratioY);

    /*RANKING*/
    bRank->x = posX;
    bRank->y = (int)(498.0f * ratioY);
}


void printText(SDL_Renderer* rend, char* f, int size, int valeur, SDL_Color col, int x, int y, char* txt){
    char str[20];

    TTF_Init();
    TTF_Font* font = TTF_OpenFont(f, size);

    if(font==NULL){
        printf("ERREUR font not found : %s\n",TTF_GetError());
        return;
    }

    sprintf(str, "%s :\n   %d",txt,valeur);
    SDL_Surface* surface= TTF_RenderText_Blended(font, str, col);
    SDL_Texture* texture= SDL_CreateTextureFromSurface(rend, surface);
    SDL_Rect r= {x,y,surface->w,surface->h};

    SDL_RenderCopy(rend, texture, NULL, &r);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}