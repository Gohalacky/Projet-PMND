#include "text.h"

#include <stdio.h>

#define FONT_SIZE_NORMAL 32
#define FONT_SIZE_TITLE 38  
#define FONT_SIZE_BUTTON 22

void ScaleText(Text* t, float ratioX, float ratioY){
    t->x    = (int)(t->refX    * ratioX);
    t->y    = (int)(t->refY    * ratioY);
    t->size = (int)(t->refSize * ratioY);
}

void ScaleAllTexts(Text **texts, int count, float ratioX, float ratioY){
    for (int i=0; i<count; i++)
        ScaleText(texts[i], ratioX, ratioY);
}

void ScaleFonts(TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton, float ratioY, const char* fontPath) {
    TTF_CloseFont(*fontNormal);
    TTF_CloseFont(*fontTitle);
    TTF_CloseFont(*fontButton);

    *fontNormal = TTF_OpenFont("FONT/fontPixel.ttf", (int)(32 * ratioY));
    *fontTitle  = TTF_OpenFont("FONT/fontPixel.ttf", (int)(38 * ratioY));
    *fontButton = TTF_OpenFont("FONT/fontPixel.ttf", (int)(22 * ratioY));

    if (!*fontNormal || !*fontTitle || !*fontButton)
        printf("Erreur ScaleFonts : %s\n", TTF_GetError());
}

void printText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y) {
    if (!font){ 
        printf("Erreur : font NULL\n"); return;
    }

    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture){
        SDL_FreeSurface(surface); return; 
    }

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void printTextCentered(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int y, int winW){
    if (!font || !text) return;

    int textW=0, textH=0;
    TTF_SizeUTF8(font, text, &textW, &textH);
    printText(renderer,font,text,color,(winW-textW)/2,y);
}

void printTextResponsive(SDL_Renderer *renderer, TTF_Font *font, const Text *t){
    if (!font || !t || !t->text) return;
        printText(renderer, font, t->text, t->color, t->x, t->y);
}