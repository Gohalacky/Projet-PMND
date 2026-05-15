#ifndef PROPWINDOW_H
#define PROPWINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "assets.h"
#include "bridge.h"
#include "Struct.h"
#include "ui/button.h"
#include "ui/text.h"

#ifndef REF_W
#define REF_W 1280.0f
#define REF_H  720.0f
#endif

typedef struct PropButtonsPack {
    Button fermer;
    Button acheter;
    Button construire;
    Button construireSocial;
    Button construireParc;
    Button echanger;
} PropButtonsPack;

void initPropButtons(PropButtonsPack* pb);
void scalePropButtons(PropButtonsPack* pb, float ratioX, float ratioY);

void renderPropWindow(SDL_Renderer* renderer,TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontSmall,const TexturesPack* tex,ContexteJeu* ctx,const EtatPartie* partie,PropButtonsPack* pb,int winW, int winH);

#include "tradewindow.h"
void handlePropClick(int x, int y, EtatJeu* state,PropButtonsPack* pb,ContexteJeu* ctx,EtatPartie* partie, TradeState* ts);

#endif