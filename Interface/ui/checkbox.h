#ifndef UI_CHECKBOX_H
#define UI_CHECKBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "../Struct.h"

typedef struct {
    SDL_Rect rect;   
    SDL_Rect ref;    
    int      checked;
} Checkbox;

void      ScaleCheckbox(Checkbox* cb, float ratioX, float ratioY);
bool      isChecked(const Checkbox* cb, int x, int y);  
void      toggleCheckbox(Checkbox* cb);
void      renderCheckbox(SDL_Renderer* renderer, TTF_Font* font,const Checkbox* cb, const char* label, SDL_Color color);

#endif