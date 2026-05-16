#ifndef RULES_H
#define RULES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Struct.h"
#include "assets.h"

#define RULES_MAX_LINES   256
#define RULES_VISIBLE     18

typedef struct RulesMenuState {
    char  lines[RULES_MAX_LINES][256]; 
    int   count;                       
    int   scroll;                      
    Button btnScrollUp;
    Button btnScrollDown;
    Button btnBack;
} RulesMenuState;

/* Charge data/rules.txt dans l'état. Doit être appelé une fois. */
void initRulesMenu(RulesMenuState* rm);

/* Mise à l'échelle quand la fenêtre est redimensionnée */
void scaleRulesMenu(RulesMenuState* rm, float rx, float ry);

/* Gestion des clics */
void handleRulesMenuClick(RulesMenuState* rm, int x, int y, EtatJeu* state);

/* Gestion de la molette de défilement (delta > 0 = vers le haut) */
void handleRulesMenuScroll(RulesMenuState* rm, int delta);

/* Rendu du panneau règlement */
void renderRulesMenu(SDL_Renderer* renderer,
                     TTF_Font* fontTitle,
                     TTF_Font* fontNormal,
                     TTF_Font* fontBtn,
                     const RulesMenuState* rm,
                     const TexturesPack* tex);

#endif 