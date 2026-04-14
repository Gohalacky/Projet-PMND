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
    Prison * prisonactu;
}ContexteJeu;

typedef struct{
    char name[50];
    int level;
    int canBuild;
    int prop_index;
}PropInfo;

int bridge_init(ContexteJeu* ctx, const EtatPartie* partie);
void bridge_free(ContexteJeu* ctx);

int bridge_roll_and_move(ContexteJeu* ctx, EtatPartie* partie);
void bridge_play_turn(ContexteJeu* ctx, EtatPartie* partie);
int bridge_next_turn(EtatPartie* partie);
void bridge_sync(const ContexteJeu* ctx, EtatPartie* partie);

const char* bridge_get_case_joueur_actif(const ContexteJeu* ctx, const EtatPartie* partie);

int bridge_get_current_prop_info(const ContexteJeu* ctx, const EtatPartie* partie, char name_out[50], int* prix_out, int* credits_joueur_out, int* owner_out);
int bridge_can_buy_current(const ContexteJeu* ctx, const EtatPartie* partie);
int bridge_get_owned_props(ContexteJeu* ctx, const EtatPartie* partie, PropInfo result[], int max);
int bridge_has_buildable(ContexteJeu* ctx, const EtatPartie* partie);
int bridge_buy_current(ContexteJeu* ctx, EtatPartie* partie);
int bridge_build(ContexteJeu* ctx, EtatPartie* partie, int build_type);

#endif 