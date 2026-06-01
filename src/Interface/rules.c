#include "rules.h"
#include "ui/button.h"
#include "ui/text.h"

#include <stdio.h>
#include <string.h>

#define PANEL_X 120
#define PANEL_Y 50
#define PANEL_W 1040
#define PANEL_H 610

#define TEXT_X (PANEL_X + 40)
#define TEXT_Y (PANEL_Y + 80)
#define LINE_H 28

#define BTN_BACK_W 160
#define BTN_BACK_H 46
#define BTN_BACK_X (PANEL_X + (PANEL_W - BTN_BACK_W) / 2)
#define BTN_BACK_Y (PANEL_Y + PANEL_H - 58)


static float g_rx = 1.0f;
static float g_ry = 1.0f;

static void fillRect(SDL_Renderer* r, const SDL_Rect* rect, SDL_Color c) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(r, rect);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

static void outlineRect(SDL_Renderer* r, const SDL_Rect* rect, SDL_Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    SDL_RenderDrawRect(r, rect);
}

static SDL_Rect scaledRect(int refX, int refY, int refW, int refH) {
    return (SDL_Rect){
        (int)(refX * g_rx),
        (int)(refY * g_ry),
        (int)(refW * g_rx),
        (int)(refH * g_ry)
    };
}

static void drawBtn(SDL_Renderer* r, TTF_Font* font, const SDL_Rect* rect,const char* label, SDL_Color bg, SDL_Color fg, SDL_Color border) {
    fillRect(r, rect, bg);
    outlineRect(r, rect, border);
    if (font && label) {
        int tw, th;
        TTF_SizeUTF8(font, label, &tw, &th);
        printText(r, font, label, fg,rect->x + (rect->w - tw) / 2,rect->y + (rect->h - th) / 2);
    }
}

void initRulesMenu(RulesMenuState* rm) {
    memset(rm, 0, sizeof(*rm));
    rm->scroll = 0;

    FILE* f = fopen("data/rules.txt", "r");
    if (f) {
        char buf[256];
        while (rm->count < RULES_MAX_LINES && fgets(buf, sizeof(buf), f)) {
            int len = (int)strlen(buf);
            while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r')) {
                buf[--len] = '\0';
            }
            strncpy(rm->lines[rm->count], buf, 255);
            rm->lines[rm->count][255] = '\0';
            rm->count++;
        }
        fclose(f);
    }

    rm->btnBack = (Button){ .ref = { BTN_BACK_X, BTN_BACK_Y, BTN_BACK_W, BTN_BACK_H } };
    ScaleButton(&rm->btnBack, g_rx, g_ry);
}

void scaleRulesMenu(RulesMenuState* rm, float rx, float ry) {
    g_rx = rx;
    g_ry = ry;
    ScaleButton(&rm->btnBack, rx, ry);
}

void handleRulesMenuScroll(RulesMenuState* rm, int delta) {
    if (delta > 0) {
        if (rm->scroll > 0) rm->scroll--;
    } else if (delta < 0) {
        if (rm->scroll + RULES_VISIBLE < rm->count) rm->scroll++;
    }
}

void handleRulesMenuClick(RulesMenuState* rm, int x, int y, EtatJeu* state) {
    if (isClicked(&rm->btnBack, x, y)) {
        *state = MENU;
        return;
    }
}

void renderRulesMenu(SDL_Renderer* renderer,TTF_Font* fontTitle,TTF_Font* fontNormal,TTF_Font* fontBtn,const RulesMenuState* rm,const TexturesPack* tex) {
    (void)fontBtn;
    if (tex && tex->launchGame)
        SDL_RenderCopy(renderer, tex->launchGame, NULL, NULL);

    SDL_Color blanc    = {235, 221, 192, 255};
    SDL_Color or_clair = {180, 150, 100, 255};
    SDL_Color panel_bg = { 20,  14,   8, 220};
    SDL_Color row_bg   = { 42,  33,  22, 200};
    SDL_Color texte_c  = {235, 221, 192, 255};

    SDL_Rect panel = scaledRect(PANEL_X, PANEL_Y, PANEL_W, PANEL_H);
    fillRect(renderer, &panel, panel_bg);
    outlineRect(renderer, &panel, or_clair);

    if (fontTitle) {
        const char* titre = "REGLEMENT DU JEU";
        int tw, th;
        TTF_SizeUTF8(fontTitle, titre, &tw, &th);
        int winW;
        SDL_GetRendererOutputSize(renderer, &winW, NULL);
        printText(renderer, fontTitle, titre, blanc,panel.x + (panel.w - tw) / 2,panel.y + (int)(16 * g_ry));
    }

    int sepY = panel.y + (int)(64 * g_ry);
    SDL_SetRenderDrawColor(renderer, or_clair.r, or_clair.g, or_clair.b, or_clair.a);
    SDL_RenderDrawLine(renderer, panel.x + 20, sepY, panel.x + panel.w - 20, sepY);

    if (fontNormal) {
        int lineH = (int)(LINE_H * g_ry);
        int end   = rm->scroll + RULES_VISIBLE;
        if (end > rm->count) end = rm->count;

        for (int i = rm->scroll; i < end; i++) {
            int vis = i - rm->scroll;
            int ty  = (int)(TEXT_Y * g_ry) + vis * lineH;
            int tx  = (int)(TEXT_X * g_rx);
            printText(renderer, fontNormal, rm->lines[i], texte_c, tx, ty);
        }
    }

    drawBtn(renderer, fontNormal, &rm->btnBack.rect, "< Retour",row_bg, blanc, or_clair);
}
