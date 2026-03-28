/**
 * @file hypo.h
 * @brief Modèle de gestion du systèmes d'hypothèque.
 * * Ce fichier gère les fonctions pour les hypothèques.
 */

#ifndef HYPO_H
#define HYPO_H
#include "player.h"
#include "tool.h"
#include "property.h"

/**
 * @brief Fait qu'un joueur place une propriété standard sous hypothèque.
 * @return 1 si l'hypothèque est reussi 0 sinon par exemple si il reste des constructions.
 */
int hypotheque_PROP(Property * prop, Player *p);

/**
 * @brief Fait qu'un joueur place une gare sous hypothèque.
 * @return 1 si l'hypothèque est reussi 0 sinon.
 */
int hypotheque_GARE(Gares * g,Player * p);

/**
 * @brief Fait qu'un joueur place une compagnie sous hypothèque.
 * @return 1 si l'hypothèque est reussi 0 sinon.
 */
int hypotheque_COMP(Compagnies * c,Player * p);

/**
 * @brief Fait qu'un joueur rachete l'hypothèque d'une propriété standart.
 * @return 1 si l'hypothèque est racheté, 0 sinon ex : le joueur n'as pas assez d'argent.
 */
int rachathypo_PROP(Property * prop, Player * p);

/**
 * @brief Fait qu'un joueur rachete l'hypothèque d'une gare.
 * @return 1 si l'hypothèque est racheté, 0 sinon ex : le joueur n'as pas assez d'argent.
 */
int rachathypo_COMP(Compagnies * prop, Player * p);

/**
 * @brief Fait qu'un joueur rachete l'hypothèque d'une compagnie.
 * @return 1 si l'hypothèque est racheté, 0 sinon ex : le joueur n'as pas assez d'argent.
 */
int rachathypo_GARE(Gares * prop, Player * p);
#endif