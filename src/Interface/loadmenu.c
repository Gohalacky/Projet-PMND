#include "loadmenu.h"
#include "pawn.h"
#include "ui/text.h"
#include "ui/button.h"

#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_render.h>


#define PANEL_X      140
#define PANEL_Y       80
#define PANEL_W     1000
#define PANEL_H      570

#define LIST_X       (PANEL_X + 30)
#define LIST_Y       (PANEL_Y + 90)
#define ROW_H         66
#define ROW_LABEL_W  580
#define ROW_LABEL_H   50

#define BTN_LOAD_W   120
#define BTN_DEL_W    130
#define BTN_ROW_H     42
#define BTN_GAP       10

#define BTN_LOAD_X  (LIST_X + ROW_LABEL_W + 18)
#define BTN_DEL_X   (BTN_LOAD_X + BTN_LOAD_W + BTN_GAP)

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

static void drawBtn(SDL_Renderer* r, TTF_Font* font,const SDL_Rect* rect, const char* label,SDL_Color bg, SDL_Color fg, SDL_Color border) {
    fillRect(r, rect, bg);
    outlineRect(r, rect, border);
    if (font && label) {
        int tw, th;
        TTF_SizeUTF8(font, label, &tw, &th);
        printText(r, font, label, fg,rect->x + (rect->w - tw) / 2,rect->y + (rect->h - th) / 2);
    }
}

static SDL_Rect scaledRect(int refX, int refY, int refW, int refH) {
    return (SDL_Rect){
        (int)(refX * g_rx),
        (int)(refY * g_ry),
        (int)(refW * g_rx),
        (int)(refH * g_ry)
    };
}

static int rowRefY(int vis) {
    return LIST_Y + vis * ROW_H;
}

static int hitRef(int x, int y, int refX, int refY, int refW, int refH) {
    int sx = (int)(refX * g_rx), sy = (int)(refY * g_ry);
    int sw = (int)(refW * g_rx), sh = (int)(refH * g_ry);
    return x >= sx && x <= sx + sw && y >= sy && y <= sy + sh;
}


void initLoadMenu(LoadMenuState* lm) {
    memset(lm, 0, sizeof(*lm));
    lm->selected = -1;
    lm->scroll = 0;
    lm->count = savegame_list(lm->saves);

    lm->btnBack = (Button){ .ref = { BTN_BACK_X, BTN_BACK_Y, BTN_BACK_W, BTN_BACK_H } };
    ScaleButton(&lm->btnBack, g_rx, g_ry);
}

void scaleLoadMenu(LoadMenuState* lm, float rx, float ry) {
    g_rx = rx;
    g_ry = ry;
    ScaleButton(&lm->btnBack, rx, ry);
}

void handleLoadMenuScroll(LoadMenuState* lm, int delta) {
    if (delta > 0) {
        if (lm->scroll > 0) lm->scroll--;
    } else if (delta < 0) {
        if (lm->scroll + LOAD_MAX_VISIBLE < lm->count) lm->scroll++;
    }
}

void handleLoadMenuClick(LoadMenuState* lm, int x, int y,EtatJeu* state,EtatPartie* partie, ContexteJeu* ctx,Pawn pawns[MAX_PLAYERS]) {
    if (isClicked(&lm->btnBack, x, y)) {
        *state = MENU;
        return;
    }

    int end = lm->scroll + LOAD_MAX_VISIBLE;
    if (end > lm->count) end = lm->count;

    for (int i = lm->scroll; i < end; i++) {
        int vis = i - lm->scroll;
        int ry_ref = rowRefY(vis);
        int by_ref = ry_ref + (ROW_LABEL_H - BTN_ROW_H) / 2;

        if (hitRef(x, y, BTN_LOAD_X, by_ref, BTN_LOAD_W, BTN_ROW_H)) {
            lm->selected = i;
            if (savegame_load_file(lm->saves[i].filename, partie, ctx) == 0) {
                initPawns(pawns, partie->nbJoueurs);
                for (int p = 0; p < partie->nbJoueurs; p++) {
                    if (ctx->joueurs[p])
                        snapPawnToCase(&pawns[p], ctx->joueurs[p]->position, pawns, partie->nbJoueurs);
                }
                switch (partie->nbJoueurs) {
                    case 2: *state = PLATEAU2J; break;
                    case 3: *state = PLATEAU3J; break;
                    case 4: *state = PLATEAU4J; break;
                    default: *state = PLATEAU2J; break;
                }
            }
            return;
        }

        if (hitRef(x, y, BTN_DEL_X, by_ref, BTN_DEL_W, BTN_ROW_H)) {
            savegame_delete(lm->saves[i].filename);
            int old = lm->selected;
            initLoadMenu(lm);
            lm->selected = (old >= lm->count) ? lm->count - 1 : old;
            return;
        }

        if (hitRef(x, y, LIST_X, ry_ref, ROW_LABEL_W, ROW_LABEL_H)) {
            lm->selected = i;
            return;
        }
    }
}


void renderLoadMenu(SDL_Renderer* renderer,TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontBtn,const LoadMenuState* lm,const TexturesPack* tex) {
    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    SDL_Color blanc    = {235, 221, 192, 255};
    SDL_Color or_clair = {180, 150, 100, 255};
    SDL_Color marron   = { 80,  60,  40, 255};
    SDL_Color vert     = { 55, 120,  65, 230};
    SDL_Color rouge    = {155,  50,  50, 230};
    SDL_Color row_bg   = { 42,  33,  22, 210};
    SDL_Color row_sel  = { 80,  62,  38, 235};
    SDL_Color panel_bg = { 30,  22,  12, 225};

    if (tex && tex->launchGame)
        SDL_RenderCopy(renderer, tex->launchGame, NULL, NULL);

    SDL_Rect panel = scaledRect(PANEL_X, PANEL_Y, PANEL_W, PANEL_H);
    fillRect(renderer, &panel, panel_bg);
    outlineRect(renderer, &panel, or_clair);

    if (fontTitle) {
        const char* t = "CHARGER UNE PARTIE";
        int tw, th;
        TTF_SizeUTF8(fontTitle, t, &tw, &th);
        printText(renderer, fontTitle, t, blanc,panel.x + (panel.w - tw) / 2,panel.y + (int)(18 * g_ry));
    }

    int sepY = panel.y + (int)(72 * g_ry);
    SDL_SetRenderDrawColor(renderer, or_clair.r, or_clair.g, or_clair.b, or_clair.a);
    SDL_RenderDrawLine(renderer, panel.x + 20, sepY, panel.x + panel.w - 20, sepY);

    if (lm->count == 0 && fontNormal) {
        SDL_Color g2 = {160, 148, 128, 200};
        const char* msg = "Aucune sauvegarde disponible";
        int tw, th;
        TTF_SizeUTF8(fontNormal, msg, &tw, &th);
        printText(renderer, fontNormal, msg, g2,panel.x + (panel.w - tw) / 2,panel.y + panel.h / 2 - th / 2);
    }

    int end = lm->scroll + LOAD_MAX_VISIBLE;
    if (end > lm->count) end = lm->count;

    for (int i = lm->scroll; i < end; i++) {
        int vis  = i - lm->scroll;
        int ry_r = rowRefY(vis);
        int by_r = ry_r + (ROW_LABEL_H - BTN_ROW_H) / 2;

        SDL_Rect rowRect = scaledRect(LIST_X, ry_r, ROW_LABEL_W, ROW_LABEL_H);
        SDL_Color bg = (i == lm->selected) ? row_sel : row_bg;
        fillRect(renderer, &rowRect, bg);
        outlineRect(renderer, &rowRect, or_clair);

        if (fontBtn) {
            printText(renderer, fontBtn, lm->saves[i].label, blanc,
                      rowRect.x + (int)(10 * g_rx),
                      rowRect.y + (rowRect.h - TTF_FontHeight(fontBtn)) / 2);
        }

        SDL_Rect loadRect = scaledRect(BTN_LOAD_X, by_r, BTN_LOAD_W, BTN_ROW_H);
        drawBtn(renderer, fontBtn, &loadRect, "Charger", vert, blanc, marron);

        SDL_Rect delRect = scaledRect(BTN_DEL_X, by_r, BTN_DEL_W, BTN_ROW_H);
        drawBtn(renderer, fontBtn, &delRect, "Supprimer", rouge, blanc, marron);
    }

    drawBtn(renderer, fontNormal, &lm->btnBack.rect, "< Retour",row_bg, blanc, or_clair);
}
