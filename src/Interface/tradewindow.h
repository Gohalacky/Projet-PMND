#ifndef TRADEWINDOW_H
#define TRADEWINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "assets.h"
#include "bridge.h"
#include "Struct.h"
#include "ui/button.h"

#ifndef REF_W
#define REF_W 1280.0f
#define REF_H  720.0f
#endif

/* Nombre max de biens affichables dans la liste par joueur */
#define TRADE_MAX_ASSETS 32
/* Nombre max de boutons de biens (un par ligne de la liste) */
#define TRADE_MAX_ASSET_BTNS 32

/**
 * @brief Pack de boutons de la fenêtre d'échange.
 *
 * Organisé par phase :
 *  - fermer      : toujours visible
 *  - target[]    : phase SELECT_TARGET  (liste des joueurs cibles)
 *  - suivant     : passer à la phase suivante / valider
 *  - precedent   : revenir à la phase précédente
 *  - asset_btn[] : phase BUILD_OFFER et BUILD_DEMAND (un bouton par bien)
 *  - creditsPlus / creditsMoins : phase SET_CREDITS
 *  - accepter / refuser : phase RECAP (boutons de la cible)
 */
typedef struct {
    Button fermer;

    /* Sélection de cible (phase 0) */
    Button target[4];       /* max 4 joueurs potentiels */
    int    nb_targets;

    /* Navigation */
    Button suivant;
    Button precedent;

    /* Biens (phases 1 et 2) */
    Button asset_btn[TRADE_MAX_ASSET_BTNS];
    int    nb_asset_btns;

    /* Compensation financière (phase 3) */
    Button creditsActifPlus;
    Button creditsActifMoins;
    Button creditsCiblePlus;
    Button creditsCibleMoins;

    /* Récapitulatif (phase 4) */
    Button accepter;
    Button refuser;

    /* Défilement de la liste de biens (phases 1 et 2) */
    int scroll_assets;
} TradeButtonsPack;

/* ─── Initialisations ─────────────────────────────────────────── */
void initTradeButtons(TradeButtonsPack* tb);
void scaleTradeButtons(TradeButtonsPack* tb, float rx, float ry);

/* ─── Rendu ───────────────────────────────────────────────────── */
/**
 * @brief Rendu complet de la fenêtre d'échange.
 *
 * Affiche le contenu correspondant à ts->phase :
 *  - TRADE_PHASE_SELECT_TARGET  : liste des joueurs cibles
 *  - TRADE_PHASE_BUILD_OFFER    : biens de l'actif à cocher
 *  - TRADE_PHASE_BUILD_DEMAND   : biens de la cible à cocher
 *  - TRADE_PHASE_SET_CREDITS    : saisie des compensations financières
 *  - TRADE_PHASE_RECAP          : récapitulatif et boutons accepter/refuser
 *  - TRADE_PHASE_DONE           : message de résultat
 */
void renderTradeWindow(SDL_Renderer* renderer,
                       TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontSmall,
                       const TexturesPack* tex,
                       ContexteJeu* ctx,
                       const EtatPartie* partie,
                       TradeButtonsPack* tb,
                       TradeState* ts,
                       int winW, int winH);

/* ─── Gestion des clics ───────────────────────────────────────── */
/**
 * @brief Traite un clic dans la fenêtre d'échange.
 *
 * Met à jour ts et tb en fonction de la phase courante,
 * et bascule state vers PLATEAU*J quand l'échange est terminé ou fermé.
 */
void handleTradeClick(int x, int y,
                      EtatJeu* state,
                      TradeButtonsPack* tb,
                      ContexteJeu* ctx,
                      EtatPartie* partie,
                      TradeState* ts);

#endif /* TRADEWINDOW_H */
