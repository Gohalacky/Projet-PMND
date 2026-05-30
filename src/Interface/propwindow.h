/**
 * @file propwindow.h
 * @brief Fenêtre modale de gestion des propriétés.
 *
 * Affiche la case courante, la liste des propriétés possédées par le joueur actif
 * (avec défilement), et les boutons d'action : Acheter, Construire, Échanger, Fermer.
 */

#ifndef PROPWINDOW_H
#define PROPWINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "tradewindow.h"
#include "assets.h"
#include "bridge.h"
#include "Struct.h"
#include "ui/button.h"
#include "ui/text.h"

#define REF_W 1280.0f          /**< Largeur de référence de la fenêtre (px). */
#define REF_H  720.0f          /**< Hauteur de référence de la fenêtre (px). */
#define PROP_LIST_VISIBLE 5    /**< Nombre de propriétés visibles simultanément dans la liste. */

/**
 * @brief Pack de boutons de la fenêtre propriétés.
 */
typedef struct PropButtonsPack {
    Button fermer;           /**< Ferme la fenêtre (retour au plateau). */
    Button acheter;          /**< Achète la propriété de la case courante. */
    Button construire;       /**< Construit un bâtiment standard sur la propriété sélectionnée. */
    Button construireSocial; /**< Construit un logement social. */
    Button construireParc;   /**< Construit un parc. */
    Button echanger;         /**< Ouvre la fenêtre d'échange. */
    Button propScrollUp;     /**< Fait défiler la liste vers le haut. */
    Button propScrollDown;   /**< Fait défiler la liste vers le bas. */
    int    propScroll;       /**< Index de défilement courant de la liste. */
} PropButtonsPack;

/**
 * @brief Initialise les boutons aux coordonnées de référence.
 * @param pb Pack à initialiser.
 */
void initPropButtons(PropButtonsPack* pb);

/**
 * @brief Met à l'échelle tous les boutons après un redimensionnement.
 * @param pb     Pack de boutons.
 * @param ratioX Ratio horizontal.
 * @param ratioY Ratio vertical.
 */
void scalePropButtons(PropButtonsPack* pb, float ratioX, float ratioY);

/**
 * @brief Gère le défilement de la liste de propriétés via la molette.
 * @param pb      Pack de boutons.
 * @param delta   Sens : positif = vers le haut, négatif = vers le bas.
 * @param nbOwned Nombre total de propriétés possédées (pour borner le scroll).
 */
void handlePropScroll(PropButtonsPack* pb, int delta, int nbOwned);

/**
 * @brief Dessine la fenêtre propriétés complète.
 * @param renderer   Renderer SDL.
 * @param fontTitle  Police titre.
 * @param fontNormal Police corps.
 * @param fontSmall  Police petite.
 * @param tex        Pack de textures.
 * @param ctx        Contexte de jeu.
 * @param partie     État de la partie.
 * @param pb         Pack de boutons.
 * @param winW       Largeur courante de la fenêtre (px).
 * @param winH       Hauteur courante de la fenêtre (px).
 */
void renderPropWindow(SDL_Renderer* renderer,
                      TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontSmall,
                      const TexturesPack* tex,
                      ContexteJeu* ctx, const EtatPartie* partie,
                      PropButtonsPack* pb, int winW, int winH);

/**
 * @brief Gère un clic dans la fenêtre propriétés.
 *
 * Met à jour l'état selon le bouton cliqué : achat, construction, échange ou fermeture.
 *
 * @param x      Coordonnée X du clic.
 * @param y      Coordonnée Y du clic.
 * @param state  Etat de l'interface.
 * @param pb     Pack de boutons.
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @param ts     État de la transaction d'échange.
 */
void handlePropClick(int x, int y, EtatJeu* state,
                     PropButtonsPack* pb, ContexteJeu* ctx,
                     EtatPartie* partie, TradeState* ts);

#endif
