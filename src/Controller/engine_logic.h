#ifndef ENGINE_LOGIC_H
#define ENGINE_LOGIC_H
 
#include "game_state.h"
#include "plateau.h" 
#include "../Model/prison.h"
 
// Applique l'effet d'une carte sur un joueur.
// plateau est nécessaire pour les cartes de déplacement (résolution de case après mouvement).
// all_players et jail sont passés pour les mêmes raisons.
void logic_apply_card_effect(GameContext *ctx, Player *p, Carte *c,
                             Plateau *plateau, Prison *jail,
                             Player *all_players[], int nb_joueurs);
 
// Calcule et met à jour l'indice écologique global dans ctx->indice_eco_global
void logic_update_global_eco(GameContext *ctx);
 
// Gère le début de tour (événements mondiaux) appeler en fin de round
void logic_handle_global_events(GameContext *ctx);
 
#endif
