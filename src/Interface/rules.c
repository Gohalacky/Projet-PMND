#include "rules.h"
#include "ui/button.h"
#include "ui/text.h"

#include <stdio.h>
#include <string.h>

#define PANEL_X   120
#define PANEL_Y    50
#define PANEL_W  1040
#define PANEL_H   610

#define TEXT_X   (PANEL_X + 40)
#define TEXT_Y   (PANEL_Y + 80)
#define LINE_H    28

#define BTN_SCR_W   40
#define BTN_SCR_H   40
#define BTN_SCR_X  (PANEL_X + PANEL_W - 55)
#define BTN_UP_Y   (TEXT_Y - 48)
#define BTN_DOWN_Y (TEXT_Y + RULES_VISIBLE * LINE_H + 8)

#define BTN_BACK_W  160
#define BTN_BACK_H   46
#define BTN_BACK_X  (PANEL_X + (PANEL_W - BTN_BACK_W) / 2)
#define BTN_BACK_Y  (PANEL_Y + PANEL_H - 58)


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

    /* Chargement du fichier rules.txt */
    FILE* f = fopen("data/rules.txt", "r");
    if (f) {
        char buf[256];
        while (rm->count < RULES_MAX_LINES && fgets(buf, sizeof(buf), f)) {
            /* Supprimer \r\n */
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

    rm->btnScrollUp   = (Button){ .ref = { BTN_SCR_X, BTN_UP_Y,   BTN_SCR_W, BTN_SCR_H } };
    rm->btnScrollDown = (Button){ .ref = { BTN_SCR_X, BTN_DOWN_Y, BTN_SCR_W, BTN_SCR_H } };
    rm->btnBack       = (Button){ .ref = { BTN_BACK_X, BTN_BACK_Y, BTN_BACK_W, BTN_BACK_H } };

    ScaleButton(&rm->btnScrollUp,   g_rx, g_ry);
    ScaleButton(&rm->btnScrollDown, g_rx, g_ry);
    ScaleButton(&rm->btnBack,       g_rx, g_ry);
}

void scaleRulesMenu(RulesMenuState* rm, float rx, float ry) {
    g_rx = rx;
    g_ry = ry;
    ScaleButton(&rm->btnScrollUp,   rx, ry);
    ScaleButton(&rm->btnScrollDown, rx, ry);
    ScaleButton(&rm->btnBack,       rx, ry);
}

void handleRulesMenuClick(RulesMenuState* rm, int x, int y, EtatJeu* state) {
    if (isClicked(&rm->btnBack, x, y)) {
        *state = MENU;
        return;
    }
    if (isClicked(&rm->btnScrollUp, x, y) && rm->scroll > 0) {
        rm->scroll--;
        return;
    }
    if (isClicked(&rm->btnScrollDown, x, y) &&
        rm->scroll + RULES_VISIBLE < rm->count) {
        rm->scroll++;
        return;
    }
}

void renderRulesMenu(SDL_Renderer* renderer,TTF_Font* fontTitle,TTF_Font* fontNormal,TTF_Font* fontBtn,const RulesMenuState* rm,const TexturesPack* tex) {

    if (tex && tex->launchGame)
        SDL_RenderCopy(renderer, tex->launchGame, NULL, NULL);

    /*  Couleurs*/
    SDL_Color blanc    = {235, 221, 192, 255};
    SDL_Color or_clair = {180, 150, 100, 255};
    SDL_Color panel_bg = { 20,  14,   8, 220};
    SDL_Color row_bg   = { 42,  33,  22, 200};
    SDL_Color arrow_on = {220, 190, 130, 255};
    SDL_Color arrow_off= { 80,  70,  60, 180};
    SDL_Color gris     = { 75,  70,  65, 200};
    SDL_Color texte_c  = {235, 221, 192, 255};

    /*Panneau principal*/
    SDL_Rect panel = scaledRect(PANEL_X, PANEL_Y, PANEL_W, PANEL_H);
    fillRect(renderer, &panel, panel_bg);
    outlineRect(renderer, &panel, or_clair);

    /* Titre */
    if (fontTitle) {
        const char* titre = "REGLEMENT DU JEU";
        int tw, th;
        TTF_SizeUTF8(fontTitle, titre, &tw, &th);
        int winW;
        SDL_GetRendererOutputSize(renderer, &winW, NULL);
        printText(renderer, fontTitle, titre, blanc,
                  panel.x + (panel.w - tw) / 2,
                  panel.y + (int)(16 * g_ry));
    }

    /* Séparateur */
    int sepY = panel.y + (int)(64 * g_ry);
    SDL_SetRenderDrawColor(renderer, or_clair.r, or_clair.g, or_clair.b, or_clair.a);
    SDL_RenderDrawLine(renderer, panel.x + 20, sepY, panel.x + panel.w - 20, sepY);

    /* Lignes de texte */
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

    /* Flèches de défilement */
    int canUp   = (rm->scroll > 0);
    int canDown = (rm->scroll + RULES_VISIBLE < rm->count);

    drawBtn(renderer, fontBtn, &rm->btnScrollUp.rect, "^",
            canUp   ? row_bg : gris,
            canUp   ? arrow_on : arrow_off, or_clair);
    drawBtn(renderer, fontBtn, &rm->btnScrollDown.rect, "v",
            canDown ? row_bg : gris,
            canDown ? arrow_on : arrow_off, or_clair);

    /* Indicateur de progression */
    if (fontBtn && rm->count > 0) {
        char buf[32];
        int shown_end = rm->scroll + RULES_VISIBLE;
        if (shown_end > rm->count) shown_end = rm->count;
        snprintf(buf, sizeof(buf), "%d-%d / %d",
                 rm->scroll + 1, shown_end, rm->count);
        SDL_Color gris3 = {155, 145, 125, 200};
        int tw, th;
        TTF_SizeUTF8(fontBtn, buf, &tw, &th);
        printText(renderer, fontBtn, buf, gris3,
                  rm->btnScrollUp.rect.x - tw - (int)(12 * g_rx),
                  rm->btnScrollUp.rect.y + (rm->btnScrollUp.rect.h - th) / 2);
    }

    /* Bouton Retour */
    drawBtn(renderer, fontNormal, &rm->btnBack.rect, "< Retour",
            row_bg, blanc, or_clair);
}