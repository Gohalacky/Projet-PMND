#ifndef UI_TEXTBOX_H
#define UI_TEXTBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../Struct.h"

typedef struct {
    TextBox initialMoney;   
    TextBox numbTurns;      
    TextBox eventFreq;      
} TextBoxPack;

void initTextBox(TextBox* tb, int refX, int refY, int refW, int refH);
void ScaleTextBox(TextBox* tb, float ratioX, float ratioY);
void ScaleAllTextBoxes(TextBoxPack* tboxes, float ratioX, float ratioY);

void handleTextBoxClick(TextBoxPack* tboxes, int mouseX, int mouseY);
void handleTextBoxEvent(SDL_Event* event, TextBox* tb);
void renderTextBox(SDL_Renderer* renderer, TTF_Font* font, TextBox* tb);

#endif