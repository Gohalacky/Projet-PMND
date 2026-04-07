#include "button.h"

void ScaleButton(Button* btn, float ratioX, float ratioY){
    btn->rect.x = (int)(btn->ref.x * ratioX);
    btn->rect.y = (int)(btn->ref.y * ratioY);
    btn->rect.w = (int)(btn->ref.w * ratioX);
    btn->rect.h = (int)(btn->ref.h * ratioY);
}

bool isClicked(const Button* btn, int x, int y) {
    if(btn->disabled) return false;
    return x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
           y >= btn->rect.y && y <= btn->rect.y + btn->rect.h;
}

void renderButtonDisabled(SDL_Renderer* renderer, const Button* btn){
    if(!btn->disabled){
        return;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer,50,50,50,180);
    SDL_RenderFillRect(renderer, &btn->rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}