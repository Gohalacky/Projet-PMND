#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "../Modele/player.h"
#include "../Modele/property.h"
#include "../Controlleur/plateau.h"

// Lance un tour complet pour un joueur
void engine_play_turn(Player *current_p, Player *all_players[],
                      Property *tableau_prop[], Plateau *plateau);

// Fonction pour lancer les dés
int engine_roll_dice();




#endif