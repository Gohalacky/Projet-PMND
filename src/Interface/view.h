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
#include "ui/checkbox.h"
#include "propwindow.h"
#include "tradewindow.h"
#include "pawn.h"
#include "anim.h"

typedef struct LoadMenuState LoadMenuState;
typedef struct RulesMenuState RulesMenuState;

#define REF_W 1280.0f
#define REF_H 720.0f

typedef struct{
    /*Main Menu*/
    Button newGame, loadGame, save;
    /*Game Board*/
    Button backToMenu, throwDice, properties, trade, endTurn;
    /*Pause*/
    Button pauseMenu;        
    Button pauseSaveQuit;    
    Button pauseResume;     
    /*New Game*/
    Button launchGame, cancel;
    /*Carte popup*/
    Button okCarte;
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

typedef struct {
    Checkbox tourInfini;  
} CheckboxPack;

void renderJoueurActif(SDL_Renderer* renderer, TTF_Font* font,const EtatPartie* partie, ContexteJeu* ctx);
void refreshTextures(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex);
void renderCartePiochee(SDL_Renderer* renderer, TTF_Font* fontNormal, TTF_Font* fontTitle, const EtatPartie* partie, ButtonsPack* btn);
void renderEvenementActif(SDL_Renderer* renderer, TTF_Font* fontNormal, const ContexteJeu* ctx, int winW, int winH);
void renderGameOver(SDL_Renderer* renderer, TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontButton,
                    const EtatPartie* partie, ContexteJeu* ctx, ButtonsPack* btn);

void render(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex,
            TextsPack* texts, TTF_Font* fontButton, TTF_Font* fontNormal, TTF_Font* fontTitle,
            Counter* counterJoueurs, TextBoxPack* tboxes, CheckboxPack* checkboxes,
            EtatPartie* partie, ContexteJeu* ctx, ButtonsPack* btn,
            PropButtonsPack* pb, TradeButtonsPack* tb, TradeState* ts,
            Pawn pawns[MAX_PLAYERS],
            LoadMenuState* lm,
            RulesMenuState* rm,
            Animation* diceAnim);

void cleanup(SDL_Window* win, SDL_Renderer* rdr, TexturesPack* tex);

#endif