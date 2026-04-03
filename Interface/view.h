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

#include "Struct.h"

#define REF_W 1280.0f
#define REF_H 720.0f

#define REF_BTN_W 370.0f
#define REF_BTN_H 67.0f

typedef struct {
    TextBox initialMoney;   
    TextBox numbTurns;      
    TextBox eventFreq;      
} TextBoxPack;

typedef struct{
    SDL_Texture* menu;
    SDL_Texture* newGame;
    SDL_Texture* launchGame;
    SDL_Texture* gameBoard2Player;
    SDL_Texture* gameBoard3Player;
    SDL_Texture* gameBoard4Player;
    SDL_Texture* throws; 
    SDL_Texture* menuProperties;
    SDL_Texture* test;
} TexturesPack;

typedef struct{
    /*Main Menu*/
    Button newGame, loadGame, save;
    /*Game Board*/
    Button backToMenu, throwDice, properties;
    /*New Game*/
    Button launchGame, cancel, validMoney;
} ButtonsPack;

typedef struct {
    /*Menu*/
    Text menuNewGame;
    Text menuLoadGame;
    Text menuSave;
    /*New Game*/
    Text titleNewGame;
    Text numberPlayer;
    Text chooseParam;
    Text btnLaunchGame;
    Text btnCancel;
    Text btnValid;
    Text initialMoney;
    Text tillBankruptcy;
    Text numbTurns;
    Text event;

    /*Game Board*/
    Text btnThrowDice;
    Text btnProperties;
    Text btnEndTurn;
} TextsPack;


/* TEXTURES */
int  loadTextures(SDL_Renderer* renderer, TexturesPack* tex);

/* RENDU */
void render(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex,
            TextsPack* texts, TTF_Font* fontButton, TTF_Font* fontNormal, TTF_Font* fontTitle,
            Counter* counterJoueurs, TextBoxPack* tboxes);

/* NETTOYAGE */
void cleanup(SDL_Window* win, SDL_Renderer* rdr, TexturesPack* tex);

/* EVENMENTS */
bool isClicked(Button* btn, int x, int y);
void handleEvents(SDL_Event* event, EtatJeu* state, ButtonsPack* btn, TextsPack* texts,
                  TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton,
                  bool* quit, Counter* counterJoueurs, TextBoxPack* tboxes);

/* TEXTES */
void printText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y);
void printTextCentered(SDL_Renderer* renderer, TTF_Font* font,const char* text, SDL_Color color, int y, int winW);
void printTextResponsive(SDL_Renderer* renderer, TTF_Font* font, const Text* t);


void ScaleText(Text* t, float ratioX, float ratioY);
void scaleAllTexts(Text** texts, int count, float ratioX, float ratioY);
void ScaleFonts(TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton, float ratioY);


/* BOUTONS */
void ScaleButton(Button* btn, float ratioX, float ratioY);

/* COMPTEURS */
void initCounter(Counter* c, int min, int max, int startValue,
                 int xPlus, int yPlus, int xMoins, int yMoins,
                 int w, int h, int xText, int yText); 
void handleCounter(Counter* c, int mouseX, int mouseY);
void renderCounter(SDL_Renderer* renderer, Counter* c, TTF_Font* font, int xText, int yText) ;

/* TEXTBOX */
void initTextBox(TextBox* tb, int refX, int refY, int refW, int refH);
void ScaleTextBox(TextBox* tb, float ratioX, float ratioY);
void handleTextBoxEvent(SDL_Event* event, TextBox* tb);
void handleTextBoxClick(TextBoxPack* tboxes, int mouseX, int mouseY);
void renderTextBox(SDL_Renderer* renderer, TTF_Font* font, TextBox* tb);
void ScaleAllTextBoxes(TextBoxPack* tboxes, float ratioX, float ratioY);

#endif
