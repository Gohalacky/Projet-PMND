#ifndef ASSETS_H
#define ASSETS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

typedef struct{
    SDL_Texture* menu;
    SDL_Texture* newGame;
    SDL_Texture* launchGame;
    SDL_Texture* gameBoard2Player;
    SDL_Texture* gameBoard3Player;
    SDL_Texture* gameBoard4Player;
    SDL_Texture* throws;
    SDL_Texture* diceSheet;
    SDL_Texture* menuProperties;
    SDL_Texture* test;

    SDL_Texture* maison[3];
    SDL_Texture* immeuble[3];
    SDL_Texture* parc[3];
    SDL_Texture* fondProp;
    SDL_Texture* pion[4];   
} TexturesPack;

int loadTextures(SDL_Renderer* renderer, TexturesPack* tex);

void freeTextures(TexturesPack* tex);

#endif