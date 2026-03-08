#ifndef VIEW
#define VIEW

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

#include <stdio.h>
#include <string.h>

#define REF_W 1280.0f
#define REF_H 720.0f

#define REF_BTN_W 370.0f
#define REF_BTN_H 67.0f

struct RelativeButton{
    float relX, relY;
    float relW, relH;
    SDL_Rect drawRect; 
};

typedef enum {MENU,PLATEAU,LANCER,PROPRIETE,ADMIN,CARTECHANCE,CARTECOMMUNAUTE, TEST} EtatJeu;

void UpdateButtonPositionsScaled(int windowW, int windowH, SDL_Rect* bNew, SDL_Rect* bLoad, SDL_Rect* bSave, SDL_Rect* bRank);

void updateTextPosition(int windowW, int windowH);

void printText(SDL_Renderer* rend, char* f, int size, int valeur, SDL_Color col, int x, int y, char* txt);


#endif