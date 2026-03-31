#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "../Modele/player.h"
#include "../Modele/property.h"
#include "../Controlleur/plateau.h"

// Lance un tour complet pour un joueur
void engine_play_turn(Player *p, Player *all_players[],
                      Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[], Plateau *plateau, int admin_mode) ;

// Fonction pour lancer les dés
int engine_roll_dice(int admin_mode);




#endif