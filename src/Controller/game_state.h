#ifndef GAME_STATE_H
#define GAME_STATE_H
 
#include "../Model/player.h"
#include "../Model/property.h"
#include "../Model/cartes.h"
#include "../Model/evenements.h"
 
#define MAX_PLAYER 4 
 
typedef struct {
    Player *players[MAX_PLAYER];
    int nb_players;
    
    // Plateau
    Property *properties[NB_PROPRIETE];
    Gares *gares[NB_GARE];
    Compagnies *compagnies[NB_COMPAGNIE];
 
    // Systèmes
    PiocheChance pioche_chance;
    PiocheCommunaute pioche_communaute;
    EtatEvenementGlobal evenement_mondial;
    
    EcoGrade indice_eco_global; // La note moyenne du plateau
 
    // Flags par joueur (index = player->number - 1)
    int bloque_construction[MAX_PLAYER]; // 1 si le joueur ne peut pas construire ce tour
 
    int numero_round; // Compteur de rounds pour déclencher les événements en fin de round
} GameContext;
 
#endif
