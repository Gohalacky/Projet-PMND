/**
 * @file anim.h
 * @brief Animation de spritesheet frame-by-frame.
 */

#ifndef ANIM_H
#define ANIM_H

#include <SDL2/SDL.h>
#include <stdbool.h>

/**
 * @brief État d'une animation spritesheet.
 *
 * La spritesheet est parcourue ligne par ligne : la frame @c n est à la colonne
 * @c n%framesPerRow, ligne @c n/framesPerRow.
 */
typedef struct {
    SDL_Texture* sheet;    /**< Spritesheet source. */
    int frameW, frameH;    /**< Dimensions d'une frame en pixels. */
    int frameCount;        /**< Nombre total de frames. */
    int framesPerRow;      /**< Nombre de frames par ligne dans la spritesheet. */
    int fps;               /**< Vitesse de lecture (frames par seconde). */
    bool loop;             /**< true = revient à la frame 0 en fin de séquence. */
    int currentFrame;      /**< Index de la frame affichée. */
    Uint32 accumulator;    /**< Temps accumulé depuis le dernier changement de frame (ms). */
} Animation;

/**
 * @brief Initialise une animation.
 * @param anim         Animation à initialiser.
 * @param sheet        Spritesheet SDL chargée.
 * @param frameW       Largeur d'une frame (px).
 * @param frameH       Hauteur d'une frame (px).
 * @param frameCount   Nombre total de frames.
 * @param framesPerRow Nombre de frames par ligne dans la spritesheet.
 * @param fps          Vitesse de lecture.
 * @param loop         true pour boucler indéfiniment.
 */
void initAnim(Animation* anim, SDL_Texture* sheet,
              int frameW, int frameH, int frameCount,
              int framesPerRow, int fps, bool loop);

/**
 * @brief Avance l'animation d'un delta-temps.
 * @param anim    Animation à mettre à jour.
 * @param deltaMs Temps écoulé depuis le dernier appel (ms).
 * @return true si l'animation vient d'atteindre sa dernière frame.
 */
bool updateAnim(Animation* anim, Uint32 deltaMs);

/** 
 * @brief Dessine la frame courante.
 * @param renderer Renderer SDL.
 * @param anim Animation source.
 * @param x Coordonnée X de repli si @p dst est NULL.
 * @param y Coordonnée Y de repli si @p dst est NULL.
 * @param dst Rectangle de destination, ou NULL pour utiliser (x, y, frameW, frameH).
 */
void renderAnim(SDL_Renderer* renderer, Animation* anim, int x, int y, SDL_Rect* dst);

/**
 * @brief Remet l'animation à la frame 0 et réinitialise l'accumulateur.
 * @param anim Animation à réinitialiser.
 */
void resetAnim(Animation* anim);

#endif