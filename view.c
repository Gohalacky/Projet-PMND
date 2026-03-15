#include "view.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void printText(SDL_Renderer* renderer, TTF_Font* font,const char* text, SDL_Color color, int x, int y) {
    if (!font) { printf("Erreur : font NULL\n"); return; }

    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) { 
        SDL_FreeSurface(surface); 
        return; 
    }

    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int loadTextures(SDL_Renderer* renderer, TexturesPack* tex){
    tex->menu        = IMG_LoadTexture(renderer, "IMAGE/menu.png");
    tex->newGame  = IMG_LoadTexture(renderer, "IMAGE/launchGame.png");
    tex->launchGame = IMG_LoadTexture(renderer, "IMAGE/chargeGame.png");
    tex->gameBoard     = IMG_LoadTexture(renderer, "IMAGE/gameBoard.png");
    tex->throws      = IMG_LoadTexture(renderer, "IMAGE/throws.png");
    tex->menuProperties   = IMG_LoadTexture(renderer, "IMAGE/Menu_interne.png");
    tex->test        = IMG_LoadTexture(renderer, "IMAGE/TEST.png");

    // Vérification centralisée
    SDL_Texture* all[] = {
        tex->menu, tex->newGame, tex->launchGame,
        tex->gameBoard, tex->throws, tex->menuProperties, tex->test
    };
    for (int i = 0; i < 7; i++) {
        if (!all[i]) {
            printf("Erreur chargement texture %d : %s\n", i, IMG_GetError());
            return -1;
        }
    }
    return 0;
}

bool isClicked(Button* btn, int x, int y) {
    return x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
           y >= btn->rect.y && y <= btn->rect.y + btn->rect.h;
}

void handleEvents(SDL_Event* event, EtatJeu* state, ButtonsPack* btn, TextsPack* texts, TTF_Font** fontNormal, TTF_Font** fontTitle, bool* quit) {
    while (SDL_PollEvent(event)) {

        if (event->type == SDL_QUIT) {
            *quit = true;
            return;
        }

        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE)
            *state = MENU;

        if (event->type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            printf("clic de souris : x=%d, y=%d\n",x,y);

            switch (*state) {
                case MENU:
                    if (isClicked(&btn->newGame,   x, y)) *state = NOUVPARTIE;
                    if (isClicked(&btn->loadGame,  x, y)) *state = CHARGERPARTIE;
                    if (isClicked(&btn->save,      x, y)) *state = TEST;
                    if (isClicked(&btn->ranking,   x, y)) *state = TEST;
                    break;
                case NOUVPARTIE:
                    if (isClicked(&btn->launchGame, x, y)) *state = PLATEAU;
                    if (isClicked(&btn->cancel,     x, y)) *state = MENU;
                    break;
                case PLATEAU:
                case LANCER:
                    if (isClicked(&btn->backToMenu, x, y)) *state = MENU;
                    if (isClicked(&btn->throwDice,  x, y)) *state = LANCER;
                    if (isClicked(&btn->properties, x, y)) *state = PROPRIETE;
                    break;
                default: break;
            }
        }
        

        // Gestion du resize
        if (event->type == SDL_WINDOWEVENT) {
            if (event->window.event == SDL_WINDOWEVENT_RESIZED) {

                float ratioX = (float)event->window.data1 / REF_W;
                float ratioY = (float)event->window.data2 / REF_H;

                Button* allButtons[] = {
                    &btn->newGame, &btn->loadGame, &btn->save,    &btn->ranking,
                    &btn->backToMenu, &btn->throwDice, &btn->properties,
                    &btn->launchGame, &btn->cancel
                };
                
                int count = sizeof(allButtons) / sizeof(allButtons[0]);
                for (int i = 0; i < count; i++) {
                    ScaleButton(allButtons[i],ratioX, ratioY);
                }

                Text* allTexts[] = {
                    &texts->titleNewGame, &texts->btnLaunchGame, &texts->btnCancel,
                    &texts->btnThrowDice, &texts->btnProperties
                };
                int textCount = sizeof(allTexts) / sizeof(allTexts[0]);
                for (int i = 0; i < textCount; i++){
                    ScaleText(allTexts[i], ratioX, ratioY);
                } 
                ScaleFonts(fontNormal, fontTitle, ratioY);
            }
        }
    }
}

void render(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex, ButtonsPack* btn, TextsPack* texts, TTF_Font* fontNormal, TTF_Font* fontTitle) {
    SDL_RenderClear(renderer);

    switch (state) {

        case MENU:
            SDL_RenderCopy(renderer, tex->menu, NULL, NULL);
            break;

        case NOUVPARTIE:
            SDL_RenderCopy(renderer, tex->newGame, NULL, NULL);
            printText(renderer, fontTitle,  texts->titleNewGame.text,  texts->titleNewGame.color,  texts->titleNewGame.x,  texts->titleNewGame.y);
            printText(renderer, fontNormal, texts->btnLaunchGame.text, texts->btnLaunchGame.color, texts->btnLaunchGame.x, texts->btnLaunchGame.y);
            printText(renderer, fontNormal, texts->btnCancel.text,     texts->btnCancel.color,     texts->btnCancel.x,     texts->btnCancel.y);
            break;

        case CHARGERPARTIE:
            SDL_RenderCopy(renderer, tex->launchGame, NULL, NULL);
            break;

        case PROPRIETE:
            SDL_RenderCopy(renderer, tex->menuProperties, NULL, NULL);
            break;

        case TEST:
            SDL_RenderCopy(renderer, tex->test, NULL, NULL);
            break;

        case PLATEAU:
        case LANCER:
            SDL_RenderCopy(renderer, tex->gameBoard, NULL, NULL);
            printText(renderer, fontNormal, texts->btnThrowDice.text,  texts->btnThrowDice.color,  texts->btnThrowDice.x,  texts->btnThrowDice.y);
            printText(renderer, fontNormal, texts->btnProperties.text, texts->btnProperties.color, texts->btnProperties.x, texts->btnProperties.y);

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 90);
            SDL_RenderFillRect(renderer, &btn->backToMenu.rect);
            SDL_RenderFillRect(renderer, &btn->properties.rect);
            SDL_RenderFillRect(renderer, &btn->throwDice.rect);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            if (state == LANCER) {
                SDL_Rect fenetreLancer = {340, 100, 600, 400};
                SDL_RenderCopy(renderer, tex->throws, NULL, &fenetreLancer);
            }
            break;
    }

    SDL_RenderPresent(renderer);
}

void cleanup(SDL_Window* win, SDL_Renderer* rdr, TexturesPack* tex){
    SDL_DestroyTexture(tex->menu);
    SDL_DestroyTexture(tex->newGame);
    SDL_DestroyTexture(tex->launchGame);
    SDL_DestroyTexture(tex->gameBoard);
    SDL_DestroyTexture(tex->throws);
    SDL_DestroyTexture(tex->menuProperties);
    SDL_DestroyTexture(tex->test);
    SDL_DestroyRenderer(rdr);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
}

void ScaleButton(Button* btn, float ratioX, float ratioY){
    btn->rect.x = (int)(btn->ref.x * ratioX);
    btn->rect.y = (int)(btn->ref.y * ratioY);
    btn->rect.w = (int)(btn->ref.w * ratioX);
    btn->rect.h = (int)(btn->ref.h * ratioY);
}

void ScaleText(Text* t, float ratioX, float ratioY){
    t->x    = (int)(t->refX    * ratioX);
    t->y    = (int)(t->refY    * ratioY);
    t->size = (int)(t->refSize * ratioY);
}

void ScaleFonts(TTF_Font** fontNormal, TTF_Font** fontTitle, float ratioY) {
    TTF_CloseFont(*fontNormal);
    TTF_CloseFont(*fontTitle);
    *fontNormal = TTF_OpenFont("FONT/fontPixel.ttf", (int)(22 * ratioY));
    *fontTitle  = TTF_OpenFont("FONT/fontPixel.ttf", (int)(38 * ratioY));

    if (!*fontNormal || !*fontTitle) {
        printf("Erreur ScaleFonts : %s\n", TTF_GetError());
    }
}