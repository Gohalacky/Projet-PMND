#include "assets.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

int loadTextures(SDL_Renderer* renderer, TexturesPack* tex){
    tex->menu                   = IMG_LoadTexture(renderer, "IMAGE/menu.png");
    tex->newGame                = IMG_LoadTexture(renderer, "IMAGE/launchGame.png");
    tex->launchGame             = IMG_LoadTexture(renderer, "IMAGE/chargeGame.png");
    tex->gameBoard2Player       = IMG_LoadTexture(renderer, "IMAGE/gameBoard2Player.png");
    tex->gameBoard3Player       = IMG_LoadTexture(renderer, "IMAGE/gameBoard3Player.png");
    tex->gameBoard4Player       = IMG_LoadTexture(renderer, "IMAGE/gameBoard4Player.png");
    tex->throws                 = IMG_LoadTexture(renderer, "IMAGE/throws.png");
    tex->menuProperties         = IMG_LoadTexture(renderer, "IMAGE/Menu_interne.png");
    tex->test                   = IMG_LoadTexture(renderer, "IMAGE/TEST.png");

    tex->maison[0]   = IMG_LoadTexture(renderer, "IMAGE/Maison x1.png");
    tex->maison[1]   = IMG_LoadTexture(renderer, "IMAGE/Maison x2.png");
    tex->maison[2]   = IMG_LoadTexture(renderer, "IMAGE/Maison x3.png");
    tex->immeuble[0] = IMG_LoadTexture(renderer, "IMAGE/Immeuble x1.png");
    tex->immeuble[1] = IMG_LoadTexture(renderer, "IMAGE/Immeuble x2.png");
    tex->immeuble[2] = IMG_LoadTexture(renderer, "IMAGE/Immeuble x3.png");
    tex->parc[0]     = IMG_LoadTexture(renderer, "IMAGE/Parc x1.png");
    tex->parc[1]     = IMG_LoadTexture(renderer, "IMAGE/Parc x2.png");
    tex->parc[2]     = IMG_LoadTexture(renderer, "IMAGE/Parc x3.png");
    tex->fondProp    = IMG_LoadTexture(renderer, "IMAGE/Fond_prop.png");

    tex->pion[0]=IMG_LoadTexture(renderer, "IMAGE/pion1.png");
    tex->pion[1]=IMG_LoadTexture(renderer, "IMAGE/pion2.png");
    tex->pion[2]=IMG_LoadTexture(renderer, "IMAGE/pion3.png");
    tex->pion[3]=IMG_LoadTexture(renderer, "IMAGE/pion4.png");

    SDL_Texture* all[] = {
        tex->menu, tex->newGame, tex->launchGame,
        tex->gameBoard2Player, tex->gameBoard3Player, tex->gameBoard4Player,
        tex->throws, tex->menuProperties, tex->test,tex->maison[0],   tex->maison[1],   tex->maison[2],
        tex->immeuble[0], tex->immeuble[1], tex->immeuble[2],
        tex->parc[0],     tex->parc[1],     tex->parc[2],
        tex->fondProp, tex->pion[0],tex->pion[1],tex->pion[2],tex->pion[3],
    };
    int n = sizeof(all)/sizeof(all[0]);
    for (int i = 0; i < n; i++) {
        if (!all[i]) {
            printf("Erreur chargement texture %d : %s\n", i, IMG_GetError());
            return -1;
        }
    }
    return 0;
}

static void safeDestroy(SDL_Texture** t){
    if(*t){
        SDL_DestroyTexture(*t);
        *t=NULL;
    }
}

void freeTextures(TexturesPack* tex){
    safeDestroy(&tex->menu);
    safeDestroy(&tex->newGame);
    safeDestroy(&tex->launchGame);
    safeDestroy(&tex->gameBoard2Player);
    safeDestroy(&tex->gameBoard3Player);
    safeDestroy(&tex->gameBoard4Player);
    safeDestroy(&tex->throws);
    safeDestroy(&tex->menuProperties);
    safeDestroy(&tex->test);
    for (int i = 0; i < 3; i++) {
        safeDestroy(&tex->maison[i]);
        safeDestroy(&tex->immeuble[i]);
        safeDestroy(&tex->parc[i]);
    }
    for(int i=0;i<4;i++){
        safeDestroy(&tex->pion[i]);
    }
    safeDestroy(&tex->fondProp);
    IMG_Quit();
}