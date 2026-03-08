#include "view.h"

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



int main(int argc, char* args[]){

    /* Test lancement */
    if(SDL_Init(SDL_INIT_VIDEO<0) || !(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)){
        printf("Erreur SDL : %s\n", SDL_GetError());
        return -1;
    }

    EtatJeu currentState=MENU;
    bool quit=false;
    SDL_Event event; 

    /*Font*/
    char* font = "FONT/fontPixel.ttf";

    /*Lancement de la fenêtre*/
    SDL_Window* window= SDL_CreateWindow("INSAPOLY",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280,720,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer * renderer= SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    /*Chargement des textures*/
    SDL_Texture* texMenu=IMG_LoadTexture(renderer,"IMAGE/Menu_principal.png");
    SDL_Texture* texPlateau=IMG_LoadTexture(renderer,"IMAGE/Plateau.png");
    SDL_Texture* texLancer=IMG_LoadTexture(renderer,"IMAGE/Lance_de.png");
    SDL_Texture* texPropriete=IMG_LoadTexture(renderer,"IMAGE/Menu_interne.png");
    SDL_Texture* texTest=IMG_LoadTexture(renderer,"IMAGE/TEST.png");

    if(!texMenu || !texPlateau || !texLancer || !texPropriete || !texTest){
        printf("Image loading error");
        return -1;
    }

    /*Boutons*/
    /*Menu Principal*/
    SDL_Rect buttonNewGame={457,255,370,67};
    SDL_Rect buttonLoadGame={457,337,370,67};
    SDL_Rect buttonSave={457,417,370,67};
    SDL_Rect buttonRanking={457,498,370,67};

    /*Plateau*/
    SDL_Rect buttonBackToMenu={1233,2,40,40};
    SDL_Rect buttonThrow={1107,559,160,50};
    SDL_Rect buttonProperties={1107,615,160,44};

    /*Evenements*/
    while(!quit){
      
        
        while(SDL_PollEvent(&event)!=0){
            if(event.type==SDL_QUIT){
                quit=true;
            }

            if(event.type==SDL_MOUSEBUTTONDOWN){
                int x,y; 
                SDL_GetMouseState(&x, &y);
                printf("Clic souris : x=%d, y=%d \n",x,y);

                /*Menu*/
                if(x>=buttonNewGame.x && x<=(buttonNewGame.x + buttonNewGame.w) &&
                y>=buttonNewGame.y && y<=(buttonNewGame.y+buttonNewGame.h)){
                    currentState=PLATEAU;
                }

                if(x>=buttonLoadGame.x && x<=(buttonLoadGame.x + buttonLoadGame.w) &&
                y>=buttonLoadGame.y && y<=(buttonLoadGame.y+buttonLoadGame.h)){
                    currentState=TEST;
                }

                if(x>=buttonSave.x && x<=(buttonSave.x + buttonSave.w) &&
                y>=buttonSave.y && y<=(buttonSave.y+buttonSave.h)){
                    currentState=TEST;
                }

                if(x>=buttonRanking.x && x<=(buttonRanking.x + buttonRanking.w) &&
                y>=buttonRanking.y && y<=(buttonRanking.y+buttonRanking.h)){
                    currentState=TEST;
                }

                /*Plateau*/
                if(x>=buttonBackToMenu.x && x<=(buttonBackToMenu.x + buttonBackToMenu.w) &&
                y>=buttonBackToMenu.y && y<=(buttonBackToMenu.y+buttonBackToMenu.h)){
                    currentState=MENU;
                }

                if(x>=buttonThrow.x && x<=(buttonThrow.x + buttonThrow.w) &&
                y>=buttonThrow.y && y<=(buttonThrow.y+buttonThrow.h)){
                    currentState=LANCER;
                }

                if(x>=buttonProperties.x && x<=(buttonProperties.x + buttonProperties.w) &&
                y>=buttonProperties.y && y<=(buttonProperties.y+buttonProperties.h)){
                    currentState=PROPRIETE;
                }           

            }

            if(event.type==SDL_WINDOWEVENT){
                if(event.window.event==SDL_WINDOWEVENT_RESIZED || event.window.event==SDL_WINDOWEVENT_SIZE_CHANGED){
                    UpdateButtonPositionsScaled(event.window.data1, event.window.data2, &buttonNewGame, &buttonLoadGame, &buttonSave, &buttonRanking);
                }
            }

            if(event.type==SDL_KEYDOWN){
                if(event.key.keysym.sym==SDLK_ESCAPE){
                    currentState=MENU;
                }
            }

        }
        SDL_RenderClear(renderer);

        /*Affichage des textures*/

        if(currentState==MENU){
            SDL_RenderCopy(renderer,texMenu, NULL,NULL);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }

        if(currentState==PLATEAU || currentState==LANCER){
            SDL_RenderCopy(renderer,texPlateau, NULL,NULL);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 90);
            SDL_RenderFillRect(renderer, &buttonBackToMenu);
            SDL_RenderFillRect(renderer, &buttonProperties);
            SDL_RenderFillRect(renderer, &buttonThrow);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            
            if(currentState==LANCER){
                SDL_Rect fenetreLancer ={340,100,600,400};
                SDL_RenderCopy(renderer,texLancer, NULL,&fenetreLancer);
            }

        }
            
        if(currentState==PROPRIETE){
            SDL_RenderCopy(renderer,texPropriete , NULL,NULL);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }

        if(currentState==TEST){
            SDL_RenderCopy(renderer,texTest, NULL,NULL);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }



        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texMenu);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
