#include "button.h"

void ScaleButton(Button* btn, float ratioX, float ratioY){
    btn->rect.x = (int)(btn->ref.x * ratioX);
    btn->rect.y = (int)(btn->ref.y * ratioY);
    btn->rect.w = (int)(btn->ref.w * ratioX);
    btn->rect.h = (int)(btn->ref.h * ratioY);
}

bool isClicked(const Button* btn, int x, int y) {
    return x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
           y >= btn->rect.y && y <= btn->rect.y + btn->rect.h;
}