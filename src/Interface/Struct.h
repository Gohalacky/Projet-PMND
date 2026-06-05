/**
 * @file Struct.h
 * @brief Structures et énumérations partagées par toute la couche Interface.
 *
 * Contient les types de base de l'interface (Button, TextBox, Text, Counter),
 * les données joueur côté Vue (Joueur, EtatPartie) et l'état de la partie (EtatJeu).
 */

#ifndef STRUCT
#define STRUCT

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define TEXTBOX_MAX_LEN 16

typedef enum { ALIGN_LEFT, ALIGN_CENTER_X, ALIGN_RIGHT } TextAlign;

/**
 * @brief État global de l'application ; gère le rendu et la gestion d'événements.
 *
 * Les états PLATEAU2J / PLATEAU3J / PLATEAU4J correspondent au plateau en jeu
 * pour 2, 3 ou 4 joueurs.  
 */
typedef enum {
    MENU, NOUVPARTIE, CHARGERPARTIE,
    PLATEAU2J, PLATEAU3J, PLATEAU4J,
    LANCER, PROPRIETE, TRADE, TEST,
    PAUSE, CARTE, REGLEMENT, GAMEOVER, PRISON
} EtatJeu;

/**
 * @brief Bouton cliquable avec coordonnées de référence et coordonnées mises à l'échelle.
 *
 * @c ref contient les coordonnées à la résolution de référence (1280×720).
 * @c rect est recalculé par ScaleButton() à chaque redimensionnement.
 */
typedef struct {
    SDL_Rect rect;    /**< Rectangle mis à l'échelle*/
    SDL_Rect ref;     /**< Référence à 1280×720. */
    int      disabled; /**< 1 = bouton inactif. */
} Button;

/**
 * @brief Champ de saisie texte mono-ligne.
 */
typedef struct {
    char     text[TEXTBOX_MAX_LEN]; /**< Contenu saisi, terminé par '\0'. */
    int      cursor_pos;            /**< Position du curseur. */
    int      active;                /**< 1 si la TextBox est utilisée. */
    SDL_Rect rect;                  /**< Rectangle mis à l'échelle. */
    SDL_Rect ref;                   /**< Référence à 1280×720. */
} TextBox;

/**
 * @brief Label de texte positionnable et redimensionnable.
 *
 * Les champs @c refX / @c refY / @c refSize contiennent les valeurs à 1280×720 ;
 * @c x / @c y / @c size sont recalculés par ScaleText() à chaque redimensionnement.
 */
typedef struct {
    const char* text;  /**< Chaîne à afficher. */
    int refX, refY;    /**< Position de référence. */
    int refSize;       /**< Taille de police de référence. */
    int x, y;          /**< Position mise à l'échelle. */
    int size;          /**< Taille de police mise à l'échelle. */
    TextAlign align;   /**< Alignement horizontal. */
    SDL_Color color;   /**< Couleur RGBA. */
} Text;

/**
 * @brief Compteur numérique avec boutons + / -.
 */
typedef struct {
    int    value;    /**< Valeur courante. */
    int    min;      /**< Borne inférieure. */
    int    max;      /**< Borne supérieure. */
    Text   label;    /**< Label affichant la valeur. */
    Button btnPlus;  /**< Bouton d'incrémentation. */
    Button btnMoins; /**< Bouton de décrémentation. */
} Counter;

/**
 * @brief Représentation côté Vue d'un joueur.
 */
typedef struct {
    char  nom[32];   /**< Nom du joueur. */
    int   credits;   /**< Solde en crédits. */
    float noteEco;   /**< Note d'éco (index 1–6, affiché A–F). */
} Joueur;

/**
 * @brief Carte piochée (copiée depuis le Modèle).
 */
typedef struct {
    char titre[64];       /**< Titre de la carte. */
    char description[128]; /**< Description de l'effet. */
    int  paquet;          /**< 0 = Chance, 1 = Communauté. */
    int  valeur;          /**< Valeur monétaire associée. */
    int  effet;           /**< TypeEffetCarte : 0=GAIN, 1=PERTE, 2=TAXE_ECO, … */
} CartePiochee;

/**
 * @brief Popup d'animation de gain/perte de crédits au-dessus de la carte joueur.
 */
typedef struct {
    int   delta;    /**< Variation de crédits (positif = gain, négatif = perte). */
    float alpha;    /**< Opacité courante, de 255 à 0. */
    float offsetY;  /**< Décalage vertical en pixels (monte avec le temps). */
    int   active;   /**< 1 si l'animation est en cours. */
} MoneyPopup;

/**
 * @brief État de la partie côté Interface.
 *
 * Mis à jour par bridge_sync() après chaque action de jeu.
 */
typedef struct {
    Joueur joueurs[4];     /**< Données des joueurs pour les cartes. */
    int nbJoueurs;         /**< Nombre de joueurs (2 - 4). */
    int joueursActif;      /**< Index du joueur dont c'est le tour. */
    int tourActuel;        /**< Numéro du tour en cours. */
    int tourMax;           /**< Nombre maximal de tours (0 = infini). */
    int dejaLance;         /**< 1 si les dés ont déjà été lancés ce tour. */
    Uint32 temps_lancer;   /**< Horodatage SDL du dernier lancer (ms). */
    int carteEnCours;      /**< 1 si une carte vient d'être piochée. */
    int doubleEnAttente;   /**< 1 si un double a été réalisé avant une carte. */
    int posAvantCarte;     /**< Position après le dé, avant l'effet de déplacement d'une carte. */
    int prisonPending;     /**< 1 si le pion doit être téléporté en prison après l'animation. */
    float prisonFailAlpha; /**< Opacité de l'indication "double raté" (0 si inactif). */
    float failliteAlpha;   /**< Opacité de la notification de faillite (0 si inactif). */
    char  failliteNom[32]; /**< Nom du joueur venant de faire faillite. */
    CartePiochee derniereCarte; /**< Dernière carte piochée (valide si carteEnCours == 1). */
} EtatPartie;

#endif
