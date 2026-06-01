/**
 * @file ui/text.h
 * @brief Fonctions d'affichage et de mise à l'échelle des textes.
 */

#ifndef UI_TEXT_H
#define UI_TEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../Struct.h"

/**
 * @brief Recalcule les coordonnées d'un Text selon les ratios de mise à l'échelle.
 * @param t      Texte à mettre à l'échelle (modifie t->x, t->y, t->size).
 * @param ratioX Ratio horizontal.
 * @param ratioY Ratio vertical.
 */
void ScaleText(Text* t, float ratioX, float ratioY);

/**
 * @brief Met à l'échelle un tableau de pointeurs de Text.
 * @param texts  Tableau de pointeurs vers les textes.
 * @param count  Taille du tableau.
 * @param ratioX Ratio horizontal.
 * @param ratioY Ratio vertical.
 */
void ScaleAllTexts(Text** texts, int count, float ratioX, float ratioY);

/**
 * @brief Recrée les trois polices principales à la bonne taille après un redimensionnement.
 *
 * Ferme les polices existantes et en ouvre de nouvelles à la taille mise à l'échelle.
 *
 * @param fontNormal  Pointeur vers la police normale (taille de référence 32).
 * @param fontTitle   Pointeur vers la police titre   (taille de référence 38).
 * @param fontButton  Pointeur vers la police bouton  (taille de référence 22).
 * @param ratioY      Ratio vertical (fenêtre_H / 720).
 * @param fontPath    Chemin du fichier .ttf à utiliser.
 */
void ScaleFonts(TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton,
                float ratioY, const char* fontPath);

/**
 * @brief Rend une chaîne de caractères à la position (x, y).
 * @param renderer Renderer SDL.
 * @param font     Police TTF à utiliser.
 * @param text     Chaîne à afficher.
 * @param color    Couleur RGBA.
 * @param x        Coordonnée X du coin supérieur gauche du texte.
 * @param y        Coordonnée Y du coin supérieur gauche du texte.
 */
void printText(SDL_Renderer* renderer, TTF_Font* font, const char* text,
               SDL_Color color, int x, int y);

/**
 * @brief Rend une chaîne centrée horizontalement sur la fenêtre à la hauteur y.
 * @param renderer Renderer SDL.
 * @param font     Police TTF à utiliser.
 * @param text     Chaîne à afficher.
 * @param color    Couleur RGBA.
 * @param y        Coordonnée Y du coin supérieur du texte.
 * @param winW     Largeur de la fenêtre (px), utilisée pour calculer le centrage.
 */
void printTextCentered(SDL_Renderer* renderer, TTF_Font* font, const char* text,
                       SDL_Color color, int y, int winW);

/**
 * @brief Rend un Text dont les coordonnées ont déjà été mises à l'échelle.
 * @param renderer Renderer SDL.
 * @param font     Police TTF à utiliser.
 * @param t        Text source (utilise t->x, t->y, t->color, t->text).
 */
void printTextResponsive(SDL_Renderer* renderer, TTF_Font* font, const Text* t);

/**
 * @brief Rend une chaîne à la position (x, y) avec une opacité personnalisée.
 * @param alpha Opacité (0 = transparent, 255 = opaque).
 */
void printTextAlpha(SDL_Renderer* renderer, TTF_Font* font, const char* text,
                    SDL_Color color, int x, int y, Uint8 alpha);

#endif
