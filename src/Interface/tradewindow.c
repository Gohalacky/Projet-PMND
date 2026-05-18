#include "tradewindow.h"
#include "ui/text.h"

#include <stdio.h>
#include <string.h>

#define WIN_X  290
#define WIN_Y    5
#define WIN_W  700
#define WIN_H  750

#define FOND_X  40
#define FOND_Y  10
#define FOND_W 1200
#define FOND_H  700

#define TITLE_H  75
#define BTN_W   160
#define BTN_H    38

static const SDL_Color C_BLANC    = {235, 221, 192, 255};
static const SDL_Color C_NOIR     = { 60,  45,  35, 255};
static const SDL_Color C_VERT     = { 80, 160,  80, 220};
static const SDL_Color C_ROUGE    = {180,  60,  60, 220};
static const SDL_Color C_GRIS     = {120, 110, 100, 180};
static const SDL_Color C_BLEU     = { 60, 100, 180, 220};

static void _draw_button(SDL_Renderer* r, TTF_Font* font,const Button* btn, const char* label,SDL_Color bg, SDL_Color fg) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(r, &btn->rect);
    SDL_SetRenderDrawColor(r, 100, 75, 60, 255);
    SDL_RenderDrawRect(r, &btn->rect);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    if (font && label && label[0]) {
        int tw, th;
        TTF_SizeUTF8(font, label, &tw, &th);
        printText(r, font, label, fg,
                  btn->rect.x + (btn->rect.w - tw) / 2,
                  btn->rect.y + (btn->rect.h - th) / 2);
    }
}

static void _draw_background(SDL_Renderer* r, const TexturesPack* tex,int winW, int winH, float rx, float ry) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 160);
    SDL_Rect overlay = {0, 0, winW, winH};
    SDL_RenderFillRect(r, &overlay);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    SDL_Rect fond = {(int)(FOND_X * rx), (int)(FOND_Y * ry),
                     (int)(FOND_W * rx), (int)(FOND_H * ry)};
    SDL_RenderCopy(r, tex->fondProp, NULL, &fond);
}

static void _go_to_plateau(EtatJeu* state, const EtatPartie* partie) {
    switch (partie->nbJoueurs) {
        case 2: *state = PLATEAU2J; break;
        case 3: *state = PLATEAU3J; break;
        default: *state = PLATEAU4J; break;
    }
}



void initTradeButtons(TradeButtonsPack* tb) {
    memset(tb, 0, sizeof(TradeButtonsPack));

    tb->fermer = (Button){ .ref = {WIN_X + WIN_W - 60, WIN_Y + 40, 36, 36} };

    tb->suivant   = (Button){ .ref = {WIN_X + WIN_W - BTN_W - 30, WIN_Y + WIN_H - 120, BTN_W, BTN_H} };
    tb->precedent = (Button){ .ref = {WIN_X + 30,                  WIN_Y + WIN_H - 120, BTN_W, BTN_H} };

    tb->accepter = (Button){ .ref = {WIN_X + 30,                  WIN_Y + WIN_H - 120, BTN_W + 20, BTN_H} };
    tb->refuser  = (Button){ .ref = {WIN_X + WIN_W - BTN_W - 50,  WIN_Y + WIN_H - 120, BTN_W + 20, BTN_H} };

    int cx = WIN_X + 30;
    int cy = WIN_Y + WIN_H - 200;
    tb->creditsActifPlus  = (Button){ .ref = {cx + 390, cy,      40, BTN_H} };
    tb->creditsActifMoins = (Button){ .ref = {cx + 240, cy,      40, BTN_H} };
    tb->creditsCiblePlus  = (Button){ .ref = {cx + 390, cy + 60, 40, BTN_H} };
    tb->creditsCibleMoins = (Button){ .ref = {cx + 240, cy + 60, 40, BTN_H} };

    for (int i = 0; i < 4; i++)
        tb->target[i] = (Button){ .ref = {WIN_X + 60, WIN_Y + TITLE_H + 100 + i * 70, BTN_W + 40, BTN_H + 4} };

    for (int i = 0; i < TRADE_MAX_ASSET_BTNS; i++)
        tb->asset_btn[i] = (Button){ .ref = {WIN_X + 30, WIN_Y + TITLE_H + 100 + i * 48, BTN_W + 200, 40} };

    scaleTradeButtons(tb, 1.0f, 1.0f);
}

void scaleTradeButtons(TradeButtonsPack* tb, float rx, float ry) {
    ScaleButton(&tb->fermer,           rx, ry);
    ScaleButton(&tb->suivant,          rx, ry);
    ScaleButton(&tb->precedent,        rx, ry);
    ScaleButton(&tb->accepter,         rx, ry);
    ScaleButton(&tb->refuser,          rx, ry);
    ScaleButton(&tb->creditsActifPlus, rx, ry);
    ScaleButton(&tb->creditsActifMoins,rx, ry);
    ScaleButton(&tb->creditsCiblePlus, rx, ry);
    ScaleButton(&tb->creditsCibleMoins,rx, ry);
    for (int i = 0; i < 4; i++)
        ScaleButton(&tb->target[i], rx, ry);
    for (int i = 0; i < TRADE_MAX_ASSET_BTNS; i++)
        ScaleButton(&tb->asset_btn[i], rx, ry);
}


/* ------ Phase 0 : choix du partenaire ------ */
static void _render_select_target(SDL_Renderer* r,TTF_Font* fontNormal, TTF_Font* fontSmall,ContexteJeu* ctx, const EtatPartie* partie,TradeButtonsPack* tb,float rx, float ry, int winW) {
    (void)winW;
    int leftX = (int)((WIN_X + 60) * rx);

    /* Sous-titre */
    int subY = (int)((WIN_Y + TITLE_H + 65) * ry);
    printText(r, fontNormal, "Choisir un partenaire :", C_NOIR, leftX, subY);

    /* Liste des cibles */
    char names[4][32];
    int  idx[4];
    int nb = bridge_trade_get_targets(ctx, partie, names, idx, 4);
    tb->nb_targets = nb;

    for (int i = 0; i < nb; i++) {
        int by = (int)((WIN_Y + TITLE_H + 100 + i * 70) * ry);
        tb->target[i].rect.y = by;

        char buf[80];
        Player* pc = ctx->joueurs[idx[i]];
        snprintf(buf, sizeof(buf), "%s  (%d ECTS)", names[i], pc ? pc->credit : 0);
        _draw_button(r, fontSmall, &tb->target[i], buf, C_BLEU, C_BLANC);
    }

    if (nb == 0)
        printText(r, fontSmall, "Aucun partenaire disponible.", C_GRIS, leftX,
                  (int)((WIN_Y + TITLE_H + 110) * ry));
}

/* ------ Phase 1 & 2 : sélection des biens ------ */
static void _render_build_offer(SDL_Renderer* r,TTF_Font* fontNormal, TTF_Font* fontSmall,ContexteJeu* ctx,TradeButtonsPack* tb,TradeState* ts,float rx, float ry, int winW) {
    (void)winW;
    int joueur_idx = (ts->phase == TRADE_PHASE_BUILD_OFFER)
                     ? ts->joueur_actif_idx : ts->joueur_cible_idx;
    Player* p = ctx->joueurs[joueur_idx];

    int leftX = (int)((WIN_X + 30) * rx);
    int subY  = (int)((WIN_Y + TITLE_H + 65) * ry);

    char buf[128];
    const char* phase_label = (ts->phase == TRADE_PHASE_BUILD_OFFER)
                              ? "Votre offre (ce que vous donnez) :"
                              : "Votre demande (ce que la cible donne) :";
    printText(r, fontNormal, phase_label, C_NOIR, leftX, subY);

    if (p) {
        snprintf(buf, sizeof(buf), "Joueur : %s  |  Solde : %d ECTS", p->name, p->credit);
        printText(r, fontSmall, buf, C_GRIS, leftX, subY + (int)(TTF_FontHeight(fontNormal) * 1.3f));
    }

    TradeAssetInfo assets[TRADE_MAX_ASSETS];
    int nb = bridge_trade_get_assets(ctx, joueur_idx, assets, TRADE_MAX_ASSETS, ts);
    tb->nb_asset_btns = nb;

    int lineH = 46;
    int startY = (int)((WIN_Y + TITLE_H + 130) * ry);
    int maxY   = (int)((WIN_Y + WIN_H - 80) * ry);

    if (nb == 0) {
        printText(r, fontSmall, "  Aucun bien echangeable.", C_GRIS, leftX, startY);
    }

    for (int i = 0; i < nb; i++) {
        int by = startY + i * (int)(lineH * ry);
        if (by + (int)(lineH * ry) > maxY) break;

        tb->asset_btn[i].rect.y = by;
        tb->asset_btn[i].rect.x = (int)((WIN_X + 30) * rx);
        tb->asset_btn[i].rect.w = (int)((BTN_W + 200) * rx);
        tb->asset_btn[i].rect.h = (int)(40 * ry);

        int sel = bridge_trade_is_selected(ts, &assets[i]);
        SDL_Color bg = sel ? C_BLEU : C_GRIS;

        const char* type_str = (assets[i].type == 1) ? "[T]"
                             : (assets[i].type == 2) ? "[G]" : "[C]";
        snprintf(buf, sizeof(buf), "%s %-20s  %d ECTS",type_str, assets[i].name, assets[i].value);
        _draw_button(r, fontSmall, &tb->asset_btn[i], buf, bg, C_BLANC);
    }
}

/* ------ Phase 3 : compensation financière ------ */
static void _render_set_credits(SDL_Renderer* r,TTF_Font* fontNormal, TTF_Font* fontSmall,ContexteJeu* ctx,TradeButtonsPack* tb,TradeState* ts,float rx, float ry, int winW) {
    (void)winW;
    int leftX = (int)((WIN_X + 30) * rx);
    int subY  = (int)((WIN_Y + TITLE_H + 65) * ry);

    printText(r, fontNormal, "Compensation financiere :", C_NOIR, leftX, subY);

    Player* pa = ctx->joueurs[ts->joueur_actif_idx];
    Player* pc = ctx->joueurs[ts->joueur_cible_idx];

    char buf[128];
    int lineH  = (int)(TTF_FontHeight(fontSmall) * 2.2f); 
    int btnH   = (int)(BTN_H * ry);
    int textH  = TTF_FontHeight(fontSmall);
    int y = subY + (int)(TTF_FontHeight(fontNormal) * 1.5f);

    /* Actif */
    snprintf(buf, sizeof(buf), "%s verse : %d ECTS", pa ? pa->name : "?", ts->credits_actif);
    int btnOffsetActif = (lineH - btnH) / 2;  
    printText(r, fontSmall, buf, C_NOIR, leftX, y + (lineH - textH) / 2);

    tb->creditsActifMoins.rect.y = y + btnOffsetActif;
    tb->creditsActifPlus.rect.y  = y + btnOffsetActif;
    _draw_button(r, fontSmall, &tb->creditsActifMoins, " - ", C_ROUGE, C_BLANC);
    _draw_button(r, fontSmall, &tb->creditsActifPlus,  " + ", C_VERT,  C_BLANC);
    y += lineH;

    /* Cible */
    snprintf(buf, sizeof(buf), "%s verse : %d ECTS", pc ? pc->name : "?", ts->credits_cible);
    int btnOffsetCible = (lineH - btnH) / 2;
    printText(r, fontSmall, buf, C_NOIR, leftX, y + (lineH - textH) / 2);

    tb->creditsCibleMoins.rect.y = y + btnOffsetCible;
    tb->creditsCiblePlus.rect.y  = y + btnOffsetCible;
    _draw_button(r, fontSmall, &tb->creditsCibleMoins, " - ", C_ROUGE, C_BLANC);
    _draw_button(r, fontSmall, &tb->creditsCiblePlus,  " + ", C_VERT,  C_BLANC);
    y += lineH + (int)(15 * ry); 

    snprintf(buf, sizeof(buf), "Solde %s : %d ECTS  |  Solde %s : %d ECTS",
             pa ? pa->name : "?", pa ? pa->credit : 0,
             pc ? pc->name : "?", pc ? pc->credit : 0);
    printText(r, fontSmall, buf, C_GRIS, leftX, y);
}

/* ------ Phase 4 : récapitulatif ------ */
static void _render_recap(SDL_Renderer* r,TTF_Font* fontNormal, TTF_Font* fontSmall,ContexteJeu* ctx,TradeButtonsPack* tb,TradeState* ts,float rx, float ry, int winW) {
    (void)winW;
    int leftX = (int)((WIN_X + 30) * rx);
    int y     = (int)((WIN_Y + TITLE_H + 65) * ry);
    int lh    = (int)(TTF_FontHeight(fontSmall) * 1.4f);

    Player* pa = ctx->joueurs[ts->joueur_actif_idx];
    Player* pc = ctx->joueurs[ts->joueur_cible_idx];
    char buf[128];

    /* Ce que l'actif donne */
    snprintf(buf, sizeof(buf), "%s donne :", pa ? pa->name : "?");
    printText(r, fontNormal, buf, C_NOIR, leftX, y); y += lh + 4;

    TradeAssetInfo summary[TRADE_MAX_ASSETS];
    int credits_out = 0;
    int nb = bridge_trade_get_offer_summary(ts, 1, summary, TRADE_MAX_ASSETS, &credits_out);
    for (int i = 0; i < nb; i++) {
        const char* t = (summary[i].type == 1) ? "[Terrain]"
                      : (summary[i].type == 2) ? "[Gare]" : "[Comp]";
        snprintf(buf, sizeof(buf), "  %s %s", t, summary[i].name);
        printText(r, fontSmall, buf, C_NOIR, leftX, y); y += lh;
    }
    if (credits_out > 0) {
        snprintf(buf, sizeof(buf), "  [Argent]  %d ECTS", credits_out);
        printText(r, fontSmall, buf, C_NOIR, leftX, y); y += lh;
    }
    if (nb == 0 && credits_out == 0)
        { printText(r, fontSmall, "  (rien)", C_GRIS, leftX, y); y += lh; }

    y += 8;

    /* Ce que la cible donne */
    snprintf(buf, sizeof(buf), "%s donne :", pc ? pc->name : "?");
    printText(r, fontNormal, buf, C_NOIR, leftX, y); y += lh + 4;

    credits_out = 0;
    nb = bridge_trade_get_offer_summary(ts, 0, summary, TRADE_MAX_ASSETS, &credits_out);
    for (int i = 0; i < nb; i++) {
        const char* t = (summary[i].type == 1) ? "[Terrain]"
                      : (summary[i].type == 2) ? "[Gare]" : "[Comp]";
        snprintf(buf, sizeof(buf), "  %s %s", t, summary[i].name);
        printText(r, fontSmall, buf, C_NOIR, leftX, y); y += lh;
    }
    if (credits_out > 0) {
        snprintf(buf, sizeof(buf), "  [Argent]  %d ECTS", credits_out);
        printText(r, fontSmall, buf, C_NOIR, leftX, y); y += lh;
    }
    if (nb == 0 && credits_out == 0)
        { printText(r, fontSmall, "  (rien)", C_GRIS, leftX, y); y += lh; }

    y += 16;
    snprintf(buf, sizeof(buf), "%s, acceptez-vous ?", pc ? pc->name : "?");
    printText(r, fontSmall, buf, C_NOIR, leftX, y);

    /* Boutons accepter / refuser */
    _draw_button(r, fontSmall, &tb->accepter, "Accepter", C_VERT,  C_BLANC);
    _draw_button(r, fontSmall, &tb->refuser,  "Refuser",  C_ROUGE, C_BLANC);
}

/* ------ Phase 5 : résultat ------ */
static void _render_done(SDL_Renderer* r, TTF_Font* fontNormal, TTF_Font* fontSmall,
                          const TradeState* ts, float rx, float ry) {
    int cx = (int)lround((WIN_X + WIN_W / 2.0f) * rx);
    int cy = (int)lround((WIN_Y + WIN_H / 2.0f - 30) * ry);

    SDL_Color col = (ts->accepted == 1) ? C_VERT : C_ROUGE;
    const char* titre = (ts->accepted == 1) ? "Echange effectue !"
                       : (ts->accepted == 0) ? "Negociation rompue."
                                              : "Echange invalide.";
    /* Centrage simple */
    int tw, th;
    TTF_SizeUTF8(fontNormal, titre, &tw, &th);
    printText(r, fontNormal, titre, col, cx - tw / 2, cy);
    cy += th + 8;

    TTF_SizeUTF8(fontSmall, ts->message, &tw, &th);
    printText(r, fontSmall, ts->message, C_NOIR, cx - tw / 2, cy);
}


void renderTradeWindow(SDL_Renderer* renderer,TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontSmall,const TexturesPack* tex,ContexteJeu* ctx,const EtatPartie* partie,TradeButtonsPack* tb,TradeState* ts,int winW, int winH) {

    if (!ctx || !ts) return;

    float rx = (float)winW / REF_W;
    float ry = (float)winH / REF_H;

    _draw_background(renderer, tex, winW, winH, rx, ry);

    /* Titre */
    int titleY = (int)lround((WIN_Y + TITLE_H - 60 / 2.0f) * ry);
    printTextCentered(renderer, fontTitle, "Salon de negociation", C_NOIR, titleY, winW);

    /* Bouton fermer */
    _draw_button(renderer, fontNormal, &tb->fermer, "X", C_ROUGE, C_BLANC);

    /* Indicateur de phase */
    const char* phase_names[] = {
        "1/5 - Partenaire",
        "2/5 - Votre offre",
        "3/5 - Votre demande",
        "4/5 - Compensation",
        "5/5 - Validation",
        "Terminé"
    };
    if (ts->phase <= TRADE_PHASE_DONE) {
        int px = (int)((WIN_X + 30) * rx);
        int py = (int)((WIN_Y + TITLE_H + 41) * ry);
        printText(renderer, fontSmall, phase_names[ts->phase], C_GRIS, px, py);
    }

    /* Contenu selon la phase */
    switch (ts->phase) {
        case TRADE_PHASE_SELECT_TARGET:
            _render_select_target(renderer, fontNormal, fontSmall, ctx, partie, tb, rx, ry, winW);
            break;
        case TRADE_PHASE_BUILD_OFFER:
        case TRADE_PHASE_BUILD_DEMAND:
            _render_build_offer(renderer, fontNormal, fontSmall, ctx, tb, ts, rx, ry, winW);
            break;
        case TRADE_PHASE_SET_CREDITS:
            _render_set_credits(renderer, fontNormal, fontSmall, ctx, tb, ts, rx, ry, winW);
            break;
        case TRADE_PHASE_RECAP:
            _render_recap(renderer, fontNormal, fontSmall, ctx, tb, ts, rx, ry, winW);
            break;
        case TRADE_PHASE_DONE:
            _render_done(renderer, fontNormal, fontSmall, ts, rx, ry);
            break;
    }

    /* Boutons de navigation */
    if (ts->phase >= TRADE_PHASE_BUILD_OFFER && ts->phase <= TRADE_PHASE_SET_CREDITS) {
        _draw_button(renderer, fontSmall, &tb->suivant,   "Suivant ->", C_VERT,  C_BLANC);
        _draw_button(renderer, fontSmall, &tb->precedent, "<- Retour",  C_GRIS,  C_BLANC);
    }
    if (ts->phase == TRADE_PHASE_DONE) {
        _draw_button(renderer, fontSmall, &tb->suivant, "Fermer", C_GRIS, C_BLANC);
    }
}


void handleTradeClick(int x, int y,EtatJeu* state,TradeButtonsPack* tb,ContexteJeu* ctx,EtatPartie* partie,TradeState* ts) {

    /* Fermer (toutes phases) */
    if (isClicked(&tb->fermer, x, y)) {
        bridge_trade_free(ts);
        bridge_sync(ctx, partie);
        _go_to_plateau(state, partie);
        return;
    }

    switch (ts->phase) {

        /* ── Phase 0 : sélection de la cible ── */
        case TRADE_PHASE_SELECT_TARGET: {
            char names[4][32];
            int  idx[4];
            int nb = bridge_trade_get_targets(ctx, partie, names, idx, 4);
            for (int i = 0; i < nb; i++) {
                if (isClicked(&tb->target[i], x, y)) {
                    bridge_trade_select_target(ctx, ts, idx[i]);
                    return;
                }
            }
            break;
        }

        /* ── Phase 1 & 2 : sélection des biens ── */
        case TRADE_PHASE_BUILD_OFFER:
        case TRADE_PHASE_BUILD_DEMAND: {
            int joueur_idx = (ts->phase == TRADE_PHASE_BUILD_OFFER)
                             ? ts->joueur_actif_idx : ts->joueur_cible_idx;

            TradeAssetInfo assets[TRADE_MAX_ASSETS];
            int nb = bridge_trade_get_assets(ctx, joueur_idx, assets, TRADE_MAX_ASSETS, ts);

            for (int i = 0; i < nb && i < tb->nb_asset_btns; i++) {
                if (isClicked(&tb->asset_btn[i], x, y)) {
                    bridge_trade_toggle_asset(ctx, ts, &assets[i]);
                    return;
                }
            }

            /* Navigation */
            if (isClicked(&tb->suivant, x, y)) {
                bridge_trade_next_phase(ctx, ts);
                return;
            }
            if (isClicked(&tb->precedent, x, y)) {
                if (ts->phase > TRADE_PHASE_BUILD_OFFER)
                    ts->phase = (TradePhase)(ts->phase - 1);
                return;
            }
            break;
        }

        /* ── Phase 3 : compensation financière ── */
        case TRADE_PHASE_SET_CREDITS: {
            int step = 50; 

            Player* pa = ctx->joueurs[ts->joueur_actif_idx];
            Player* pc = ctx->joueurs[ts->joueur_cible_idx];

            if (isClicked(&tb->creditsActifPlus, x, y) && pa) {
                int newval = ts->credits_actif + step;
                if (newval <= pa->credit)
                    bridge_trade_set_credits_actif(ts, newval);
                return;
            }
            if (isClicked(&tb->creditsActifMoins, x, y)) {
                int newval = ts->credits_actif - step;
                if (newval >= 0) bridge_trade_set_credits_actif(ts, newval);
                return;
            }
            if (isClicked(&tb->creditsCiblePlus, x, y) && pc) {
                int newval = ts->credits_cible + step;
                if (newval <= pc->credit)
                    bridge_trade_set_credits_cible(ts, newval);
                return;
            }
            if (isClicked(&tb->creditsCibleMoins, x, y)) {
                int newval = ts->credits_cible - step;
                if (newval >= 0) bridge_trade_set_credits_cible(ts, newval);
                return;
            }

            if (isClicked(&tb->suivant, x, y)) {
                bridge_trade_next_phase(ctx, ts); 
                return;
            }
            if (isClicked(&tb->precedent, x, y)) {
                ts->phase = TRADE_PHASE_BUILD_DEMAND;
                return;
            }
            break;
        }

        /* ── Phase 4 : récapitulatif ── */
        case TRADE_PHASE_RECAP: {
            if (isClicked(&tb->accepter, x, y)) {
                bridge_trade_confirm(ctx, ts, 1);
                bridge_sync(ctx, partie);
                return;
            }
            if (isClicked(&tb->refuser, x, y)) {
                bridge_trade_confirm(ctx, ts, 0);
                return;
            }
            /* Retour possible */
            if (isClicked(&tb->precedent, x, y)) {
                ts->phase = TRADE_PHASE_SET_CREDITS;
                return;
            }
            break;
        }

        /* ── Phase 5 : terminé ── */
        case TRADE_PHASE_DONE: {
            if (isClicked(&tb->suivant, x, y)) {
                bridge_trade_free(ts);
                _go_to_plateau(state, partie);
            }
            break;
        }
    }
}