#include "view.h"
#include "Struct.h"
#include "assets.h"
#include "ui/playercard.h"
#include "ui/text.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>


static void renderPlateau(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex, TextsPack* texts, TTF_Font* fontButton, EtatPartie* partie){
    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    SDL_Texture* board = tex->gameBoard2Player;
    if      (state == PLATEAU3J) board = tex->gameBoard3Player;
    else if (state == PLATEAU4J) board = tex->gameBoard4Player;
    SDL_RenderCopy(renderer, board, NULL, NULL);
 
    printTextResponsive(renderer, fontButton, &texts->btnThrowDice);
    printTextResponsive(renderer, fontButton, &texts->btnProperties);
    printTextResponsive(renderer, fontButton, &texts->btnEndTurn);
    printTextResponsive(renderer, fontButton, &texts->indNbTurn);
 
    const CardRef* cards;
    switch (partie->nbJoueurs) {
        case 3:  cards = CARDS_3J; break;
        case 4:  cards = CARDS_4J; break;
        default: cards = CARDS_2J; break;
    }
    for (int i = 0; i < partie->nbJoueurs; i++) {
        SDL_Rect rect = {
            (int)(cards[i].x * winW / REF_W),
            (int)(cards[i].y * winH / REF_H),
            (int)(cards[i].w * winW / REF_W),
            (int)(cards[i].h * winH / REF_H),
        };
        renderPlayerCard(renderer, fontButton, &partie->joueurs[i], i + 1, rect);
    }

    if (state == LANCER) {

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_Rect overlay = { 0, 0, winW, winH };
        SDL_RenderFillRect(renderer, &overlay);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        int ja = partie->joueursActif;
        SDL_Rect rect = {
            (int)(cards[ja].x * winW / REF_W),
            (int)(cards[ja].y * winH / REF_H),
            (int)(cards[ja].w * winW / REF_W),
            (int)(cards[ja].h * winH / REF_H),
        };
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 235, 221, 192, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        renderPlayerCard(renderer, fontButton,&partie->joueurs[ja], ja + 1, rect);

        SDL_Rect fenetreLancer = {
            (int)(340.0f * winW / REF_W),
            (int)(100.0f * winH / REF_H),
            (int)(600.0f * winW / REF_W),
            (int)(400.0f * winH / REF_H),
        };
        SDL_RenderCopy(renderer, tex->throws, NULL, &fenetreLancer);
    }
}

static void renderJoueurs(SDL_Renderer* renderer, TTF_Font* font,EtatJeu state, EtatPartie* partie, int winW, int winH){
    const CardRef* cards;
    switch (state){
        case PLATEAU3J: cards=CARDS_3J; break;
        case PLATEAU4J: cards=CARDS_4J; break;
        default: cards=CARDS_2J; break;
    }

    for (int i=0; i<partie->nbJoueurs; i++){
        SDL_Rect rect = { (int)(cards[i].x*winW/REF_W), (int)(cards[i].y*winH/REF_H),(int)(cards[i].w*winW/REF_W),(int)(cards[i].h*winH/REF_H)};
        renderPlayerCard(renderer, font, &partie->joueurs[i], i+1, rect);
    }
}

void render(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex, TextsPack* texts, TTF_Font* fontButton, TTF_Font* fontNormal, TTF_Font* fontTitle,Counter* counterJoueurs, TextBoxPack* tboxes, EtatPartie* partie){
    
    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);
    SDL_RenderClear(renderer);

    switch (state) {

        case MENU:
            SDL_RenderCopy(renderer, tex->menu, NULL, NULL);
            printTextCentered(renderer, fontTitle,  texts->menuLoadGame.text,  texts->menuLoadGame.color,  texts->menuLoadGame.y, winW);
            printTextCentered(renderer, fontTitle,  texts->menuNewGame.text,  texts->menuNewGame.color,  texts->menuNewGame.y,  winW);
            printTextCentered(renderer, fontTitle,  texts->menuSave.text,  texts->menuSave.color,  texts->menuSave.y, winW);
            break;

        case NOUVPARTIE:
            SDL_RenderCopy(renderer, tex->newGame, NULL, NULL);
            printTextCentered(renderer, fontTitle,  texts->titleNewGame.text,  texts->titleNewGame.color,  texts->titleNewGame.y,  winW);
            printTextCentered(renderer, fontTitle,  texts->chooseParam.text,   texts->chooseParam.color,   texts->chooseParam.y,   winW);
            printTextCentered(renderer, fontTitle,  texts->numberPlayer.text,  texts->numberPlayer.color,  texts->numberPlayer.y,  winW);
            
            printTextResponsive(renderer, fontButton, &texts->btnLaunchGame);
            printTextResponsive(renderer, fontButton, &texts->btnCancel);
            printTextResponsive(renderer, fontNormal, &texts->initialMoney);
            printTextResponsive(renderer, fontNormal, &texts->tillBankruptcy);
            printTextResponsive(renderer, fontNormal, &texts->numbTurns);
            printTextResponsive(renderer, fontNormal, &texts->event);

            renderCounter(renderer, counterJoueurs, fontTitle, 634, 176);

            renderTextBox(renderer, fontNormal, &tboxes->initialMoney);
            renderTextBox(renderer, fontNormal, &tboxes->numbTurns);
            renderTextBox(renderer, fontNormal, &tboxes->eventFreq);
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
        case PLATEAU2J:
        case PLATEAU3J:
        case PLATEAU4J:
        case LANCER:
            renderPlateau(renderer, state, tex, texts, fontButton, partie);
            renderJoueurs(renderer, fontNormal, state, partie, winW, winH);
            printTextResponsive(renderer, fontButton, &texts->indNbTurn);
            break;
    }
    SDL_RenderPresent(renderer);
}

void cleanup(SDL_Window* win, SDL_Renderer* rdr, TexturesPack* tex){
    freeTextures(tex);
    SDL_DestroyRenderer(rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
