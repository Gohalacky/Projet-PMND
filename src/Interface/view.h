/**
 * @file view.h
 * @brief Rendu principal et structures UI globales.
 *
 * Expose la fonction render() qui dispatche vers le bon rendu selon l'EtatJeu,
 * ainsi que les structures ButtonsPack, TextsPack et CheckboxPack qui regroupent
 * tous les éléments UI partagés entre les différents écrans.
 */

#ifndef VIEW_H
#define VIEW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Struct.h"
#include "assets.h"
#include "bridge.h"
#include "ui/text.h"
#include "ui/counter.h"
#include "ui/textbox.h"
#include "ui/checkbox.h"
#include "propwindow.h"
#include "tradewindow.h"
#include "pawn.h"
#include "anim.h"

/* Déclarations anticipées */
typedef struct PrisonButtonsPack PrisonButtonsPack;
typedef struct LoadMenuState     LoadMenuState;
typedef struct RulesMenuState    RulesMenuState;

#define REF_W 1280.0f /**< Largeur de référence de la fenêtre (px). */
#define REF_H  720.0f /**< Hauteur de référence de la fenêtre (px). */

/**
 * @brief Ensemble de tous les boutons cliquables de l'interface.
 *
 * Organisé par écran.  Initialisé dans main_interface.c, mis à l'échelle
 * par handleResize() lors de chaque redimensionnement de fenêtre.
 */
typedef struct ButtonsPack {
    /* Menu principal */
    Button newGame;    /**< "Nouvelle Partie". */
    Button loadGame;   /**< "Charger la Partie". */
    Button save;       /**< "Règlement". */
    /* Plateau */
    Button backToMenu; /**< Retour au menu. */
    Button throwDice;  /**< Lancer les dés. */
    Button properties; /**< Ouvrir la fenêtre propriétés. */
    Button trade;      /**< Ouvrir la fenêtre échange. */
    Button endTurn;    /**< Fin de tour. */
    /* Menu pause */
    Button pauseMenu;     /**< Bouton pause. */
    Button pauseSaveQuit; /**< "Sauvegarder & Quitter" dans l'overlay pause. */
    Button pauseResume;   /**< "Reprendre" dans l'overlay pause. */
    /* Nouvelle partie */
    Button launchGame; /**< "Lancer la partie". */
    Button cancel;     /**< "Annuler". */
    /* Popup carte piochée */
    Button okCarte;    /**< "OK" pour fermer la popup de carte. */
} ButtonsPack;

/**
 * @brief Ensemble de tous les labels de texte de l'interface.
 *
 * Chaque champ correspond à un texte affiché sur un écran particulier.
 * Mis à l'échelle par ScaleAllTexts() lors de chaque redimensionnement.
 */
typedef struct {
    /* Menu principal */
    Text menuNewGame;  /**< "NOUVELLE PARTIE". */
    Text menuLoadGame; /**< "CHARGER LA PARTIE". */
    Text menuSave;     /**< "RÈGLEMENT". */
    /* Nouvelle partie */
    Text titleNewGame;  /**< "Nouvelle Partie" (titre). */
    Text numberPlayer;  /**< "Choix du nombre de joueurs". */
    Text chooseParam;   /**< "Choix des paramètres". */
    Text btnLaunchGame; /**< Libellé du bouton Lancer. */
    Text btnCancel;     /**< Libellé du bouton Annuler. */
    Text initialMoney;  /**< "Argent initial". */
    Text tillBankruptcy;/**< "Ou jusqu'à la faillite". */
    Text numbTurns;     /**< "Nombre de tours". */
    Text event;         /**< "Fréquence d'événement". */
    /* Plateau */
    Text btnThrowDice;  /**< Libellé du bouton Lancer les dés. */
    Text btnProperties; /**< Libellé du bouton Propriétés. */
    Text btnEndTurn;    /**< Libellé du bouton Fin de tour. */
    Text indNbTurn;     /**< Indicateur "Tour". */
    Text indJoueurActif;/**< Nom du joueur actif. */
} TextsPack;

/**
 * @brief Cases à cocher de l'interface.
 */
typedef struct {
    Checkbox tourInfini; /**< "Ou jusqu'à la faillite" (désactive le nombre de tours). */
} CheckboxPack;

/**
 * @brief Dessine le nom et le solde du joueur dont c'est le tour.
 * @param renderer Renderer SDL.
 * @param font     Police à utiliser.
 * @param partie   État de la partie (joueursActif, joueurs[]).
 * @param ctx      Contexte de jeu (position du joueur).
 */
void renderJoueurActif(SDL_Renderer* renderer, TTF_Font* font,
                       const EtatPartie* partie, ContexteJeu* ctx);

/**
 * @brief Recharge les textures si l'état a changé (transitions d'écran).
 * @param renderer Renderer SDL.
 * @param state    État courant.
 * @param tex      Pack de textures.
 */
void refreshTextures(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex);

/**
 * @brief Dessine la carte piochée avec un bouton OK.
 * @param renderer   Renderer SDL.
 * @param fontNormal Police corps.
 * @param fontTitle  Police titre.
 * @param partie     État de la partie (derniereCarte).
 * @param btn        Pack de boutons (btn->okCarte).
 */
void renderCartePiochee(SDL_Renderer* renderer, TTF_Font* fontNormal, TTF_Font* fontTitle,
                        const EtatPartie* partie, ButtonsPack* btn);

/**
 * @brief Dessine la bannière de l'événement actif s'il y en a un.
 * @param renderer   Renderer SDL.
 * @param fontNormal Police corps.
 * @param ctx        Contexte de jeu.
 * @param winW       Largeur courante de la fenêtre (px).
 * @param winH       Hauteur courante de la fenêtre (px).
 */
void renderEvenementActif(SDL_Renderer* renderer, TTF_Font* fontNormal,
                          const ContexteJeu* ctx, int winW, int winH);

/**
 * @brief Dessine l'écran de fin de partie avec le classement.
 * @param renderer   Renderer SDL.
 * @param fontTitle  Police titre.
 * @param fontNormal Police corps.
 * @param fontButton Police boutons.
 * @param partie     État final de la partie.
 * @param ctx        Contexte de jeu.
 * @param btn        Pack de boutons (btn->backToMenu).
 */
void renderGameOver(SDL_Renderer* renderer, TTF_Font* fontTitle, TTF_Font* fontNormal,
                    TTF_Font* fontButton, const EtatPartie* partie,
                    ContexteJeu* ctx, ButtonsPack* btn);

/**
 * @brief Fonction de rendu principale — dispatche selon l'état courant.
 *
 * Appelée à chaque frame.  Efface le renderer, dessine l'écran correspondant
 * à @p state, puis appelle SDL_RenderPresent().
 *
 * @param renderer       Renderer SDL.
 * @param state          État courant de l'interface.
 * @param tex            Pack de textures.
 * @param texts          Pack de textes UI.
 * @param fontButton     Police boutons.
 * @param fontNormal     Police corps.
 * @param fontTitle      Police titre.
 * @param counterJoueurs Compteur du nombre de joueurs.
 * @param tboxes         Champs de saisie.
 * @param checkboxes     Cases à cocher.
 * @param partie         État de la partie.
 * @param ctx            Contexte de jeu.
 * @param btn            Pack de boutons principaux.
 * @param pb             Boutons fenêtre propriétés.
 * @param tb             Boutons fenêtre échange.
 * @param ts             État de la transaction d'échange.
 * @param pawns          Tableau des pions joueurs.
 * @param lm             État du menu de chargement.
 * @param rm             État du menu règlement.
 * @param diceAnim       Animation des dés.
 * @param pb_prison      Boutons fenêtre prison.
 */
void render(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex,
            TextsPack* texts, TTF_Font* fontButton, TTF_Font* fontNormal, TTF_Font* fontTitle,
            Counter* counterJoueurs, TextBoxPack* tboxes, CheckboxPack* checkboxes,
            EtatPartie* partie, ContexteJeu* ctx, ButtonsPack* btn,
            PropButtonsPack* pb, TradeButtonsPack* tb, TradeState* ts,
            Pawn pawns[MAX_PLAYERS],
            LoadMenuState* lm, RulesMenuState* rm,
            Animation* diceAnim, PrisonButtonsPack* pb_prison,
            MoneyPopup popups[MAX_PLAYERS]);

/**
 * @brief Libère toutes les ressources SDL (fenêtre, renderer, textures).
 * @param win Fenêtre SDL à détruire.
 * @param rdr Renderer SDL à détruire.
 * @param tex Pack de textures à libérer via freeTextures().
 */
void cleanup(SDL_Window* win, SDL_Renderer* rdr, TexturesPack* tex);

#endif
