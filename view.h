#ifndef VIEW
#define VIEW

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define REF_W 1280.0f
#define REF_H 720.0f

#define REF_BTN_W 370.0f
#define REF_BTN_H 67.0f

typedef struct {
    SDL_Rect rect;      
    SDL_Rect ref;       
} Button;

typedef struct{
    SDL_Texture* menu;
    SDL_Texture* newGame;
    SDL_Texture* launchGame;
    SDL_Texture* gameBoard;
    SDL_Texture* throws; 
    SDL_Texture* menuProperties;
    SDL_Texture* test;
} TexturesPack;

typedef struct{
    /*Main Menu*/
    Button newGame, loadGame, save, ranking;
    /*Game Board*/
    Button backToMenu, throwDice, properties;
    /*New Game*/
    Button launchGame, cancel;
} ButtonsPack;

typedef struct {
    const char* text;
    int refX, refY;     
    int refSize;        
    int x, y;          
    int size;           
    SDL_Color color;
} Text;

typedef struct {
    /*New Game*/
    Text titleNewGame;
    Text btnLaunchGame;
    Text btnCancel;
    /*Game Board*/
    Text btnThrowDice;
    Text btnProperties;
} TextsPack;


typedef enum {MENU,NOUVPARTIE,CHARGERPARTIE,PLATEAU,LANCER,PROPRIETE, TEST} EtatJeu;


int loadTextures(SDL_Renderer* renderer, TexturesPack* tex);
bool isClicked(Button* btn, int x, int y);
void handleEvents(SDL_Event* event, EtatJeu* state, ButtonsPack* btn, TextsPack* texts, TTF_Font** fontNormal, TTF_Font** fontTitle, bool* quit);
void render(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex, ButtonsPack* btn, TextsPack* texts, TTF_Font* fontNormal, TTF_Font* fontTitle);
void cleanup(SDL_Window* win, SDL_Renderer* rdr, TexturesPack* tex);
void ScaleButton(Button* btn, float ratioX, float ratioY);
void updateTextPosition(int windowW, int windowH);
void printText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y);
void ScaleText(Text* t, float ratioX, float ratioY);
void ScaleFonts(TTF_Font** fontNormal, TTF_Font** fontTitle, float ratioY);


#endif