#include "assets.h"

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

    SDL_Texture* all[] = {
        tex->menu, tex->newGame, tex->launchGame,
        tex->gameBoard2Player, tex->gameBoard3Player, tex->gameBoard4Player,
        tex->throws, tex->menuProperties, tex->test,tex->maison[0],   tex->maison[1],   tex->maison[2],
        tex->immeuble[0], tex->immeuble[1], tex->immeuble[2],
        tex->parc[0],     tex->parc[1],     tex->parc[2],
        tex->fondProp,
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

void freeTextures(TexturesPack* tex){
    SDL_DestroyTexture(tex->menu);
    SDL_DestroyTexture(tex->newGame);
    SDL_DestroyTexture(tex->launchGame);
    SDL_DestroyTexture(tex->gameBoard2Player);
    SDL_DestroyTexture(tex->gameBoard3Player);
    SDL_DestroyTexture(tex->gameBoard4Player);
    SDL_DestroyTexture(tex->throws);
    SDL_DestroyTexture(tex->menuProperties);
    SDL_DestroyTexture(tex->test);
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(tex->maison[i]);
        SDL_DestroyTexture(tex->immeuble[i]);
        SDL_DestroyTexture(tex->parc[i]);
    }
    SDL_DestroyTexture(tex->fondProp);
    IMG_Quit();
}