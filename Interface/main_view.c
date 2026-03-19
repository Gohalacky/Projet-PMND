#include "view.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <stdbool.h>
#include <stdio.h>

int main(){

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || !(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
        printf("Erreur SDL : %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window*   window   = SDL_CreateWindow("INSAPOLY",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                1280, 720,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Init();

    TTF_Font* fontButton = TTF_OpenFont("FONT/fontPixel.ttf", 22);
    TTF_Font* fontNormal = TTF_OpenFont("FONT/fontPixel.ttf", 32);
    TTF_Font* fontTitle  = TTF_OpenFont("FONT/fontPixel.ttf", 38);

    /* ── Boutons ── */
    TexturesPack tex;
    ButtonsPack btn = {
        .newGame    = { .ref = {457, 255, 370, 67} },
        .loadGame   = { .ref = {457, 337, 370, 67} },
        .save       = { .ref = {457, 417, 370, 67} },
        .ranking    = { .ref = {457, 498, 370, 67} },
        .backToMenu = { .ref = {1233,  2,  40, 40} },
        .throwDice  = { .ref = {1107, 559, 160, 50} },
        .properties = { .ref = {1107, 615, 160, 44} },
        .launchGame = { .ref = { 747, 636, 193, 45} },
        .cancel     = { .ref = { 359, 636, 134, 45} },
        .validMoney = { .ref = { 785, 422, 105, 36} },
    };

    Button* allButtons[] = {
        &btn.newGame, &btn.loadGame, &btn.save,    &btn.ranking,
        &btn.backToMenu, &btn.throwDice, &btn.properties,
        &btn.launchGame, &btn.cancel, &btn.validMoney
    };

    /* ── Textes statiques ── */
    TextsPack texts = {
        .titleNewGame   = {"Nouvelle Partie",        541,  53, 38, 0,0,0, {235,221,192,255}},
        .btnLaunchGame  = {"Lancer la partie",        790, 650, 22, 0,0,0, {235,221,192,255}},
        .btnCancel      = {"Annuler",                 392, 650, 22, 0,0,0, {235,221,192,255}},
        .btnValid       = {"Valider",                 810, 432, 22, 0,0,0, {235,221,192,255}},

        .numbTurns      = {"Nombre de tours",         380, 310, 22, 380, 316, 22, {235,221,192,255}},
        .tillBankruptcy = {"Ou jusqu'a la faillite",  380, 355, 22, 380, 351, 22, {235,221,192,255}},
        .initialMoney   = {"Argent initial",          380, 430, 22, 380, 436, 22, {235,221,192,255}},
        .event          = {"Frequence d'evenement",   380, 536, 22, 380, 436, 22, {235,221,192,255}},

        .chooseParam    = {"Choix des parametres",    500, 256, 38, 0,0,0, {235,221,192,255}},
        .numberPlayer   = {"Choix du nombre de joueurs", 450, 108, 38, 0,0,0, {235,221,192,255}},
    };

    Text* allTexts[] = {
        &texts.titleNewGame, &texts.btnLaunchGame, &texts.btnCancel, &texts.numberPlayer,
        &texts.chooseParam, &texts.initialMoney, &texts.tillBankruptcy,
        &texts.numbTurns, &texts.event, &texts.btnValid
    };

    /* Zones de saisie */
    TextBoxPack tboxes;
    initTextBox(&tboxes.initialMoney, 630, 422,  140, 36);
    initTextBox(&tboxes.numbTurns,    630, 304,  140, 36);
    initTextBox(&tboxes.eventFreq,    630, 528,  140, 36);

    /* Compteur joueurs  */
    Counter counterJoueurs;
    initCounter(&counterJoueurs, 2, 4, 2, 716, 172, 526, 172, 37, 37, 634, 176);

    float ratioX = 1.0f;
    float ratioY = 1.0f;
    int count = sizeof(allButtons) / sizeof(allButtons[0]);
    for (int i = 0; i < count; i++) {
        ScaleButton(allButtons[i], ratioX, ratioY);
        ScaleText(&counterJoueurs.label, ratioX, ratioY);
    }
    int textCount = sizeof(allTexts) / sizeof(allTexts[0]);
    for (int i = 0; i < textCount; i++)
        ScaleText(allTexts[i], 1.0f, 1.0f);

    if (loadTextures(renderer, &tex) < 0)
        return -1;

    EtatJeu   state = MENU;
    bool      quit  = false;
    SDL_Event event;

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
