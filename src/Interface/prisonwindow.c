#include "prisonwindow.h"
#include "view.h"
#include "ui/button.h"
#include "ui/text.h"

#include <stdio.h>
#include <string.h>

#define PAN_X 390
#define PAN_Y 200
#define PAN_W 500
#define PAN_H 310

#define BTN_W 210
#define BTN_H 42
#define BTN_Y 420
#define BTN_LEFT_X 402
#define BTN_RIGHT_X 668

void initPrisonButtons(PrisonButtonsPack* pb) {
    pb->btnTenterDouble  = (Button){ .ref = {BTN_LEFT_X,  BTN_Y, BTN_W, BTN_H} };
    pb->btnUtiliserCarte = (Button){ .ref = {BTN_RIGHT_X, BTN_Y, BTN_W, BTN_H} };
    scalePrisonButtons(pb, 1.0f, 1.0f);
}

void scalePrisonButtons(PrisonButtonsPack* pb, float rx, float ry) {
    ScaleButton(&pb->btnTenterDouble,  rx, ry);
    ScaleButton(&pb->btnUtiliserCarte, rx, ry);
}

void renderPrisonWindow(SDL_Renderer* renderer, TTF_Font* fontTitle, TTF_Font* fontNormal,const ContexteJeu* ctx, const EtatPartie* partie, PrisonButtonsPack* pb, int winW, int winH) {

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_Rect full = {0, 0, winW, winH};
    SDL_RenderFillRect(renderer, &full);

    SDL_Rect panel = {
        (int)(PAN_X * winW / REF_W),
        (int)(PAN_Y * winH / REF_H),
        (int)(PAN_W * winW / REF_W),
        (int)(PAN_H * winH / REF_H),
    };
    SDL_SetRenderDrawColor(renderer, 30, 20, 40, 240);
    SDL_RenderFillRect(renderer, &panel);

    SDL_SetRenderDrawColor(renderer, 180, 40, 40, 255);
    for (int t = 0; t < 3; t++) {
        SDL_Rect b = {panel.x - t, panel.y - t, panel.w + 2*t, panel.h + 2*t};
        SDL_RenderDrawRect(renderer, &b);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_Color rouge  = {220,  60,  60, 255};
    SDL_Color blanc  = {235, 221, 192, 255};
    SDL_Color gris   = {160, 150, 130, 255};

    int lineTitle  = TTF_FontHeight(fontTitle);
    int lineNormal = TTF_FontHeight(fontNormal);

    int y = panel.y + 18;
    printTextCentered(renderer, fontTitle, "EN PRISON", rouge, y, winW);
    y += lineTitle + 8;
    SDL_SetRenderDrawColor(renderer, 180, 40, 40, 255);
    SDL_RenderDrawLine(renderer, panel.x + 20, y, panel.x + panel.w - 20, y);
    y += 16;

    int restants = bridge_jail_tours_restants(ctx, partie);
    int passe    = 3 - restants;
    char buf[80];
    snprintf(buf, sizeof(buf), "Tours en prison : %d / 3", passe);
    printTextCentered(renderer, fontNormal, buf, blanc, y, winW);
    y += lineNormal + 8;

    if (restants > 1) {
        snprintf(buf, sizeof(buf), "(%d tentative(s) restante(s))", restants);
        printTextCentered(renderer, fontNormal, buf, gris, y, winW);
    } else if (restants == 1) {
        printTextCentered(renderer, fontNormal, "Derniere chance ! Sinon sortie payante.", gris, y, winW);
    } else {
        printTextCentered(renderer, fontNormal, "Sortie forcee au prochain lancer !", rouge, y, winW);
    }

    SDL_SetRenderDrawColor(renderer, 55, 35, 90, 255);
    SDL_RenderFillRect(renderer, &pb->btnTenterDouble.rect);
    SDL_SetRenderDrawColor(renderer, 200, 160, 240, 255);
    SDL_RenderDrawRect(renderer, &pb->btnTenterDouble.rect);
    int btnLineH = TTF_FontHeight(fontNormal);
    int tw, th;
    TTF_SizeText(fontNormal, "Tenter un double", &tw, &th);
    int txD = pb->btnTenterDouble.rect.x + (pb->btnTenterDouble.rect.w - tw) / 2;
    printText(renderer, fontNormal, "Tenter un double", blanc,txD, pb->btnTenterDouble.rect.y + (pb->btnTenterDouble.rect.h - btnLineH) / 2);

    int hasCard = bridge_jail_has_sortie_card(ctx, partie);
    TTF_SizeText(fontNormal, "Utiliser carte lib.", &tw, &th);
    int txC = pb->btnUtiliserCarte.rect.x + (pb->btnUtiliserCarte.rect.w - tw) / 2;
    int tyC = pb->btnUtiliserCarte.rect.y + (pb->btnUtiliserCarte.rect.h - btnLineH) / 2;
    if (hasCard) {
        SDL_SetRenderDrawColor(renderer, 25, 65, 25, 255);
        SDL_RenderFillRect(renderer, &pb->btnUtiliserCarte.rect);
        SDL_SetRenderDrawColor(renderer, 80, 200, 80, 255);
        SDL_RenderDrawRect(renderer, &pb->btnUtiliserCarte.rect);
        printText(renderer, fontNormal, "Utiliser carte lib.", blanc, txC, tyC);
    } else {
        SDL_SetRenderDrawColor(renderer, 38, 38, 38, 255);
        SDL_RenderFillRect(renderer, &pb->btnUtiliserCarte.rect);
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderDrawRect(renderer, &pb->btnUtiliserCarte.rect);
        printText(renderer, fontNormal, "Utiliser carte lib.", gris, txC, tyC);
    }
}

void handlePrisonClick(int x, int y, EtatJeu* state, PrisonButtonsPack* pb, ButtonsPack* btn,
                       ContexteJeu* ctx, EtatPartie* partie, Pawn pawns[MAX_PLAYERS]) {

    if (isClicked(&pb->btnTenterDouble, x, y)) {
        int ja = partie->joueursActif;
        int result = bridge_jail_try_double(ctx, partie);

        if (result == 1 || result == 2) {
            startPawnMove(&pawns[ja], 10, ctx->dernier_de);
            partie->temps_lancer    = SDL_GetTicks();
            partie->dejaLance       = 1;
            partie->posAvantCarte   = ctx->joueurs[ja]->position;

            CarteInfo carteInfo;
            int pioche = bridge_play_turn(ctx, partie, &carteInfo);
            bridge_apply_bankruptcy(ctx, partie);

            if (pioche) {
                strncpy(partie->derniereCarte.titre,       carteInfo.titre,       63);
                strncpy(partie->derniereCarte.description, carteInfo.description, 127);
                partie->derniereCarte.paquet    = carteInfo.paquet;
                partie->derniereCarte.valeur    = carteInfo.valeur;
                partie->derniereCarte.effet     = carteInfo.effet;
                partie->carteEnCours            = 1;
                partie->doubleEnAttente         = 0;
                btn->throwDice.disabled         = 1;
                btn->endTurn.disabled           = 1;
            } else {
                partie->carteEnCours = 0;
                if (bridge_check_bankruptcy_gameover(partie)) {
                    *state = GAMEOVER;
                    return;
                }
                btn->throwDice.disabled = 1;
                btn->endTurn.disabled   = 0;
            }
            *state = LANCER;

        } else {
            partie->prisonFailAlpha = 255.0f;
            btn->throwDice.disabled = 1;
            btn->endTurn.disabled   = 0;
            switch (partie->nbJoueurs) {
                case 2: *state = PLATEAU2J; break;
                case 3: *state = PLATEAU3J; break;
                default: *state = PLATEAU4J; break;
            }
        }
        return;
    }


    if (isClicked(&pb->btnUtiliserCarte, x, y)) {
        if (bridge_jail_has_sortie_card(ctx, partie)) {
            bridge_jail_use_card(ctx, partie);
            btn->throwDice.disabled = 0;
            btn->endTurn.disabled   = 1;
            switch (partie->nbJoueurs) {
                case 2: *state = PLATEAU2J; break;
                case 3: *state = PLATEAU3J; break;
                default: *state = PLATEAU4J; break;
            }
        }
    }
}
