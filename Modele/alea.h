#ifndef ALEA_H
#define ALEA_H

#include <stdint.h>

/* Generateur pseudo-aleatoire simple (xorshift32) */
typedef struct {
    uint32_t etat;
} Alea;

/* Initialise le generateur avec une graine (0 autorise). */
void alea_init(Alea *a, uint32_t graine);

/* Renvoie un entier pseudo-aleatoire 32 bits. */
uint32_t alea_u32(Alea *a);

/* Renvoie un entier dans [0, borne-1]. borne > 0. */
uint32_t alea_borne(Alea *a, uint32_t borne);

/* Renvoie 1 avec probabilite p dans [0.0, 1.0], sinon 0. */
int alea_chance(Alea *a, double p);

#endif