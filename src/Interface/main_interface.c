#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>
#include <stdio.h>

#include "Struct.h"
#include "assets.h"
#include "view.h"
#include "events.h"
#include "bridge.h"
#include "ui/counter.h"
#include "ui/textbox.h"
#include "ui/button.h"
#include "ui/checkbox.h"
#include "propwindow.h"
#include "tradewindow.h"
#include "prisonwindow.h"
#include "pawn.h"
#include "anim.h"
#include "pausemenu.h"
#include "loadmenu.h"
#include "rules.h"

#define FONT "FONT/fontPixel.ttf"

int main(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erreur SDL : %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    if(TTF_Init()<0){
        printf("Erreur TTF_Init : %s\n",TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return -1;
    }
    
    SDL_Window*   window   = SDL_CreateWindow("INSAPOLY",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,1280, 720,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* fontPetit  = TTF_OpenFont("assets/FONT/fontPixel.ttf",18);
    TTF_Font* fontButton = TTF_OpenFont("assets/FONT/fontPixel.ttf", 22);
    TTF_Font* fontNormal = TTF_OpenFont("assets/FONT/fontPixel.ttf", 32);
    TTF_Font* fontTitle  = TTF_OpenFont("assets/FONT/fontPixel.ttf", 38);

    TexturesPack tex;
    memset(&tex,0,sizeof(tex));
    if(loadTextures(renderer, &tex)<0){
        cleanup(window, renderer, &tex);
        return-1;
    }
    
    ButtonsPack btn = {
        .newGame    = { .ref = {457, 255, 370, 67} },
        .loadGame   = { .ref = {457, 337, 370, 67} },
        .save       = { .ref = {457, 417, 370, 67} },
        .backToMenu = { .ref = {1233,  2,  40, 40} },

        .throwDice  = { .ref = {1107, 559, 160, 50}},
        .properties = { .ref = {1107, 615, 160, 44}},

        .trade      = { .ref = {1107, 663, 160, 44}},
        .endTurn    = { .ref = {1107, 666, 160, 44}, .disabled = 1},

        .launchGame = { .ref = { 747, 636, 193, 45}},
        .cancel     = { .ref = { 359, 636, 134, 45}},
        
    };

    /* Initialisation des boutons du menu pause */
    initPauseButtons(&btn);

    TextsPack texts = {
        /*Menu*/
        .menuNewGame    = { "NOUVELLE PARTIE",      0, 270, 38, 0,290,38, ALIGN_CENTER_X, {235,221,192,255} },
        .menuLoadGame   = { "CHARGER LA PARTIE",    0, 351, 38, 0,371,38, ALIGN_CENTER_X, {235,221,192,255} },
        .menuSave       = { "REGLEMENT",          0, 431, 38, 0,451,38, ALIGN_CENTER_X, {235,221,192,255} },
 
        /*Nouvelle partie*/
        .titleNewGame   = { "Nouvelle Partie",      0,  53, 38, 0, 53,38, ALIGN_CENTER_X, {235,221,192,255} },
        .chooseParam    = { "Choix des parametres", 0, 250, 38, 0,256,38, ALIGN_CENTER_X, {235,221,192,255} },
        .numberPlayer   = { "Choix du nombre de joueurs", 0, 104, 38, 0,108,38, ALIGN_CENTER_X, {235,221,192,255} },

        .btnLaunchGame  = { "Lancer la partie",  790, 650, 22, 790,650,22, ALIGN_LEFT, {235,221,192,255} },
        .btnCancel      = { "Annuler",           395, 650, 22, 395,650,22, ALIGN_LEFT, {235,221,192,255} },
        .numbTurns      = { "Nombre de tours",   380, 310, 22, 380,310,22, ALIGN_LEFT, {235,221,192,255} },
        .tillBankruptcy = { "Ou jusqu'a la faillite", 380, 355, 22, 380,355,22, ALIGN_LEFT, {235,221,192,255} },
        .initialMoney   = { "Argent initial",    380, 430, 22, 380,430,22, ALIGN_LEFT, {235,221,192,255} },
        .event          = { "Frequence d'evenement", 380, 533, 22, 380,536,22, ALIGN_LEFT, {235,221,192,255} },
 
        /*Plateau*/
        .btnThrowDice   = { "Lancer les des",    1125, 570, 22, 1125,575,22, ALIGN_LEFT, {235,221,192,255} },
        .btnProperties  = { "Proprietes",        1145, 622, 22, 1145,626,22, ALIGN_LEFT, {235,221,192,255} },
        .btnEndTurn     = { "Fin de tour",       1145, 674, 22, 1145,678,22, ALIGN_LEFT, {235,221,192,255} },
        .indNbTurn      = { "Tour",              105, 579, 22, 105,584,22, ALIGN_LEFT, {235,221,192,255} },
    };

    TextBoxPack tboxes;
    initTextBox(&tboxes.initialMoney, 680, 422,  140, 36);
    initTextBox(&tboxes.numbTurns,    680, 304,  140, 36);
    initTextBox(&tboxes.eventFreq,    680, 528,  140, 36);

    CheckboxPack checkboxes;
    checkboxes.tourInfini = (Checkbox){ .ref = {680, 361, 20, 20}, .checked = 0 };
    ScaleCheckbox(&checkboxes.tourInfini, 1.0f, 1.0f);

    PropButtonsPack pb;
    initPropButtons(&pb);

    TradeButtonsPack tb;
    initTradeButtons(&tb);

    PrisonButtonsPack pb_prison;
    initPrisonButtons(&pb_prison);

    TradeState ts;
    memset(&ts, 0, sizeof(ts));
    
    Counter counterJoueurs;
    initCounter(&counterJoueurs, 2, 4, 2, 716, 172, 526, 172, 37, 37, 634, 176);
    
    EtatPartie partie = {.nbJoueurs = 2, .joueursActif = 0, .tourActuel = 1, .tourMax = 20};

    ContexteJeu ctx;
    memset(&ctx,0,sizeof(ctx));

    Pawn pawns[MAX_PLAYERS];
    initPawns(pawns, 2);

    LoadMenuState lm;
    initLoadMenu(&lm);

    RulesMenuState rm;
    initRulesMenu(&rm);

    Animation diceAnim;
    initAnim(&diceAnim, tex.diceSheet, 156, 153, 27, 9, 15, true);

    Button* allButtons[] = {
        &btn.newGame, &btn.loadGame, &btn.save,
        &btn.backToMenu, &btn.throwDice, &btn.properties,
        &btn.trade, &btn.launchGame, &btn.cancel, &btn.endTurn,
        &btn.pauseMenu, &btn.pauseResume, &btn.pauseSaveQuit,
    };

    int btnCount = sizeof(allButtons) / sizeof(allButtons[0]);
    for (int i = 0; i < btnCount; i++) {
        ScaleButton(allButtons[i], 1.0f, 1.0f);
    }
    ScaleButton(&counterJoueurs.btnPlus, 1.0f, 1.0f);
    ScaleButton(&counterJoueurs.btnMoins, 1.0f, 1.0f);

    Text* allTexts[] = {
        &texts.menuNewGame,&texts.menuLoadGame, &texts.menuSave,
        &texts.titleNewGame, &texts.btnLaunchGame, &texts.btnCancel, &texts.numberPlayer,
        &texts.chooseParam, &texts.initialMoney, &texts.tillBankruptcy,
        &texts.numbTurns, &texts.event,
        &texts.btnThrowDice, &texts.btnProperties, &texts.btnEndTurn, &texts.indNbTurn
    };
    int textCount = sizeof(allTexts) / sizeof(allTexts[0]);
    ScaleAllTexts(allTexts, textCount,1.0f, 1.0f);


    EtatJeu   state = MENU;
    EtatJeu   prevState = MENU;
    bool      quit  = false;
    SDL_Event event;
    Uint32 lastTime=SDL_GetTicks();

    MoneyPopup popups[MAX_PLAYERS];
    memset(popups, 0, sizeof(popups));

    while (!quit) {
        Uint32 now = SDL_GetTicks();
        Uint32 delta = now-lastTime;
        lastTime=now;

        int creditsAvant[MAX_PLAYERS];
        for (int i = 0; i < partie.nbJoueurs; i++)
            creditsAvant[i] = partie.joueurs[i].credits;

        prevState = state;
        handleEvents(&event, &state, &btn, &texts,&fontNormal, &fontTitle, &fontButton,&quit, &counterJoueurs, &tboxes, &checkboxes, &partie, &ctx, &pb, &tb, &ts, pawns, &lm, &rm, &pb_prison);

        int gameState = (state == PLATEAU2J || state == PLATEAU3J || state == PLATEAU4J|| state == LANCER || state == CARTE || state == PRISON);
        int wasGame   = (prevState == PLATEAU2J || prevState == PLATEAU3J || prevState == PLATEAU4J|| prevState == LANCER || prevState == CARTE || prevState == PRISON);
        if (gameState && wasGame) {
            for (int i = 0; i < partie.nbJoueurs; i++) {
                int diff = partie.joueurs[i].credits - creditsAvant[i];
                if (diff != 0) {
                    popups[i].delta   = diff;
                    popups[i].alpha   = 255.0f;
                    popups[i].offsetY = 0.0f;
                    popups[i].active  = 1;
                }
            }
        }

        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (popups[i].active) {
                popups[i].alpha   -= (float)delta * (255.0f / 2500.0f);
                popups[i].offsetY += (float)delta * 0.035f;
                if (popups[i].alpha <= 0.0f) popups[i].active = 0;
            }
        }

        if (partie.prisonFailAlpha > 0.0f) {
            partie.prisonFailAlpha -= (float)delta * (255.0f / 5000.0f);
            if (partie.prisonFailAlpha < 0.0f) partie.prisonFailAlpha = 0.0f;
        }

        updatePawns(pawns, partie.nbJoueurs, delta);
        if (state == LANCER && prevState != LANCER) resetAnim(&diceAnim);
        if (state == LANCER) updateAnim(&diceAnim, delta);
        render(renderer, state, &tex, &texts,fontButton, fontNormal, fontTitle,&counterJoueurs, &tboxes, &checkboxes, &partie, &ctx, &btn, &pb, &tb, &ts, pawns, &lm, &rm, &diceAnim, &pb_prison, popups);
    }

    TTF_CloseFont(fontNormal);
    TTF_CloseFont(fontTitle);
    TTF_CloseFont(fontButton);
    TTF_CloseFont(fontPetit);
    TTF_Quit();
    cleanup(window, renderer, &tex);
    return 0;
}