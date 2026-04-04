#include "playercard.h"
#include "text.h"

#include <stdio.h>

const CardRef CARDS_2J[2] = { {11,181,110,110,},{1100,181,110,110}};
const CardRef CARDS_3J[3] = {{11,181,110,110,},{11,341,110,110},{1100,181,110,110}};
const CardRef CARDS_4J[4] = { {11,182,110,110,},{18,341,110,110},{1100,182,110,110},{1100,341,110,110}};

void renderPlayerCard(SDL_Renderer *renderer, TTF_Font *font, const Joueur *j, int numero, SDL_Rect rect){
    char buffer[64];
    SDL_Color blanc = {235,221,192,255};

    int lineH = TTF_FontHeight(font);
    int totalH = lineH*3;
    int baseY = rect.y + (rect.h-totalH)/2;

    snprintf(buffer,sizeof(buffer), "Etudiant %d",numero);
    printText(renderer, font, buffer, blanc,rect.x+4,baseY);

    snprintf(buffer,sizeof(buffer), "CR : %d",j->credits);
    printText(renderer, font, buffer, blanc,rect.x+55,baseY+lineH*1.4);

    snprintf(buffer,sizeof(buffer), "Eco : %1.f",j->noteEco);
    printText(renderer, font, buffer, blanc,rect.x+55,baseY+lineH*2.4);

}