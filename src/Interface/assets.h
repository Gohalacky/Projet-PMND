/**
 * @file assets.h
 * @brief Chargement et libération des textures SDL2 du jeu.
 */

#ifndef ASSETS_H
#define ASSETS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

/**
 * @brief Ensemble de toutes les textures chargées en mémoire.
 *
 * Initialisé par loadTextures() au démarrage ; libéré par freeTextures() à la fermeture.
 * Les textures de bâtiments sont indexées selon leur niveau.
 */
typedef struct {
    SDL_Texture* menu;             /**< Fond du menu principal. */
    SDL_Texture* newGame;          /**< Fond de l'écran "Nouvelle partie". */
    SDL_Texture* launchGame;       /**< Fond partagé par "Nouvelle partie" et "Charger" et "Règlement". */
    SDL_Texture* gameBoard2Player; /**< Plateau de jeu 2 joueurs. */
    SDL_Texture* gameBoard3Player; /**< Plateau de jeu 3 joueurs. */
    SDL_Texture* gameBoard4Player; /**< Plateau de jeu 4 joueurs. */
    SDL_Texture* throws;           /**< Image de résultat de lancer de dés. */
    SDL_Texture* diceSheet;        /**< Spritesheet d'animation des dés. */
    SDL_Texture* menuProperties;   /**< Fond de la fenêtre propriétés. */
    SDL_Texture* test;             /**< Texture de test / debug. */
    SDL_Texture* maison[3];        /**< Textures de maisons par niveau (0–2). */
    SDL_Texture* immeuble[3];      /**< Textures d'immeubles par niveau (0–2). */
    SDL_Texture* parc[3];          /**< Textures de parcs par niveau (0–2). */
    SDL_Texture* fondProp;         /**< Fond de la carte propriété. */
    SDL_Texture* pion[4];          /**< Textures des pions joueurs (0–3). */
} TexturesPack;

/** 
 * @brief Charge toutes les textures depuis le répertoire @c assets/.
 * @param renderer  Renderer SDL cible.
 * @param tex       Structure à remplir.
 * @return 0 en cas de succès, -1 si au moins une texture n'a pas pu être chargée.
 */
int loadTextures(SDL_Renderer* renderer, TexturesPack* tex);

/**
 * @brief Libère toutes les textures du pack (appelle SDL_DestroyTexture sur chacune).
 * @param tex  Pack à vider.
 */
void freeTextures(TexturesPack* tex);

#endif
