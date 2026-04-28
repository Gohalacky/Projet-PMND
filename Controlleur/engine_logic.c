
#include "engine_logic.h"
#include "game_engine_console.h" // Pour engine_resolve_case (déplacement par carte)
#include <stdio.h>
 
#define NB_CASE_PLATEAU 40
 

void logic_apply_card_effect(GameContext *ctx, Player *p, Carte *c,
                             Plateau *plateau, Prison *jail,
                             Player *all_players[], int nb_joueurs)
{
    if (!ctx || !p || !c) return;
 
    printf("\n┌─────────────────────────────────────┐");
    printf("\n│  CARTE : %-28s│", c->titre);
    printf("\n│  %-36s│", c->description);
    printf("\n└─────────────────────────────────────┘");
 
    double mult_evt = evenements_mult_gains_cartes(&ctx->evenement_mondial);
 
    switch (c->effet) {
 
        case CARTE_GAIN_ECTS: {
            if (c->paquet == PAQUET_COMMUNAUTE) {
                /* Communaute GAIN : tous les joueurs */
                printf("\n  Tous les joueurs gagnent %d ECTS !", c->valeur);
                for (int i = 0; i < nb_joueurs; i++)
                    if (all_players[i] && all_players[i]->credit >= 0)
                        player_add_credits(all_players[i], c->valeur);
            } else {
                int gain = (int)(c->valeur * mult_evt + 0.5);
                player_add_credits(p, gain);
                printf("\n  +%d ECTS", gain);
                if (mult_evt != 1.0) printf(" (x%.2f evenement)", mult_evt);
            }
            break;
        }
 
        case CARTE_PERTE_ECTS: {
            if (c->paquet == PAQUET_COMMUNAUTE) {
                /* Communaute PERTE : tous les joueurs */
                printf("\n  Tous les joueurs paient %d ECTS !", c->valeur);
                for (int i = 0; i < nb_joueurs; i++)
                    if (all_players[i] && all_players[i]->credit >= 0)
                        player_remove_credits(all_players[i], c->valeur);
            } else {
                int perte = (int)(c->valeur * mult_evt + 0.5);
                player_remove_credits(p, perte);
                printf("\n  -%d ECTS", perte);
                if (mult_evt != 1.0) printf(" (x%.2f evenement)", mult_evt);
            }
            break;
        }
 
        case CARTE_TAXE_ECO: {
            int montant = calcul_taxe_ecologique(c->valeur, p->eco_index);
            printf("\n  Taxe de base : %d ECTS | Indice eco : %d | Taxe reelle : %d ECTS",
                   c->valeur, p->eco_index, montant);
            player_remove_credits(p, montant);
            break;
        }
 
        case CARTE_DEPLACEMENT_RELATIF: {
            int ancienne_pos = p->position;
            int nouvelle_pos = (p->position + c->valeur + NB_CASE_PLATEAU) % NB_CASE_PLATEAU;
            if (c->valeur > 0 && nouvelle_pos < ancienne_pos) {
                printf("\n  Passage par la case Depart : +200 ECTS !");
                player_add_credits(p, 200);
            }
            p->position = nouvelle_pos;
            printf("\n  %s se deplace de %+d cases -> case %d (%s)",
                   p->name, c->valeur, p->position,
                   plateau->cases[p->position].nom_affichage);
            engine_resolve_case(p, all_players, ctx->properties,
                                ctx->gares, ctx->compagnies,
                                plateau, jail, 0, ctx);
            break;
        }
 
        case CARTE_DEPLACEMENT_ABSOLU: {
            int cible = c->valeur;
            if (cible >= 0 && cible < NB_CASE_PLATEAU) {
                if (cible <= p->position) {
                    printf("\n  Passage par la case Depart : +200 ECTS !");
                    player_add_credits(p, 200);
                }
                p->position = cible;
                printf("\n  %s se deplace vers la case %d (%s)",
                       p->name, cible, plateau->cases[cible].nom_affichage);
                engine_resolve_case(p, all_players, ctx->properties,
                                    ctx->gares, ctx->compagnies,
                                    plateau, jail, 0, ctx);
            }
            break;
        }
 
        case CARTE_BLOQUE_CONSTRUCTION: {
            int idx = p->number - 1;
            if (idx >= 0 && idx < MAX_PLAYER) {
                ctx->bloque_construction[idx] = 1;
                printf("\n  %s ne peut pas construire au prochain tour !", p->name);
            }
            break;
        }
 
        case CARTE_SORTIE_PRISON:
            printf("\n  %s conserve la carte 'Sortie de Prison'.", p->name);
            break;
 
        default:
            printf("\n  Effet de carte non supporte.");
            break;
    }
    printf("\n");
}
 


void logic_update_global_eco(GameContext *ctx)
{
    if (!ctx || ctx->nb_players == 0) return;
    int somme = 0, actifs = 0;
    for (int i = 0; i < ctx->nb_players; i++) {
        if (ctx->players[i] && ctx->players[i]->credit >= 0) {
            somme += (int)ctx->players[i]->eco_index;
            actifs++;
        }
    }
    if (actifs == 0) return;
    int moy = (somme + actifs / 2) / actifs;
    if (moy < GRADE_A) moy = GRADE_A;
    if (moy > GRADE_F) moy = GRADE_F;
    ctx->indice_eco_global = (EcoGrade)moy;
}
 

 // Appeler en fin de round (après que tous les joueurs ont joué).
void logic_handle_global_events(GameContext *ctx)
{
    if (!ctx) return;
 
    
    if (evenements_tick(&ctx->evenement_mondial)) {
        printf("\n======================================================");
        printf("\n  L'EVENEMENT MONDIAL EST TERMINE. Retour au calme.");
        printf("\n======================================================\n");
    }
 
    /* Tentative de declenchement */
    if (!evenements_est_actif(&ctx->evenement_mondial)) {
        logic_update_global_eco(ctx);
        double risque = evenements_bonus_risque_depuis_eco(ctx->indice_eco_global);
        if (evenements_tenter_demarrage(&ctx->evenement_mondial, 0.05, risque, NB_CASE_PLATEAU)) {
            const DefEvenement *def = evenements_def(ctx->evenement_mondial.id);
            printf("\n======================================================");
            printf("\n  /!\\ ALERTE MONDIALE : %s /!\\", def->nom);
            printf("\n  Duree : %d round(s)", ctx->evenement_mondial.tours_restants);
            if (ctx->evenement_mondial.id == EVT_INONDATIONS)
                printf("\n  Cases bloquees : %d a %d !",
                       ctx->evenement_mondial.zone_debut,
                       ctx->evenement_mondial.zone_debut + ctx->evenement_mondial.zone_longueur - 1);
            printf("\n======================================================\n");
        }
    }
 
    // Effets constants
    if (evenements_est_actif(&ctx->evenement_mondial)) {
        const DefEvenement *def = evenements_def(ctx->evenement_mondial.id);
        if (def->cout_fixe_tous > 0) {
            printf("\n  [EVENEMENT] %s : chaque joueur paie %d ECTS.", def->nom, def->cout_fixe_tous);
            for (int i = 0; i < ctx->nb_players; i++) {
                Player *pj = ctx->players[i];
                if (!pj || pj->credit < 0) continue;
                int cout = def->cout_fixe_tous;
                if (def->reduction_AB &&
                    (pj->eco_index == GRADE_A || pj->eco_index == GRADE_B))
                    cout = cout / 2;
                player_remove_credits(pj, cout);
                printf("\n    -> %s paie %d ECTS.", pj->name, cout);
            }
        }
        printf("\n  [EN COURS] %s (%d round(s))",
               def->nom, ctx->evenement_mondial.tours_restants);
        if (def->interdit_construire)   printf(" | Construction INTERDITE");
        if (def->mult_loyers != 1.0)    printf(" | Loyers x%.2f", def->mult_loyers);
        if (def->mult_cout_construction != 1.0)
            printf(" | Construction x%.2f", def->mult_cout_construction);
        printf("\n");
    }
}
