#include "checkbox.h"
#include "text.h"

void ScaleCheckbox(Checkbox* cb, float ratioX, float ratioY) {
    cb->rect.x = (int)(cb->ref.x * ratioX);
    cb->rect.y = (int)(cb->ref.y * ratioY);
    cb->rect.w = (int)(cb->ref.w * ratioX);
    cb->rect.h = (int)(cb->ref.h * ratioY);
}

bool isChecked(const Checkbox* cb, int x, int y) {
    return x >= cb->rect.x && x <= cb->rect.x + cb->rect.w &&
           y >= cb->rect.y && y <= cb->rect.y + cb->rect.h;
}

void toggleCheckbox(Checkbox* cb) {
    cb->checked = !cb->checked;
}

void renderCheckbox(SDL_Renderer* renderer, TTF_Font* font,
                    const Checkbox* cb, const char* label, SDL_Color color) {
    if (!renderer || !cb) return;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &cb->rect);

    if (cb->checked) {

        SDL_Rect inner = {
            cb->rect.x + 3,
            cb->rect.y + 3,
            cb->rect.w - 6,
            cb->rect.h - 6,
        };
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &inner);
    }

    if (font && label) {
        int labelX = cb->rect.x + cb->rect.w + 8;
        int labelY = cb->rect.y + (cb->rect.h - TTF_FontHeight(font)) / 2;
        printText(renderer, font, label, color, labelX, labelY);
    }
}