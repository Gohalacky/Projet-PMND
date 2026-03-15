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



int main(int argc, char* args[]){

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || !(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
        printf("Erreur SDL : %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window*   window   = SDL_CreateWindow("INSAPOLY", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, 1280, 720,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Init(); 

    TTF_Font* fontNormal = TTF_OpenFont("FONT/fontPixel.ttf", 22);
    TTF_Font* fontTitle  = TTF_OpenFont("FONT/fontPixel.ttf", 38);  

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
    };

    Button* allButtons[] = {
        &btn.newGame, &btn.loadGame, &btn.save,    &btn.ranking,
        &btn.backToMenu, &btn.throwDice, &btn.properties,
        &btn.launchGame, &btn.cancel
    };

    TextsPack texts = {
        .titleNewGame  = {"Nouvelle Partie",      541, 53,  38, 0,0,0, {235,221,192,255}},
        .btnLaunchGame = {"Lancer la partie",      790, 650, 22, 0,0,0, {235,221,192,255}},
        .btnCancel     = {"Annuler",               392, 650, 22, 0,0,0, {235,221,192,255}},
        .btnThrowDice  = {"Lancer les dés",       1123, 582, 22, 0,0,0, {235,221,192,255}},
        .btnProperties = {"Gérer les propriétés", 1123, 636, 22, 0,0,0, {235,221,192,255}},
    };

    Text* allTexts[] = {
        &texts.titleNewGame, &texts.btnLaunchGame, &texts.btnCancel,
        &texts.btnThrowDice, &texts.btnProperties
    };
    int textCount = sizeof(allTexts) / sizeof(allTexts[0]);
    for (int i = 0; i < textCount; i++){
        ScaleText(allTexts[i], 1.0f, 1.0f);
    } 

    float ratioX = 1.0f; // fenêtre démarre à 1280x720 = ratio 1
    float ratioY = 1.0f;
    int count = sizeof(allButtons) / sizeof(allButtons[0]);
    for (int i = 0; i < count; i++) {
        ScaleButton(allButtons[i], ratioX, ratioY);
    }

    if (loadTextures(renderer, &tex) < 0){
        return -1;
    } 

    EtatJeu   state = MENU;
    bool      quit  = false;
    SDL_Event event;

    while (!quit) {
        handleEvents(&event, &state, &btn, &texts, &fontNormal, &fontTitle, &quit);
        render(renderer, state, &tex, &btn, &texts, fontNormal, fontTitle);
    }

    TTF_CloseFont(fontNormal);
    TTF_CloseFont(fontTitle);
    cleanup(window, renderer, &tex);
    return 0;

    
}