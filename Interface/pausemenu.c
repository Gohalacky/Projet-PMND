#include "pausemenu.h"
#include "ui/text.h"
#include "ui/button.h"

#define BTN_PAUSE_X   1220
#define BTN_PAUSE_Y      5
#define BTN_PAUSE_W     38
#define BTN_PAUSE_H     38


#define MENU_W  380
#define MENU_H  220
#define MENU_X  ((1280 - MENU_W) / 2)
#define MENU_Y  ((720  - MENU_H) / 2)


#define BTN_W   300
#define BTN_H    50
#define BTN_MARGIN 20

void initPauseButtons(ButtonsPack* btn) {

    btn->pauseMenu = (Button){ .ref = {BTN_PAUSE_X, BTN_PAUSE_Y, BTN_PAUSE_W, BTN_PAUSE_H} };

    btn->pauseResume = (Button){
        .ref = {
            MENU_X + (MENU_W - BTN_W) / 2,
            MENU_Y + 60,
            BTN_W, BTN_H
        }
    };

    btn->pauseSaveQuit = (Button){
        .ref = {
            MENU_X + (MENU_W - BTN_W) / 2,
            MENU_Y + 60 + BTN_H + BTN_MARGIN,
            BTN_W, BTN_H
        }
    };

    scalePauseButtons(btn, 1.0f, 1.0f);
}

void scalePauseButtons(ButtonsPack* btn, float ratioX, float ratioY) {
    ScaleButton(&btn->pauseMenu,     ratioX, ratioY);
    ScaleButton(&btn->pauseResume,   ratioX, ratioY);
    ScaleButton(&btn->pauseSaveQuit, ratioX, ratioY);
}


static void drawRoundedRect(SDL_Renderer* r, const SDL_Rect* rect,
                            SDL_Color fill, SDL_Color border) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(r, rect);
    SDL_SetRenderDrawColor(r, border.r, border.g, border.b, border.a);
    SDL_RenderDrawRect(r, rect);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

static void drawMenuButton(SDL_Renderer* renderer, TTF_Font* font,
                           const Button* btn, const char* label,
                           SDL_Color bg, SDL_Color fg) {
    SDL_Color border = {60, 45, 35, 255};
    drawRoundedRect(renderer, &btn->rect, bg, border);

    if (font && label) {
        int tw, th;
        TTF_SizeUTF8(font, label, &tw, &th);
        printText(renderer, font, label, fg,
                  btn->rect.x + (btn->rect.w - tw) / 2,
                  btn->rect.y + (btn->rect.h - th) / 2);
    }
}

void renderPauseButton(SDL_Renderer* renderer, TTF_Font* font,const ButtonsPack* btn) {
    SDL_Color bg     = {50,  40,  30, 210};
    SDL_Color border = {140, 120, 90, 255};
    drawRoundedRect(renderer, &btn->pauseMenu.rect, bg, border);

    SDL_SetRenderDrawColor(renderer, 235, 221, 192, 255);
    int bx = btn->pauseMenu.rect.x;
    int by = btn->pauseMenu.rect.y;
    int bw = btn->pauseMenu.rect.w;
    int bh = btn->pauseMenu.rect.h;

    int lw = (int)(bw * 0.6f);   
    int lx = bx + (bw - lw) / 2;
    int th = (int)(bh * 0.08f);  
    if (th < 2) th = 2;

    for (int i = 0; i < 3; i++) {
        int ly = by + (int)(bh * (0.25f + i * 0.25f)) - th / 2;
        SDL_Rect bar = {lx, ly, lw, th};
        SDL_RenderFillRect(renderer, &bar);
    }

    (void)font; 
}

void renderPauseMenu(SDL_Renderer* renderer, TTF_Font* font,const ButtonsPack* btn) {

    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_Rect overlay = {0, 0, winW, winH};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color panelBg     = {45,  35,  25, 240};
    SDL_Color panelBorder = {180, 150, 100, 255};
    SDL_Rect panel = {
        btn->pauseResume.rect.x - 40,
        btn->pauseResume.rect.y - 60,
        btn->pauseResume.rect.w + 80,
        btn->pauseSaveQuit.rect.y + btn->pauseSaveQuit.rect.h
            - btn->pauseResume.rect.y + 100
    };
    drawRoundedRect(renderer, &panel, panelBg, panelBorder);

    if (font) {
        SDL_Color titre = {235, 221, 192, 255};
        const char* label = "PAUSE";
        int tw, th;
        TTF_SizeUTF8(font, label, &tw, &th);
        printText(renderer, font, label, titre,
                  panel.x + (panel.w - tw) / 2,
                  panel.y + 14);
    }

    SDL_SetRenderDrawColor(renderer, 140, 120, 90, 200);
    SDL_RenderDrawLine(renderer,panel.x + 20, btn->pauseResume.rect.y - 12,panel.x + panel.w - 20, btn->pauseResume.rect.y - 12);

    SDL_Color bgResume  = {70,  130,  80, 230};
    SDL_Color fgBtn     = {235, 221, 192, 255};
    drawMenuButton(renderer, font, &btn->pauseResume, "Reprendre", bgResume, fgBtn);


    SDL_Color bgSave = {150, 60, 55, 230};
    drawMenuButton(renderer, font, &btn->pauseSaveQuit, "Sauvegarder & Quitter", bgSave, fgBtn);
}
