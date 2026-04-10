#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
 
#include "../Modele/player.h"
#include "../Modele/property.h"
#include "../Controlleur/plateau.h"
#include "../Modele/prison.h"


//Retourne le nom de la case sur laquelle se trouve le joueur actif 
const char* engine_get_case_joueur_actif(int joueurActif,Player* joueurs[],const Plateau* plateau);

// Lance un tour complet pour un joueur
void engine_play_turn(Player *p, Player *all_players[],
                      Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[], Plateau *plateau, Prison * jail) ;

// Fonction pour lancer les dés maintenant déplacé dans le modèle dans tool
// Fonction pour lancer les dés
int engine_roll_dice(int * compteur_doubles);

//Sépare le lancer du dé du déroulement du tour
int engine_roll_and_move(Player* p, const Plateau* plateau, int * compteur_doubles);

//Résout uniquement la case sur laquelle le joueur se trouve déjà sans relancer les dés
void engine_resolve_case(Player *p, Player *all_players[],Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[],
                         Plateau *plateau, int total_des);

                         


#endif