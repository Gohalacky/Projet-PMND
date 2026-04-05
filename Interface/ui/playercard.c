#include "playercard.h"
#include "text.h"

#include <stdio.h>

#define CARD_REF_W 110
#define CARD_REF_H 110
#define REF_OFFSET_ETUDIANT_X 4
#define REF_OFFSET_CR_ECO__X 55
#define REF_FONT_SIZE 22

const CardRef CARDS_2J[2] = { {11,175,110,110,},{1130,175,110,110}};
const CardRef CARDS_3J[3] = {{11,175,110,110,},{11,335,110,110},{1130,175,110,110}};
const CardRef CARDS_4J[4] = { {11,175,110,110,},{18,335,110,110},{1130,175,110,110},{1130,335,110,110}};

void renderPlayerCard(SDL_Renderer *renderer, TTF_Font *font, const Joueur *j, int numero, SDL_Rect rect){
    char buffer[64];
    SDL_Color blanc = {235,221,192,255};

    float ratioW = (float)rect.w/CARD_REF_W;
    float ratioH = (float)rect.h/CARD_REF_H;

    int fontSize=rect.h*22/110;
    if(fontSize<8){
        fontSize=8;
    } 

    TTF_Font* scaledFont = TTF_OpenFont("FONT/fontPixel.ttf", fontSize);
    if (!scaledFont){
        scaledFont = font;
    } 

    int lineH = TTF_FontHeight(font);
    int totalH = lineH*3;
    int baseY = rect.y + (rect.h-totalH)/2;
    
    int offsetEdutiant = rect.x+(int)(REF_OFFSET_ETUDIANT_X*ratioW);
    int offsetCrEco = rect.x+(int)(REF_OFFSET_CR_ECO__X*ratioW);

    snprintf(buffer,sizeof(buffer), "Etudiant %d",numero);
    printText(renderer, font, buffer, blanc,offsetEdutiant,baseY);

    snprintf(buffer,sizeof(buffer), "CR : %d",j->credits);
    printText(renderer, font, buffer, blanc,offsetCrEco,baseY+lineH*1.7);

    snprintf(buffer,sizeof(buffer), "Eco : %1.f",j->noteEco);
    printText(renderer, font, buffer, blanc,offsetCrEco,baseY+lineH*2.9);

    if(scaledFont!=font){
        TTF_CloseFont(scaledFont);
    }
}