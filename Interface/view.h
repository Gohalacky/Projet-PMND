#ifndef VIEW_H
#define VIEW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Struct.h"
#include "assets.h"
#include "bridge.h"
#include "ui/text.h"
#include "ui/counter.h"
#include "ui/textbox.h"

#define REF_W 1280.0f
#define REF_H 720.0f

typedef struct{
    /*Main Menu*/
    Button newGame, loadGame, save;
    /*Game Board*/
    Button backToMenu, throwDice, properties, endTurn;
    /*New Game*/
    Button launchGame, cancel;
} ButtonsPack;

typedef struct {
    /*Menu*/
    Text menuNewGame;
    Text menuLoadGame;
    Text menuSave;
    /*New Game*/
    Text titleNewGame;
    Text numberPlayer;
    Text chooseParam;
    Text btnLaunchGame;
    Text btnCancel;
    Text initialMoney;
    Text tillBankruptcy;
    Text numbTurns;
    Text event;

    /*Game Board*/
    Text btnThrowDice;
    Text btnProperties;
    Text btnEndTurn;
    Text indNbTurn;
    Text indJoueurActif;
} TextsPack;

void renderJoueurActif(SDL_Renderer* renderer, TTF_Font* font,const EtatPartie* partie, const ContexteJeu* ctx);

void render(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex,
            TextsPack* texts, TTF_Font* fontButton, TTF_Font* fontNormal, TTF_Font* fontTitle,
            Counter* counterJoueurs, TextBoxPack* tboxes, EtatPartie* partie, const ContexteJeu* ctx);

void cleanup(SDL_Window* win, SDL_Renderer* rdr, TexturesPack* tex);

#endif
