#ifndef LOADMENU_H
#define LOADMENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "Struct.h"
#include "savegame.h"
#include "assets.h"
#include "pawn.h"


#define LOAD_MAX_VISIBLE  6   


typedef struct LoadMenuState {
    SaveInfo saves[MAX_SAVES];  
    int      count;             
    int      selected;          
    int      scroll;            

    Button   btnScrollUp;
    Button   btnScrollDown;
    Button   btnBack;
} LoadMenuState;


void initLoadMenu(LoadMenuState* lm);

void scaleLoadMenu(LoadMenuState* lm, float rx, float ry);


void handleLoadMenuClick(LoadMenuState* lm, int x, int y,EtatJeu* state,EtatPartie* partie, ContexteJeu* ctx,Pawn pawns[MAX_PLAYERS]);

void renderLoadMenu(SDL_Renderer* renderer,TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontBtn,const LoadMenuState* lm,const TexturesPack* tex);

#endif 