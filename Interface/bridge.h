#ifndef BRIDGE_H
#define BRIDGE_H

#include "Struct.h"
#include "../Modele/player.h"
#include "../Modele/property.h"
#include "../Controlleur/plateau.h"
#include "../Controlleur/game_engine.h"

typedef struct{
    Player* joueurs[4];
    Property* tableau_prop[NB_PROPRIETE];
    Gares* tableau_gar[NB_GARE];
    Compagnies* tableau_comp[NB_COMPAGNIE];
    Plateau plateau;
    int admin_mode; /*0 dés aléatoires ou 1 mode admin*/
    int dernier_de;
}ContexteJeu;

const char* bridge_get_case_joueur_actif(const ContexteJeu* ctx, const EtatPartie* partie);

int bridge_init(ContexteJeu* ctx, const EtatPartie* partie);
void bridge_play_turn(ContexteJeu* ctx, EtatPartie* partie);
void bridge_sync(const ContexteJeu* ctx, EtatPartie* partie);
int bridge_next_turn(EtatPartie* partie);
void bridge_free(ContexteJeu* ctx);
int bridge_roll_and_move(ContexteJeu* ctx, EtatPartie* partie);

#endif 