#ifndef ANIM
#define ANIM

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    const char* text;
    int refX, refY;     
    int refSize;        
    int x, y;          
    int size;           
    SDL_Color color;
} Text;

typedef struct {
    SDL_Rect rect;      
    SDL_Rect ref;       
} Button;

bool isClicked(Button* btn, int x, int y);
void handleEvents();
void render();


#endif