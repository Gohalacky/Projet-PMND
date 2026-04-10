#include "textbox.h"

#include <string.h>
#include <stdio.h>

void initTextBox(TextBox* tb, int refX, int refY, int refW, int refH) {
    memset(tb->text, 0, sizeof(tb->text));
    tb->cursor_pos = 0;
    tb->active     = 0;
    tb->ref  = (SDL_Rect){refX, refY, refW, refH};
    tb->rect = tb->ref;
}

void ScaleTextBox(TextBox* tb, float ratioX, float ratioY) {
    tb->rect.x = (int)(tb->ref.x * ratioX);
    tb->rect.y = (int)(tb->ref.y * ratioY);
    tb->rect.w = (int)(tb->ref.w * ratioX);
    tb->rect.h = (int)(tb->ref.h * ratioY);
}

void ScaleAllTextBoxes(TextBoxPack* tboxes, float ratioX, float ratioY) {
    ScaleTextBox(&tboxes->initialMoney, ratioX, ratioY);
    ScaleTextBox(&tboxes->numbTurns,    ratioX, ratioY);
    ScaleTextBox(&tboxes->eventFreq,    ratioX, ratioY);
}

static int isInTextBox(TextBox* tb, int x, int y) {
    return x >= tb->rect.x && x <= tb->rect.x + tb->rect.w && y >= tb->rect.y && y <= tb->rect.y + tb->rect.h;
}

void handleTextBoxClick(TextBoxPack* tboxes, int mouseX, int mouseY) {
    TextBox* all[] = { &tboxes->initialMoney, &tboxes->numbTurns, &tboxes->eventFreq };
    for (int i = 0; i < 3; i++) {
        if (isInTextBox(all[i], mouseX, mouseY)) {
            all[i]->active = 1;
            SDL_StartTextInput();
        } else {
            all[i]->active = 0;
        }
    }
    if (!tboxes->initialMoney.active &&
        !tboxes->numbTurns.active    &&
        !tboxes->eventFreq.active)
        SDL_StopTextInput();
}

void handleTextBoxEvent(SDL_Event* event, TextBox* tb) {
    if (!tb->active) return;

    if (event->type == SDL_TEXTINPUT) {
        
        int len = (int)strlen(tb->text);
        int addLen = (int)strlen(event->text.text);
        if (len + addLen >= TEXTBOX_MAX_LEN) return;

        
        memmove(&tb->text[tb->cursor_pos + addLen],
                &tb->text[tb->cursor_pos],
                len - tb->cursor_pos + 1);
        memcpy(&tb->text[tb->cursor_pos], event->text.text, addLen);
        tb->cursor_pos += addLen;
    }

    if (event->type == SDL_KEYDOWN) {
        int len = (int)strlen(tb->text);
        switch (event->key.keysym.sym) {

            case SDLK_BACKSPACE:
                if (tb->cursor_pos > 0) {
                    memmove(&tb->text[tb->cursor_pos - 1],
                            &tb->text[tb->cursor_pos],
                            len - tb->cursor_pos + 1);
                    tb->cursor_pos--;
                }
                break;

            case SDLK_DELETE:
                if (tb->cursor_pos < len)
                    memmove(&tb->text[tb->cursor_pos],
                            &tb->text[tb->cursor_pos + 1],
                            len - tb->cursor_pos);
                break;

            case SDLK_LEFT:
                if (tb->cursor_pos > 0) tb->cursor_pos--;
                break;

            case SDLK_RIGHT:
                if (tb->cursor_pos < len) tb->cursor_pos++;
                break;

            case SDLK_HOME:
                tb->cursor_pos = 0;
                break;

            case SDLK_END:
                tb->cursor_pos = len;
                break;

            default: break;
        }
    }
}

void renderTextBox(SDL_Renderer* renderer, TTF_Font* font, TextBox* tb) {
    SDL_SetRenderDrawColor(renderer, 245, 240, 225, 255);
    SDL_RenderFillRect(renderer, &tb->rect);


    if (tb->active)
        SDL_SetRenderDrawColor(renderer, 180, 140, 60, 255);
    else
        SDL_SetRenderDrawColor(renderer, 120, 110, 90, 255);
    SDL_RenderDrawRect(renderer, &tb->rect);

    if (font && strlen(tb->text) > 0) {
        SDL_Color color = {40, 30, 20, 255};
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, tb->text, color);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            if (tex) {
                int pad = 6;
                SDL_Rect dst = {
                    tb->rect.x + pad,
                    tb->rect.y + (tb->rect.h - surf->h) / 2,
                    surf->w,
                    surf->h
                };
                if (dst.w > tb->rect.w - 2 * pad)
                    dst.w = tb->rect.w - 2 * pad;
                SDL_RenderCopy(renderer, tex, NULL, &dst);
                SDL_DestroyTexture(tex);
            }
            SDL_FreeSurface(surf);
        }
    }


    if (tb->active && (SDL_GetTicks() / 500) % 2 == 0) {
        int cursor_x = tb->rect.x + 6; 
        if (font && tb->cursor_pos > 0) {

            char tmp[TEXTBOX_MAX_LEN];
            strncpy(tmp, tb->text, tb->cursor_pos);
            tmp[tb->cursor_pos] = '\0';
            int w = 0;
            TTF_SizeUTF8(font, tmp, &w, NULL);
            cursor_x += w;
        }

        SDL_SetRenderDrawColor(renderer, 40, 30, 20, 255);
        SDL_RenderDrawLine(renderer,
                           cursor_x, tb->rect.y + 4,
                           cursor_x, tb->rect.y + tb->rect.h - 4);
    }
}