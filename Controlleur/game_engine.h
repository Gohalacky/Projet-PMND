#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "../Modele/player.h"
#include "../Modele/property.h"

// Lance un tour complet pour un joueur
void engine_play_turn(Player *current_p, Player *all_players[], int nb_players, Property *tableau_prop[]);

// Fonction pour lancer les dés
int engine_roll_dice();

#endif