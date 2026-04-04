#include "counter.h"
#include "button.h"
#include "text.h"

#include <stdio.h>

void initCounter(Counter* c, int min, int max, int startValue,int xPlus, int yPlus, int xMoins, int yMoins,int w, int h, int xText, int yText){
    c->value = startValue;
    c->min   = min;
    c->max   = max;
    c->label = (Text){
        .refX    = xText,
        .refY    = yText,
        .refSize = 38,
        .x       = xText,
        .y       = yText,
        .size    = 38,
        .color   = {235, 221, 192, 255}
    };
    c->btnPlus.ref  = (SDL_Rect){xPlus,  yPlus,  w, h};
    c->btnPlus.rect = c->btnPlus.ref;
    c->btnMoins.ref  = (SDL_Rect){xMoins, yMoins, w, h};
    c->btnMoins.rect = c->btnMoins.ref;
}

void handleCounter(Counter* c, int mouseX, int mouseY) {
    if (isClicked(&c->btnPlus,  mouseX, mouseY) && c->value < c->max) c->value++;
    if (isClicked(&c->btnMoins, mouseX, mouseY) && c->value > c->min) c->value--;
}

void renderCounter(SDL_Renderer* renderer, Counter* c, TTF_Font* font, int xText, int yText) {
    (void)xText; (void)yText; 
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", c->value);
    printText(renderer, font, buf, c->label.color, c->label.x, c->label.y);
}