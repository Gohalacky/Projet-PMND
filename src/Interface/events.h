/**
 * @file events.h
 * @brief Boucle de gestion des événements SDL2.
 *
 * handleEvents() est l'unique point d'entrée pour tous les événements SDL
 * (clics, clavier, redimensionnement, fermeture fenêtre).  Elle dispatche
 * vers les gestionnaires spécialisés (handlePropClick, handleTradeClick, ...)
 * selon l'état courant de EtatJeu.
 */

#ifndef EVENST_H
#define EVENST_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "Struct.h"
#include "bridge.h"
#include "propwindow.h"
#include "tradewindow.h"
#include "prisonwindow.h"
#include "view.h"
#include "pawn.h"
#include "loadmenu.h"
#include "rules.h"

/**
 * @brief Traite tous les événements SDL en attente.
 *
 * Gère : clic souris, saisie clavier, molette, redimensionnement de fenêtre
 * et SDL_QUIT.  Met à jour @p state, @p partie et les structures UI en
 * conséquence.  Les polices sont rechargées à l'échelle si la fenêtre est
 * redimensionnée.
 *
 * @param event          Événement SDL.
 * @param state          Etat du jeu.
 * @param btn            Pack de boutons principaux.
 * @param texts          Pack de textes.
 * @param fontNormal     Pointeur vers la police normale.
 * @param fontTitle      Pointeur vers la police titre.
 * @param fontButton     Pointeur vers la police bouton.
 * @param quit           Mis à true lors de SDL_QUIT ou clic sur "Quitter".
 * @param counterJoueurs Compteur du nombre de joueurs.
 * @param tboxes         Champs de saisie.
 * @param checkboxes     Cases à cocher.
 * @param partie         État de la partie côté Interface.
 * @param ctx            Contexte de jeu.
 * @param pb             Boutons de la fenêtre propriétés.
 * @param tb             Boutons de la fenêtre échange.
 * @param ts             État courant de la transaction d'échange.
 * @param pawns          Tableau des pions joueurs.
 * @param lm             État du menu de chargement de sauvegarde.
 * @param rm             État du menu règlement.
 * @param pb_prison      Boutons de la fenêtre prison.
 */
void handleEvents(SDL_Event* event, EtatJeu* state, ButtonsPack* btn, TextsPack* texts,
                  TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton,
                  bool* quit, Counter* counterJoueurs, TextBoxPack* tboxes,
                  CheckboxPack* checkboxes, EtatPartie* partie, ContexteJeu* ctx,
                  PropButtonsPack* pb, TradeButtonsPack* tb, TradeState* ts,
                  Pawn pawns[MAX_PLAYERS],
                  LoadMenuState* lm, RulesMenuState* rm,
                  PrisonButtonsPack* pb_prison);

#endif
