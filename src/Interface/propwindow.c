#include "propwindow.h"
#include "tradewindow.h"

#include <stdio.h>
#include <string.h>

#define WIN_X 290
#define WIN_Y 5
#define WIN_W 700
#define WIN_H 750

#define FOND_X 40
#define FOND_Y 10
#define FOND_W 1200
#define FOND_H 700

#define TITLE_H 75
#define BTN_W 180
#define BTN_H 40 

void initPropButtons(PropButtonsPack* pb) {
    pb->fermer           = (Button){ .ref = {WIN_X + WIN_W - 60, WIN_Y + 40,          36,    36   } };
    pb->acheter          = (Button){ .ref = {WIN_X + 30,          WIN_Y + WIN_H - 150,  BTN_W, BTN_H} };
    pb->construire       = (Button){ .ref = {WIN_X + 30,          WIN_Y + WIN_H - 280, BTN_W, BTN_H} };
    pb->construireSocial = (Button){ .ref = {WIN_X + 230,         WIN_Y + WIN_H - 280, BTN_W, BTN_H} };
    pb->construireParc   = (Button){ .ref = {WIN_X + 430,         WIN_Y + WIN_H - 280, BTN_W, BTN_H} };
    pb->echanger         = (Button){ .ref = {WIN_X + WIN_W - BTN_W - 30, WIN_Y + WIN_H - 150, BTN_W, BTN_H} };
    scalePropButtons(pb, 1.0f, 1.0f);
}

void scalePropButtons(PropButtonsPack* pb, float ratioX, float ratioY) {
    ScaleButton(&pb->fermer,           ratioX, ratioY);
    ScaleButton(&pb->acheter,          ratioX, ratioY);
    ScaleButton(&pb->construire,       ratioX, ratioY);
    ScaleButton(&pb->construireSocial, ratioX, ratioY);
    ScaleButton(&pb->construireParc,   ratioX, ratioY);
    ScaleButton(&pb->echanger,         ratioX, ratioY);
}

static void drawButton(SDL_Renderer* renderer, TTF_Font* font,const Button* btn, const char* label,SDL_Color bg, SDL_Color textColor) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(renderer, &btn->rect);
    SDL_SetRenderDrawColor(renderer, 100, 75, 60, 255);
    SDL_RenderDrawRect(renderer, &btn->rect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    if (font && label) {
        int tw, th;
        TTF_SizeUTF8(font, label, &tw, &th);
        printText(renderer, font, label, textColor,btn->rect.x + (btn->rect.w - tw) / 2,btn->rect.y + (btn->rect.h - th) / 2);
    }
}

void renderPropWindow(SDL_Renderer* renderer,TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontSmall,const TexturesPack* tex,ContexteJeu* ctx,const EtatPartie* partie,PropButtonsPack* pb,int winW, int winH) {

    if (!ctx || !ctx->joueurs[partie->joueursActif]) return;

    float rx = (float)winW / REF_W;
    float ry = (float)winH / REF_H;

    SDL_Color blanc    = {235, 221, 192, 255};
    SDL_Color noir     = { 60,  45,  35, 255};
    SDL_Color vert     = { 80, 160,  80, 220};
    SDL_Color rouge    = {180,  60,  60, 220};
    SDL_Color gris     = {120, 110, 100, 220};
    SDL_Color disabled = {100,  95,  90, 150};

    /* Overlay sombre */
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_Rect overlay = {0, 0, winW, winH};
    SDL_RenderFillRect(renderer, &overlay);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    /* Image de fond */
    SDL_Rect fondRect = {(int)(FOND_X * rx), (int)(FOND_Y * ry),(int)(FOND_W * rx), (int)(FOND_H * ry),};
    SDL_RenderCopy(renderer, tex->fondProp, NULL, &fondRect);

    /* Titre */
    int titleY = (int)((WIN_Y + TITLE_H - 60 / 2) * ry);
    printTextCentered(renderer, fontTitle, "Gestion des proprietes", noir, titleY, winW);

    /* Bouton fermer */
    drawButton(renderer, fontNormal, &pb->fermer, "X", rouge, blanc);

    int contentY = (int)((WIN_Y + TITLE_H + 40) * ry);
    int leftX    = (int)((WIN_X + 30) * rx);
    char buf[128];

    char   propName[50];
    int    prix = 0, credits = 0, owner = 0;
    int    hasProp = bridge_get_current_prop_info(ctx, partie,propName, &prix, &credits, &owner);

    if (hasProp) {
        snprintf(buf, sizeof(buf), "Case actuelle : %s", propName);
        printText(renderer, fontNormal, buf, noir, leftX, contentY);
        contentY += (int)(TTF_FontHeight(fontNormal) * 1.4f);

        if (owner == 0) {
            snprintf(buf, sizeof(buf), "Prix : %d CR  |  Votre solde : %d CR", prix, credits);
            printText(renderer, fontSmall, buf, noir, leftX, contentY);
            contentY += (int)(TTF_FontHeight(fontSmall) * 1.4f);

            int canBuy = bridge_can_buy_current(ctx, partie);
            pb->acheter.disabled = !canBuy;
            snprintf(buf, sizeof(buf), "Acheter (%d CR)", prix);
            drawButton(renderer, fontSmall, &pb->acheter, buf, canBuy ? vert : gris, blanc);

        } else if (owner == partie->joueursActif + 1) {
            printText(renderer, fontSmall, "Vous etes proprietaire de cette case.",noir, leftX, contentY);
            contentY += (int)(TTF_FontHeight(fontSmall) * 1.4f);
            pb->acheter.disabled = 1;
            drawButton(renderer, fontSmall, &pb->acheter, "Deja possedee", gris, blanc);
        } else {
            snprintf(buf, sizeof(buf), "Appartient au joueur %d.", owner);
            printText(renderer, fontSmall, buf, noir, leftX, contentY);
            contentY += (int)(TTF_FontHeight(fontSmall) * 1.4f);
            pb->acheter.disabled = 1;
            drawButton(renderer, fontSmall, &pb->acheter, "Non disponible", gris, blanc);
        }
    } else {
        printText(renderer, fontSmall, "Case actuelle : pas de propriete achetable.",noir, leftX, contentY);
        contentY += (int)(TTF_FontHeight(fontSmall) * 1.4f);
        pb->acheter.disabled = 1;
        drawButton(renderer, fontSmall, &pb->acheter, "Acheter", gris, blanc);
    }

    int buildY = (int)((WIN_Y + WIN_H - 350) * ry);
    printText(renderer, fontNormal, "Construire sur vos proprietes :", noir, leftX, buildY-200);
    buildY += (int)(TTF_FontHeight(fontNormal) * 1.3f);

    PropInfo owned[NB_PROPRIETE];
    int nbOwned = bridge_get_owned_props(ctx, partie, owned, NB_PROPRIETE);

    if (nbOwned == 0) {
        printText(renderer, fontSmall, "  Aucune propriete possedee.", noir, leftX, buildY-190);
    } else {
        for (int i = 0; i < nbOwned; i++) {
            snprintf(buf, sizeof(buf), "- %s (niv.%d) %s",owned[i].name, owned[i].level,owned[i].canBuild ? "[Construction possible]" : "");
            printText(renderer, fontSmall, buf, noir, leftX, buildY-190);
            buildY += (int)(TTF_FontHeight(fontSmall) * 1.2f);
            if (buildY > (int)((WIN_Y + WIN_H - 190) * ry)) break;
        }
    }

    int anyBuildable = bridge_has_buildable(ctx, partie);
    pb->construire.disabled       = !anyBuildable;
    pb->construireSocial.disabled = !anyBuildable;
    pb->construireParc.disabled   = !anyBuildable;

    SDL_Color bgBuild = anyBuildable ? vert : disabled;
    drawButton(renderer, fontSmall, &pb->construire,       "Maison", bgBuild, blanc);
    drawButton(renderer, fontSmall, &pb->construireSocial, "Social", bgBuild, blanc);
    drawButton(renderer, fontSmall, &pb->construireParc,   "Parc",   bgBuild, blanc);

    /* Bouton echanger */
    SDL_Color bleu = {60, 100, 180, 220};
    drawButton(renderer, fontSmall, &pb->echanger, "Echanger", bleu, blanc);
}

void handlePropClick(int x, int y, EtatJeu* state,PropButtonsPack* pb,ContexteJeu* ctx,EtatPartie* partie, TradeState* ts) {

    /* Echanger */
    if (isClicked(&pb->echanger, x, y)) {
        bridge_trade_init(ctx, partie, ts);
        *state = TRADE;
        return;
    }

    /* Fermer */
    if (isClicked(&pb->fermer, x, y)) {
        switch (partie->nbJoueurs) {
            case 2: *state = PLATEAU2J; break;
            case 3: *state = PLATEAU3J; break;
            case 4: *state = PLATEAU4J; break;
        }
        return;
    }

    /* Acheter */
    if (isClicked(&pb->acheter, x, y) && !pb->acheter.disabled) {
        bridge_buy_current(ctx, partie);
        return;
    }

    /* Construire */
    if (isClicked(&pb->construire,       x, y) && !pb->construire.disabled){
        bridge_build(ctx, partie, 1);
    }else if (isClicked(&pb->construireSocial, x, y) && !pb->construireSocial.disabled){
        bridge_build(ctx, partie, 2);
    }else if (isClicked(&pb->construireParc,   x, y) && !pb->construireParc.disabled){
        bridge_build(ctx, partie, 3);
    }
}