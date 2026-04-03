#include "view.h"
#include "adminWindow.h"
#include "anim.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>

int main(){
    float ratioX = 1.0f;
    float ratioY = 1.0f;
    EtatJeu   state = MENU;
    bool      quit  = false;
    SDL_Event event;

    Counter counterJoueurs;
    TextBoxPack tboxes;

    SDL_Window*   window   = SDL_CreateWindow("INSAPOLY",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,1280, 720,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Init();
    TTF_Font* fontButton = TTF_OpenFont("FONT/fontPixel.ttf", 22);
    TTF_Font* fontNormal = TTF_OpenFont("FONT/fontPixel.ttf", 32);
    TTF_Font* fontTitle  = TTF_OpenFont("FONT/fontPixel.ttf", 38);

    /* Boutons */
    TexturesPack tex;
    ButtonsPack btn = {
        .newGame    = { .ref = {457, 255, 370, 67} },
        .loadGame   = { .ref = {457, 337, 370, 67} },
        .save       = { .ref = {457, 417, 370, 67} },
        .backToMenu = { .ref = {1233,  2,  40, 40} },
        .throwDice  = { .ref = {1107, 559, 160, 50} },
        .properties = { .ref = {1107, 615, 160, 44} },
        .launchGame = { .ref = { 747, 636, 193, 45} },
        .cancel     = { .ref = { 359, 636, 134, 45} },
        .validMoney = { .ref = { 785, 422, 105, 36} },
    };

    

    /* Textes */
    TextsPack texts = {
        /*Menu*/
        .menuNewGame    = { "NOUVELLE PARTIE",      0, 275, 38, 0,290,38, ALIGN_CENTER_X, {235,221,192,255} },
        .menuLoadGame   = { "CHARGER LA PARTIE",    0, 356, 38, 0,371,38, ALIGN_CENTER_X, {235,221,192,255} },
        .menuSave       = { "SAUVEGARDES",          0, 436, 38, 0,451,38, ALIGN_CENTER_X, {235,221,192,255} },
 
        /*Nouvelle partie*/
        .titleNewGame   = { "Nouvelle Partie",      0,  53, 38, 0, 53,38, ALIGN_CENTER_X, {235,221,192,255} },
        .chooseParam    = { "Choix des parametres", 0, 256, 38, 0,256,38, ALIGN_CENTER_X, {235,221,192,255} },
        .numberPlayer   = { "Choix du nombre de joueurs", 0, 108, 38, 0,108,38, ALIGN_CENTER_X, {235,221,192,255} },
 
        /*Nouvelle partie*/
        .btnLaunchGame  = { "Lancer la partie",  790, 650, 22, 790,650,22, ALIGN_LEFT, {235,221,192,255} },
        .btnCancel      = { "Annuler",           392, 650, 22, 392,650,22, ALIGN_LEFT, {235,221,192,255} },
        .btnValid       = { "Valider",           810, 432, 22, 810,432,22, ALIGN_LEFT, {235,221,192,255} },
        .numbTurns      = { "Nombre de tours",   380, 310, 22, 380,310,22, ALIGN_LEFT, {235,221,192,255} },
        .tillBankruptcy = { "Ou jusqu'a la faillite", 380, 355, 22, 380,355,22, ALIGN_LEFT, {235,221,192,255} },
        .initialMoney   = { "Argent initial",    380, 430, 22, 380,430,22, ALIGN_LEFT, {235,221,192,255} },
        .event          = { "Frequence d'evenement", 380, 536, 22, 380,536,22, ALIGN_LEFT, {235,221,192,255} },
 
        /*Plateau*/
        .btnThrowDice   = { "Lancer les des",    1110, 570, 22, 1110,570,22, ALIGN_LEFT, {235,221,192,255} },
        .btnProperties  = { "Proprietes",        1110, 625, 22, 1110,625,22, ALIGN_LEFT, {235,221,192,255} },
        .btnEndTurn     = { "Fin de tour",       1110, 660, 22, 1110,660,22, ALIGN_LEFT, {235,221,192,255} },
    };

    if (loadTextures(renderer, &tex) < 0) return -1;
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || !(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
        printf("Erreur SDL : %s\n", SDL_GetError());
        return -1;
    }

    
    initTextBox(&tboxes.initialMoney, 630, 422,  140, 36);
    initTextBox(&tboxes.numbTurns,    630, 304,  140, 36);
    initTextBox(&tboxes.eventFreq,    630, 528,  140, 36);

    initCounter(&counterJoueurs, 2, 4, 2, 716, 172, 526, 172, 37, 37, 634, 176);
    
    Button* allButtons[] = {
        &btn.newGame, &btn.loadGame, &btn.save,
        &btn.backToMenu, &btn.throwDice, &btn.properties,
        &btn.launchGame, &btn.cancel, &btn.validMoney
    };

    int btnCount = sizeof(allButtons) / sizeof(allButtons[0]);
    for (int i = 0; i < btnCount; i++) {
        ScaleButton(allButtons[i], ratioX, ratioY);
    }
    ScaleButton(&counterJoueurs.btnPlus, ratioX, ratioY);
    ScaleButton(&counterJoueurs.btnMoins, ratioX, ratioY);

    Text* allTexts[] = {
        &texts.menuNewGame,&texts.menuLoadGame, &texts.menuSave,
        &texts.titleNewGame, &texts.btnLaunchGame, &texts.btnCancel, &texts.numberPlayer,
        &texts.chooseParam, &texts.initialMoney, &texts.tillBankruptcy,
        &texts.numbTurns, &texts.event, &texts.btnValid
    };
    int textCount = sizeof(allTexts) / sizeof(allTexts[0]);
    scaleAllTexts(allTexts, textCount,ratioX, ratioY);

    while (!quit) {
        handleEvents(&event, &state, &btn, &texts,
                     &fontNormal, &fontTitle, &fontButton,
                     &quit, &counterJoueurs, &tboxes);
        render(renderer, state, &tex, &texts,
               fontButton, fontNormal, fontTitle,
               &counterJoueurs, &tboxes);
    }

    TTF_CloseFont(fontNormal);
    TTF_CloseFont(fontTitle);
    TTF_CloseFont(fontButton);
    cleanup(window, renderer, &tex);
    return 0;
}
