#include <stdio.h>
#include <stdlib.h>
#include "game_engine_console.h"
#include "engine_logic.h"
#include "../Model/voe.h"


void vider_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Convertit un EcoGrade (1-6) en lettre A-F */
static char eco_grade_letter(int grade) {
    switch (grade) {
        case 1: return 'A';
        case 2: return 'B';
        case 3: return 'C';
        case 4: return 'D';
        case 5: return 'E';
        case 6: return 'F';
        default: return '?';
    }
}

/* AFFICHAGE DE L'ÉTAT DU JOUEUR */
void display_player_status(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[], GameContext *ctx) {
    printf("\n==================================================");
    printf("\n   DOSSIER JOUEUR : %s", p->name);
    printf("\n==================================================");
    printf("\n Solde : %d ECTS |  Indice Éco : %c (note %d/6)", p->credit, eco_grade_letter(p->eco_index), p->eco_index);
    printf("\n Position : Case n°%d", p->position);
    if (ctx) printf("\n Indice Éco GLOBAL : %c (note %d/6)", eco_grade_letter(ctx->indice_eco_global), ctx->indice_eco_global);
    
    printf("\n\n PATRIMOINE IMMOBILIER :");
    int has_anything = 0;
    
    // PROPRIÉTÉS
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i] && tableau_prop[i]->owner == p->number) {
            char *type_str = "Terrain nu";
            int loyer = tableau_prop[i]->rent_empty;

            if (tableau_prop[i]->level > 0) {
                switch(tableau_prop[i]->type) {
                    case Maison: 
                        type_str = "Maisons"; 
                        loyer = tableau_prop[i]->rent_maison[tableau_prop[i]->level - 1];
                        break;
                    case Social: 
                        type_str = "Social"; 
                        loyer = tableau_prop[i]->rent_logement[tableau_prop[i]->level - 1];
                        break;
                    case Parc:   
                        type_str = "Parc Éco"; 
                        loyer = tableau_prop[i]->rent_parc[tableau_prop[i]->level - 1];
                        break;
                    default: break;
                }
            }
            printf("\n   > %-18s | %-12s | Niv: %d | Loyer: %d", 
                   tableau_prop[i]->name, type_str, tableau_prop[i]->level, loyer);
            has_anything = 1;
        }
    }

    // GARES
    for (int i = 0; i < NB_GARE; i++) {
        if (tableau_gare[i] && tableau_gare[i]->owner == p->number) {
            int n = nb_Gare(tableau_gare, p);
            printf("\n   > [GARE] %-12s | Loyer actuel: %d", 
                   tableau_gare[i]->name, tableau_gare[i]->rent[n-1]);
            has_anything = 1;
        }
    }

    // COMPAGNIES
    for (int i = 0; i < NB_COMPAGNIE; i++) {
        if (tableau_comp[i] && tableau_comp[i]->owner == p->number) {
            printf("\n   > [COMP] %-12s | Service Public", tableau_comp[i]->name);
            has_anything = 1;
        }
    }

    if (!has_anything) printf("\n   (Aucun bien possédé)");
    printf("\n==================================================\n");
}

/*  DÉS  */
int engine_roll_dice(int admin_mode, int *compteur_doubles) {
    int d1, d2;
    if (admin_mode) {
        printf("\n[ADMIN] Dé 1 : "); scanf("%d", &d1);
        printf("[ADMIN] Dé 2 : "); scanf("%d", &d2);
    } else {
        d1 = (rand() % 6) + 1;
        d2 = (rand() % 6) + 1;
        printf("\n Dés : %d et %d (Total: %d)", d1, d2, d1 + d2);
    }
    if (d1 == d2) {
        (*compteur_doubles)++;
        printf(" -> DOUBLE !");
    } else *compteur_doubles = 0;
    
    return d1 + d2;
}

/* ACHATS + ENCHÈRES si refus */
void engine_handle_purchase(Player *p, void *asset, int type,
                             Player *all_players[], int nb_joueurs) {
    int price = 0; char *name = "";
    if (type == 1) { price = ((Property*)asset)->value; name = ((Property*)asset)->name; }
    else if (type == 2) { price = ((Gares*)asset)->value; name = ((Gares*)asset)->name; }
    else if (type == 3) { price = ((Compagnies*)asset)->value; name = ((Compagnies*)asset)->name; }

    printf("\n [OPPORTUNITÉ] %s est libre pour %d ECTS.", name, price);

    int achete = 0;
    if (p->credit >= price) {
        printf("\nVoulez-vous investir ? (y/n) : ");
        char c; scanf(" %c", &c);
        if (c == 'y' || c == 'Y') {
            player_remove_credits(p, price);
            if (type == 1) player_add_property(p, (Property*)asset);
            else if (type == 2) player_add_gar(p, (Gares*)asset);
            else if (type == 3) player_add_comp(p, (Compagnies*)asset);
            printf(" Acquisition réussie !\n");
            achete = 1;
        }
    } else {
        printf("\n Fonds insuffisants (%d ECTS requis).\n", price);
    }

    /* Si le joueur n'a pas acheté, lancer une vente aux enchères */
    if (!achete) {
        /* Vérifier qu'il reste au moins un autre joueur solvable */
        int candidats = 0;
        for (int i = 0; i < nb_joueurs; i++)
            if (all_players[i] != p && all_players[i]->credit > 0) candidats++;

        if (candidats == 0) {
            printf("\n Aucun autre joueur ne peut enchérir. Le bien reste à la banque.\n");
            return;
        }

        printf("\n ═══ VENTE AUX ENCHÈRES : %s (mise de départ : 10 ECTS) ═══\n", name);

        AuctionState *auction = NULL;
        if (type == 1) auction = init_auction_prop((Property*)asset,p, all_players, nb_joueurs);
        else if (type == 2) auction = init_auction_gar((Gares*)asset,p, all_players, nb_joueurs);
        else if (type == 3) auction = init_auction_comp((Compagnies*)asset,p, all_players, nb_joueurs);

        if (!auction) return;

        /* Boucle d'enchères : tours complets jusqu'à ce qu'il ne reste qu'un joueur */
        int continuer_enchere = 1;
        while (continuer_enchere) {
            continuer_enchere = 0;

            for (int i = 0; i < nb_joueurs; i++) {
                Player *enchérisseur = all_players[i];
                if (enchérisseur->credit < 0) continue;  /* joueur éliminé uniquement */

                int dans_enchere = 0;
                for (int j = 0; j < MAX_PLAYERS; j++)
                    if (auction->in_auction[j] == enchérisseur) { dans_enchere = 1; break; }
                if (!dans_enchere) continue;

                printf("\n[%s - %d ECTS] Enchère actuelle : %d ECTS",
                       enchérisseur->name, enchérisseur->credit, auction->current_bid);
                printf("\n  1. Surenchérir  2. Se coucher\nChoix : ");
                int choix_enc; scanf("%d", &choix_enc);

                if (choix_enc == 1) {
                    printf("  Votre offre : ");
                    int offre; scanf("%d", &offre);
                    process_bid(auction, enchérisseur, offre);
                } else {
                    fold_player(auction, enchérisseur);
                }
            }

            if (auction->players_left > 1) continuer_enchere = 1;
        }

        finalize_auction(auction);
        free(auction);
    }
}

/* CONSTRUCTION (RÈGLE D'UNIFORMITÉ) */
void engine_manage_assets(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[], GameContext *ctx) {
    printf("\n Gestion des biens ");

    /* Vérification des blocages de construction (carte ou événement) */
    int idx = p->number - 1;
    int construction_bloquee = 0;
    if (ctx) {
        if (ctx->bloque_construction[idx]) {
            printf("\n [BLOQUÉ] Vous ne pouvez pas construire ce tour (carte Semaine de partiels).");
            construction_bloquee = 1;
        } else if (evenements_interdit_construire(&ctx->evenement_mondial)) {
            const DefEvenement *def = evenements_def(ctx->evenement_mondial.id);
            printf("\n [BLOQUÉ] Construction interdite par l'événement : %s", def->nom);
            construction_bloquee = 1;
        }
    }
    printf("\n1. Construire / Améliorer%s", construction_bloquee ? " [BLOQUÉ]" : "");
    printf("\n2. Hypothéquer un bien (Gagner des ECTS)");
    printf("\n3. Racheter une hypothèque (Retrouver les loyers)");
    printf("\n4. Vendre un bâtiment (Remboursement)");
    printf("\n0. Retour");
    printf("\nChoix : ");
    
    int action; scanf("%d", &action);
    if (action == 0) return;

    // Construction 
    if (action == 1) {
        if (construction_bloquee) {
            printf("\n Construction impossible ce tour.\n");
            return;
        }
        int list[NB_PROPRIETE], count = 0;
        for (int i = 0; i < NB_PROPRIETE; i++) {
            Property *prop = tableau_prop[i];
            if (prop->owner == p->number && full_color(tableau_prop, p, prop->color)) {
                if (prop->level < 3 && check_uniformity(tableau_prop, prop, 1)) {
                
                    //vérification du type de batiment déjà existant dans la couleur
                    int type_impose = Vide;
                    for (int j = 0; j < NB_PROPRIETE; j++) {
                        if (tableau_prop[j]->color == prop->color && tableau_prop[j]->type != Vide) {
                            type_impose = tableau_prop[j]->type;
                            break;
                        }
                    }

                    if (type_impose != Vide) {
                        printf("\n%d. %-15s (Niveau %d -> %d | Type : %d imposé | Coût: %d)", 
                        count + 1, prop->name, prop->level, prop->level + 1, type_impose, prop->construction_cost);
                    }else {
                        printf("\n%d. %-15s (Niveau %d -> %d | Coût: %d)", 
                        count + 1, prop->name, prop->level, prop->level + 1, prop->construction_cost);
                    }   
                    list[count++] = i;
                }
            }
        }

        if (count == 0) { printf("\n Construction impossible (Uniformité ou Monopole requis).\n"); return; }
    
        printf("\nChoix (0 pour quitter) : ");
        int choix; scanf("%d", &choix);
        if (choix > 0 && choix <= count) {
            Property *sel = tableau_prop[list[choix-1]];
        
            // On cherche si un type est déjà imposé pour cette couleur
            int t = Vide;
            for (int j = 0; j < NB_PROPRIETE; j++) {
                if (tableau_prop[j]->color == sel->color && tableau_prop[j]->type != Vide) {
                    t = tableau_prop[j]->type;
                    break;
                }
            }   

            // Si aucun type n'est imposé, on demande au joueur
            if (t == Vide) {
                printf("Type de bâtiment (1:Maison, 2:Social, 3:Parc) : ");
                scanf("%d", &t);
            }

            if (p->credit >= sel->construction_cost) {
                /* Coût de construction potentiellement majoré par un événement */
                int cout_reel = sel->construction_cost;
                if (ctx) {
                    double mult = evenements_mult_cout_construction(&ctx->evenement_mondial);
                    if (mult != 1.0) {
                        cout_reel = (int)(cout_reel * mult + 0.5);
                        printf("\n [ÉVÉNEMENT] Coût majoré : %d ECTS (x%.2f)", cout_reel, mult);
                    }
                }
                if (p->credit >= cout_reel) {
                    /* On force le coût réel en retirant directement les crédits
                       avant add_level (qui retire construction_cost en interne).
                       */
                    int cout_sauvegarde = sel->construction_cost;
                    sel->construction_cost = cout_reel;
                    add_level(tableau_prop, sel, p, t);
                    sel->construction_cost = cout_sauvegarde;
                    printf(" Chantier terminé !");
                } else {
                    printf(" Pas assez d'ECTS après majoration événementielle !\n");
                }
            } else printf(" Pas assez d'ECTS !\n");
        }
    }
    
    // Hypothèque
    else if (action == 2) {
        printf("\n HYPOTHÈQUES DISPONIBLES");
        // On utilise 3 listes pour stocker les indices des biens affichés
        int lp[NB_PROPRIETE], lg[NB_GARE], lc[NB_COMPAGNIE];
        int cp, cg, cc, global_count;
        cp = 0;
        cg = 0;
        cc = 0;
        global_count = 1;

        for(int i=0; i<NB_PROPRIETE; i++)
            if(tableau_prop[i]->owner == p->number && !tableau_prop[i]->hypo) {
            // On n'affiche le terrain que s'il est nu et que tout son groupe est nu
                if (tableau_prop[i]->level == 0 && !engine_group_has_buildings(tableau_prop, tableau_prop[i]->color)) {
                    printf("\n%d. [TERRAIN] %-15s (+%d ECTS)", global_count++, tableau_prop[i]->name, tableau_prop[i]->value/2);
                    lp[cp++] = i;
                }
            }
        for(int i=0; i<NB_GARE; i++)
            if(tableau_gare[i]->owner == p->number && !tableau_gare[i]->hypo) {
                printf("\n%d. [GARE]    %-15s (+%d ECTS)", global_count++, tableau_gare[i]->name, tableau_gare[i]->value/2);
                lg[cg++] = i;
            }
        for(int i=0; i<NB_COMPAGNIE; i++)
            if(tableau_comp[i]->owner == p->number && !tableau_comp[i]->hypo) {
                printf("\n%d. [COMP]    %-15s (+%d ECTS)", global_count++, tableau_comp[i]->name, tableau_comp[i]->value/2);
                lc[cc++] = i;
            }

        if (global_count == 1) { printf("\nAucun bien hypothéquable (Vendez les bâtiments d'abord).\n"); return; }
        
        int choix; printf("\nChoix : "); scanf("%d", &choix);

        if (choix > 0 && choix <= cp) {
            hypotheque_PROP(tableau_prop[lp[choix-1]], p);
        }
        else if (choix > cp && choix <= (cp + cg)) {
            hypotheque_GARE(tableau_gare[lg[choix - cp - 1]], p);
        }
        else if (choix > (cp + cg) && choix <= (cp + cg + cc)) {
            hypotheque_COMP(tableau_comp[lc[choix - cp - cg - 1]], p);
        }
    }

    // Rachat d'hypothèque
    else if (action == 3) {
        printf("\n RACHAT D'HYPOTHÈQUES (Coût +10%%)");
        int lp[NB_PROPRIETE], lg[NB_GARE], lc[NB_COMPAGNIE];
        int cp, cg, cc, global_count;
        cp = 0;
        cg = 0;
        cc = 0;
        global_count = 1;

        for(int i=0; i<NB_PROPRIETE; i++)
            if(tableau_prop[i]->owner == p->number && tableau_prop[i]->hypo) {
                printf("\n%d. [TERRAIN] %-15s (%d ECTS)", global_count++, tableau_prop[i]->name, (int)((tableau_prop[i]->value / 2.0) * 1.1));
                lp[cp++] = i;
            }
        for(int i=0; i<NB_GARE; i++)
            if(tableau_gare[i]->owner == p->number && tableau_gare[i]->hypo) {
                printf("\n%d. [GARE]    %-15s (%d ECTS)", global_count++, tableau_gare[i]->name, (int)((tableau_gare[i]->value / 2.0) * 1.1));
                lg[cg++] = i;
            }

        for(int i=0; i<NB_COMPAGNIE; i++)
            if(tableau_comp[i]->owner == p->number && tableau_comp[i]->hypo) {
                printf("\n%d. [COMP]    %-15s (%d ECTS)", global_count++, tableau_comp[i]->name, (int)((tableau_comp[i]->value / 2.0) * 1.1));
                lc[cc++] = i;
            }

        if (global_count == 1) { printf("\nAucune hypothèque en cours.\n"); return; }

        int choix; printf("\nChoix : "); scanf("%d", &choix);
        if (choix > 0 && choix <= cp) {
            rachathypo_PROP(tableau_prop[lp[choix-1]], p);
        }
        else if (choix > cp && choix <= cp + cg) {
            rachathypo_GARE(tableau_gare[lg[choix-cp-1]], p);
        }
        else if (choix > cp + cg && choix <= cp + cg + cc) {
            rachathypo_COMP(tableau_comp[lc[choix-cp-cg-1]], p);
        }
    }

    // Vendre un batiment
    // Vendre un bâtiment (Déconstruction)
    else if (action == 4) {
        printf("\n DÉCONSTRUCTION");
        int list[NB_PROPRIETE], count = 0;

        // On parcourt les propriétés pour lister celles qui ont des bâtiments
        for (int i = 0; i < NB_PROPRIETE; i++) {
            // Le joueur doit être propriétaire et avoir au moins 1 niveau
            if (tableau_prop[i]->owner == p->number && tableau_prop[i]->level > 0) {
                // On vérifie si la vente respecte la règle d'uniformité (-1)
                if (check_uniformity(tableau_prop, tableau_prop[i], -1)) {
                    // Remboursement standard : 50% du prix d'achat initial
                    int remboursement = tableau_prop[i]->construction_cost / 2;
                    
                    printf("\n%d. %-15s (Niveau %d -> %d | Remboursement: +%d ECTS)", 
                           count + 1, 
                           tableau_prop[i]->name, 
                           tableau_prop[i]->level, 
                           tableau_prop[i]->level - 1, 
                           remboursement);
                    
                    list[count++] = i; // On mémorise l'indice réel
                }
            }
        }

        if (count == 0) { 
            printf("\nAucun bâtiment ne peut être vendu pour le moment (vérifiez l'uniformité).\n"); 
            return; 
        }

        printf("\n0. Retour");
        printf("\nChoix : ");
        int choix; 
        scanf("%d", &choix);

        if (choix > 0 && choix <= count) {
            // Appel de ta fonction de déconstruction
            remove_level(tableau_prop, tableau_prop[list[choix-1]], p);
        }
    }

}

/* RÉSOLUTION DE CASE */
void engine_resolve_case(Player *p, Player *all_players[], Property *tableau_prop[], 
                         Gares *tableau_gare[], Compagnies *tableau_comp[], 
                         Plateau *plateau, Prison *jail, int total_des,
                         GameContext *ctx) {
    (void)jail;
    Case *c = &plateau->cases[p->position];
    printf("\n Vous êtes sur : %s", c->nom_affichage);

    /* Multiplicateur de loyer lié à l'événement actif */
    double mult_loyer = ctx ? evenements_mult_loyers(&ctx->evenement_mondial) : 1.0;

    /* Confinement : si le joueur est sur sa propre propriété, il peut s'y arrêter sans loyer */
    if (ctx && evenements_autorise_arret_chez_soi(&ctx->evenement_mondial)) {
        if (c->type == CASE_TERRAIN) {
            Property *prop = (Property*)c->data;
            if (prop && prop->owner == p->number) {
                printf("\n [CONFINEMENT] Vous restez chez vous, aucun effet.");
                return;
            }
        }
    }

    /* Case bloquée par inondations */
    if (ctx && evenements_case_est_bloquee(&ctx->evenement_mondial, p->position)) {
        printf("\n [INONDATIONS] Cette zone est bloquée ! Vous ne pouvez pas agir ici.");
        return;
    }

    switch (c->type) {
        case CASE_TERRAIN: {
            Property *prop = (Property*)c->data;
            if (prop->owner == 0) engine_handle_purchase(p, prop, 1, all_players, ctx ? ctx->nb_players : 1);
            else if (prop->owner != p->number) {
                /* Appliquer le multiplicateur avant d'appeler must_rent */
                if (mult_loyer != 1.0) {
                    /* Patch temporaire du loyer pour passer le multiplicateur */
                    int loyer_base = prop->rent_empty;
                    if (prop->level > 0) {
                        switch (prop->type) {
                            case Maison: loyer_base = prop->rent_maison[prop->level-1]; break;
                            case Social: loyer_base = prop->rent_logement[prop->level-1]; break;
                            case Parc:   loyer_base = prop->rent_parc[prop->level-1]; break;
                            default: break;
                        }
                    }
                    int loyer_event = (int)(loyer_base * mult_loyer + 0.5);
                    printf("\n [ÉVÉNEMENT] Loyer majoré : %d ECTS (x%.2f)", loyer_event, mult_loyer);
                    player_remove_credits(p, loyer_event);
                    player_add_credits(all_players[prop->owner-1], loyer_event);
                } else {
                    must_rent_Property(tableau_prop, prop, p, all_players[prop->owner-1]);
                }
            }
            break;
        }
        case CASE_GARE: {
            Gares *g = (Gares*)c->data;
            if (g->owner == 0) engine_handle_purchase(p, g, 2, all_players, ctx ? ctx->nb_players : 1);
            else if (g->owner != p->number) {
                if (mult_loyer != 1.0) {
                    int n = nb_Gare(tableau_gare, all_players[g->owner-1]);
                    int loyer_event = (int)(g->rent[n > 0 ? n-1 : 0] * mult_loyer + 0.5);
                    printf("\n [ÉVÉNEMENT] Loyer gare majoré : %d ECTS (x%.2f)", loyer_event, mult_loyer);
                    player_remove_credits(p, loyer_event);
                    player_add_credits(all_players[g->owner-1], loyer_event);
                } else {
                    must_rent_Gares(tableau_gare, g, p, all_players[g->owner-1]);
                }
            }
            break;
        }
        case CASE_COMPAGNIE: {
            Compagnies *cp = (Compagnies*)c->data;
            if (cp->owner == 0) engine_handle_purchase(p, cp, 3, all_players, ctx ? ctx->nb_players : 1);
            else if (cp->owner != p->number) must_rent_Compagnies(tableau_comp, cp, p, all_players[cp->owner-1], total_des);
            break;
        }
        case CASE_TAXE: {
            int montant = (p->position == 4) ? 200 : 100;
            printf("\n Vous devez payer l'administration : %d ECTS.\n", montant);
            player_remove_credits(p, montant);
            break;
        }
        case CASE_ALLEZ_PRISON: {
            printf("\n Direction la cellule ! Ne passez pas par la case départ.");
            p->position = 10;
            go_to_jail(p, jail);
            break;
        }
        case CASE_CHANCE: {
            if (ctx) {
                printf("\n [CHANCE] Vous piochez une carte Chance !");
                Carte *carte = piocher_carte_chance(&ctx->pioche_chance, p->number);
                if (carte) {
                    logic_apply_card_effect(ctx, p, carte, plateau, jail, all_players,
                                           ctx ? ctx->nb_players : 0);
                    /* Remettre la carte dans le paquet sauf si c'est une Sortie de Prison */
                    if (carte->effet != CARTE_SORTIE_PRISON)
                        remettre_carte_dans_paquet(carte);
                } else {
                    printf("\n (Pioche Chance vide)");
                }
            }
            break;
        }
        case CASE_COMMUNAUTE: {
            if (ctx) {
                printf("\n [COMMUNAUTÉ] Vous piochez une carte Caisse de Communauté !");
                Carte *carte = piocher_carte_communaute(&ctx->pioche_communaute, p->number);
                if (carte) {
                    logic_apply_card_effect(ctx, p, carte, plateau, jail, all_players,
                                           ctx ? ctx->nb_players : 0);
                    remettre_carte_dans_paquet(carte);
                } else {
                    printf("\n (Pioche Communauté vide)");
                }
            }
            break;
        }
        default: printf("\n Rien à signaler."); break;
    }
    // Gestion dette (crédit négatif ou nul sans biens)
    if (p->credit <= 0) {
        engine_resolve_debt(p, tableau_prop, tableau_gare, tableau_comp);
    }
}

/* ÉCHANGES */
void engine_handle_trade(Player *p_actif, Player *all_players[], int nb_joueurs, 
                         Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    
    printf("\n==================================================");
    printf("\n           SALON DE NÉGOCIATION     ");
    printf("\n==================================================");

    // 1. Choix du partenaire
    int valid_targets[MAX_PLAYER];
    int target_count = 0;
    for (int i = 0; i < nb_joueurs; i++) {
        if (all_players[i]->number != p_actif->number && all_players[i]->credit >= 0) {
            printf("\n%d. %-15s (Solde: %d ECTS)", target_count + 1, all_players[i]->name, all_players[i]->credit);
            valid_targets[target_count++] = i;
        }
    }

    if (target_count == 0) {
        printf("\nAucun partenaire disponible.");
        return;
    }

    printf("\n\nAvec qui négocier ? (0 pour annuler) : ");
    int t_idx; scanf("%d", &t_idx);
    if (t_idx <= 0 || t_idx > target_count) return;
    
    Player *p_cible = all_players[valid_targets[t_idx-1]];
    TradeOffer *off_actif = init_empty_offer(p_actif);
    TradeOffer *off_cible = init_empty_offer(p_cible);

    // 2. Sélection des biens (PROPOSITION puis DEMANDE)
    Player *participants[2] = {p_actif, p_cible};
    TradeOffer *offers[2] = {off_actif, off_cible};
    char *phase_msg[2] = {"VOTRE OFFRE (Ce que vous donnez)", "VOTRE DEMANDE (Ce que vous voulez)"};

    for (int k = 0; k < 2; k++) {
        int en_cours = 1;
        while (en_cours) {
            printf("\n--- %s ---", phase_msg[k]);
            printf("\n1. Ajouter un Terrain");
            printf("\n2. Ajouter une Gare");
            printf("\n3. Ajouter une Compagnie");
            printf("\n0. Passer à la suite");
            printf("\nChoix : ");
            int type_choix; scanf("%d", &type_choix);

            if (type_choix == 0) en_cours = 0;
            
            // TERRAINS
            else if (type_choix == 1) {
                int list[NB_PROPRIETE], count = 0;
                for (int i = 0; i < NB_PROPRIETE; i++) {
                    if (tableau_prop[i]->owner == participants[k]->number && tableau_prop[i]->level == 0) {
                        printf("%d. %s  ", count + 1, tableau_prop[i]->name);
                        list[count++] = i;
                    }
                }
                if (count > 0) {
                    printf("\nLequel ajouter ? (0 pour retour) : ");
                    int p_sel; scanf("%d", &p_sel);
                    if (p_sel > 0 && p_sel <= count) trade_add_property(offers[k], tableau_prop[list[p_sel-1]]);
                } else printf("\nAucun terrain nu disponible.");
            }

            // GARES
            else if (type_choix == 2) {
                int list[NB_GARE], count = 0;
                for (int i = 0; i < NB_GARE; i++) {
                    if (tableau_gare[i]->owner == participants[k]->number) {
                        printf("%d. %s  ", count + 1, tableau_gare[i]->name);
                        list[count++] = i;
                    }
                }
                if (count > 0) {
                    printf("\nLaquelle ajouter ? (0 pour retour) : ");
                    int g_sel; scanf("%d", &g_sel);
                    if (g_sel > 0 && g_sel <= count) trade_add_gare(offers[k], tableau_gare[list[g_sel-1]]);
                } else printf("\nAucune gare disponible.");
            }

            // COMPAGNIES
            else if (type_choix == 3) {
                int list[NB_COMPAGNIE], count = 0;
                for (int i = 0; i < NB_COMPAGNIE; i++) {
                    if (tableau_comp[i]->owner == participants[k]->number) {
                        printf("%d. %s  ", count + 1, tableau_comp[i]->name);
                        list[count++] = i;
                    }
                }
                if (count > 0) {
                    printf("\nLaquelle ajouter ? (0 pour retour) : ");
                    int c_sel; scanf("%d", &c_sel);
                    if (c_sel > 0 && c_sel <= count) trade_add_compagnie(offers[k], tableau_comp[list[c_sel-1]]);
                } else printf("\nAucune compagnie disponible.");
            }
        }
    }

    // Ajustement financier final
    int m1, m2;
    printf("\n--- COMPENSATION FINANCIÈRE ---");
    printf("\nECTS que vous versez à %s : ", p_cible->name);
    scanf("%d", &m1); trade_set_credits(off_actif, m1);
    printf("ECTS que %s vous verse : ", p_cible->name);
    scanf("%d", &m2); trade_set_credits(off_cible, m2);

    // Récapitulatif détaillé
    printf("\n==================================================");
    printf("\nRÉCAPITULATIF DE L'ACCORD :");
    
    printf("\n%s DONNE :", p_actif->name);
    for(int i=0; i<off_actif->nb_props; i++) printf("\n  [Terrain] %s", off_actif->props[i]->name);
    for(int i=0; i<off_actif->nb_gares; i++) printf("\n  [Gare]    %s", off_actif->gares[i]->name);
    for(int i=0; i<off_actif->nb_comps; i++) printf("\n  [Comp]    %s", off_actif->comps[i]->name);
    if(off_actif->ects > 0) printf("\n  [Argent]  %d ECTS", off_actif->ects);

    printf("\n\n%s DONNE :", p_cible->name);
    for(int i=0; i<off_cible->nb_props; i++) printf("\n  [Terrain] %s", off_cible->props[i]->name);
    for(int i=0; i<off_cible->nb_gares; i++) printf("\n  [Gare]    %s", off_cible->gares[i]->name);
    for(int i=0; i<off_cible->nb_comps; i++) printf("\n  [Comp]    %s", off_cible->comps[i]->name);
    if(off_cible->ects > 0) printf("\n  [Argent]  %d ECTS", off_cible->ects);
    printf("\n==================================================");

    // Validation par la cible
    if (is_trade_valid(off_actif, off_cible, tableau_prop)) {
        printf("\n%s, acceptez-vous cet accord ? (y/n) : ", p_cible->name);
        char final_c; scanf(" %c", &final_c);
        if (final_c == 'y' || final_c == 'Y') {
            execute_trade(off_actif, off_cible, tableau_prop);
        } else printf("\nNégociation rompue par %s.\n", p_cible->name);
    }

    free(off_actif); free(off_cible);
}


// Vérifie si le joueur possède encore quelque chose de vendable ou d'hypothécable
int engine_has_assets_left(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    for (int i = 0; i < NB_PROPRIETE; i++)
        if (tableau_prop[i]->owner == p->number && (tableau_prop[i]->level > 0 || !tableau_prop[i]->hypo)) return 1;
    
    for (int i = 0; i < NB_GARE; i++)
        if (tableau_gare[i]->owner == p->number && !tableau_gare[i]->hypo) return 1;

    for (int i = 0; i < NB_COMPAGNIE; i++)
        if (tableau_comp[i]->owner == p->number && !tableau_comp[i]->hypo) return 1;

    return 0; // Rien à vendre, c'est la fin
}

// Boucle de sauvetage : force le joueur à vendre jusqu'à ce qu'il soit positif
void engine_resolve_debt(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    while (p->credit <= 0) {
        printf("\n  DETTE  : %d ECTS !", -p->credit);
        
        if (!engine_has_assets_left(p, tableau_prop, tableau_gare, tableau_comp)) {
            printf("\n[DÉFAITE] %s n'a plus aucune ressource...", p->name);
            engine_handle_bankruptcy(p, tableau_prop, tableau_gare, tableau_comp);
            return; // Sortie vers la faillite (crédit passera à -1)
        }

        printf("\nVous devez vous séparer de vos biens pour payer vos dettes.");
        printf("\nAppuyez sur Entrée pour ouvrir le menu de gestion...");
        vider_buffer(); getchar();

        // On lance le menu de gestion (Action 2: Hypothèque, Action 4: Vendre bâtiment)
        engine_manage_assets(p, tableau_prop, tableau_gare, tableau_comp, NULL);
    }
    printf("\n Dette réglée ! Nouveau solde : %d ECTS\n", p->credit);
}





/* TOUR COMPLET */
void engine_play_turn(Player *p, Player *all_players[], Property *tableau_prop[], 
                      Gares *tableau_gare[], Compagnies *tableau_comp[], 
                      Plateau *plateau, Prison *jail, int admin_mode, int nb_joueurs,
                      GameContext *ctx) {
    
    printf("\n\n##################################################");
    printf("\n# AU TOUR DE : %-33s #", p->name);
    printf("\n##################################################");

    /* Afficher l'événement actif en début de tour */
    if (ctx && evenements_est_actif(&ctx->evenement_mondial)) {
        const DefEvenement *def = evenements_def(ctx->evenement_mondial.id);
        printf("\n [ÉVÉNEMENT ACTIF] %s (%d round(s) restant(s))", def->nom, ctx->evenement_mondial.tours_restants);
    }

    // GESTION DE LA SORTIE DE PRISON
    if (is_in_jail(p, jail)) {
        /* Sortie forcée au 3ème tour */
        if (istreetour_in_jail(p, jail)) {
            printf("\n Cela fait 3 tours en prison ! Sortie forcée : 50 ECTS prélevés.");
            pay_to_get_out(p, jail);
            /* Le joueur est sorti, on continue normalement vers le lancer */
        } else {
            /* Vérifier si le joueur possède une carte Sortie de Prison */
            int a_carte_sortie = 0;
            if (ctx) {
                a_carte_sortie = joueur_possede_sortie_prison(ctx->pioche_chance.cartes,
                                                              ctx->pioche_chance.nb_cartes,
                                                              p->number);
            }
            printf("\n EN PRISON ! 1: Payer (50 ECTS) | 2: Tenter un double%s\nChoix : ",
                   a_carte_sortie ? " | 3: Utiliser carte Sortie de Prison" : "");
            int c; scanf("%d", &c);
            if (c == 1) {
                pay_to_get_out(p, jail);
            } else if (c == 3 && a_carte_sortie) {
                for (int i = 0; i < ctx->pioche_chance.nb_cartes; i++) {
                    Carte *carte = &ctx->pioche_chance.cartes[i];
                    if (carte->effet == CARTE_SORTIE_PRISON &&
                        carte->etat == ETAT_CARTE_EN_MAIN &&
                        carte->owner_number == p->number) {
                        utiliser_carte(carte);
                        remettre_carte_dans_paquet(carte);
                        break;
                    }
                }
                get_out_of_jail(p, jail);
                printf("\n Carte utilisée ! Vous êtes libre.");
            } else {
                int d_jail = 0;
                engine_roll_dice(admin_mode, &d_jail);
                if (d_jail > 0) {
                    printf("\n DOUBLE ! Vous êtes libre.");
                    get_out_of_jail(p, jail);
                } else {
                    printf("\n Pas de double. Vous restez en cellule.");
                    add_tour_jail(p, jail);
                    return;
                }
            }
        }
    }

    int d_count = 0;
    int rejouer = 0;

    do {
        rejouer = 0;
        int menu_boucle = 1;

        // MENU D'AVANT LANCER
        while (menu_boucle) {
            printf("\n\n--- MENU (%s - %d ECTS | Éco: %c) ---", p->name, p->credit, eco_grade_letter(p->eco_index));
            printf("\n1.  Lancer les dés%s", (d_count > 0 ? " (REJOUER)" : ""));
            printf("\n2.  Gestion des biens");
            printf("\n3.  Échanger");
            printf("\n4.  État");
            printf("\nVotre choix : ");
            int choix; scanf("%d", &choix);

            if (choix == 1) menu_boucle = 0;
            else if (choix == 2) engine_manage_assets(p, tableau_prop, tableau_gare, tableau_comp, ctx);
            else if (choix == 3) engine_handle_trade(p, all_players, nb_joueurs, tableau_prop, tableau_gare, tableau_comp);
            else if (choix == 4) display_player_status(p, tableau_prop, tableau_gare, tableau_comp, ctx);
        }

        // LANCER DES DÉS
        int des = engine_roll_dice(admin_mode, &d_count);

        // RÈGLE DES 3 DOUBLES
        if (d_count == 3) {
            printf("\n 3 DOUBLES D'AFFILÉE ! C'est suspect... Direction la PRISON !");
            p->position = 10;
            go_to_jail(p, jail);
            rejouer = 0;
        } else {
            player_move(p, des);
            engine_resolve_case(p, all_players, tableau_prop, tableau_gare, tableau_comp,
                                plateau, jail, des, ctx);
            if (is_in_jail(p, jail)) {
                rejouer = 0;
            } else if (d_count > 0) {
                printf("\n DOUBLE ! Vous avez le droit de rejouer.");
                rejouer = 1;
            }
        }

    } while (rejouer);

    // FIN DU TOUR
    calcul_moyenne_eco_index_player(p,tableau_prop, NB_PROPRIETE);

    /* Reset du flag bloque_construction pour ce joueur */
    if (ctx) {
        int idx = p->number - 1;
        if (idx >= 0 && idx < MAX_PLAYER)
            ctx->bloque_construction[idx] = 0;
    }

    printf("\n--- Fin du tour de %s ---\n", p->name);
}


void engine_handle_bankruptcy(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    printf("\n PROTOCOLE DE BANQUEROUTE : %s ", p->name);
    printf("\n %s est officiellement en faillite. Libération du patrimoine...", p->name);

    // Libération des Terrains
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i]->owner == p->number) {
            tableau_prop[i]->owner = 0;
            tableau_prop[i]->level = 0;
            tableau_prop[i]->type = 0;    
            tableau_prop[i]->hypo = 0;    
            printf("\n  - %s est de nouveau disponible.", tableau_prop[i]->name);
        }
    }

    // Libération des Gares
    for (int i = 0; i < NB_GARE; i++) {
        if (tableau_gare[i]->owner == p->number) {
            tableau_gare[i]->owner = 0;
            tableau_gare[i]->hypo = 0;
            printf("\n  - %s est de nouveau disponible.", tableau_gare[i]->name);
        }
    }

    // Libération des Compagnies
    for (int i = 0; i < NB_COMPAGNIE; i++) {
        if (tableau_comp[i]->owner == p->number) {
            tableau_comp[i]->owner = 0;
            tableau_comp[i]->hypo = 0;
            printf("\n  - %s est de nouveau disponible.", tableau_comp[i]->name);
        }
    }
 
    //  on met son crédit à -1 pour signifier qu'il est hors-jeu.
    p->credit = -1; 
    printf("\n\n [INFO] Tous les biens de %s ont été saisis.\n", p->name);
}


int engine_get_player_total_value(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    if (p->credit < 0) return -1; // Joueur éliminé

    int total = p->credit;

    // Valeur des terrains + constructions
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i]->owner == p->number) {
            total += tableau_prop[i]->value;
            // On ajoute le coût des bâtiments investis
            total += (tableau_prop[i]->level * tableau_prop[i]->construction_cost);
        }
    }
    // Valeur des gares
    for (int i = 0; i < NB_GARE; i++) {
        if (tableau_gare[i]->owner == p->number) {
            total += tableau_gare[i]->value;
        }
    }
    // Valeur des compagnies
    for (int i = 0; i < NB_COMPAGNIE; i++) {
        if (tableau_comp[i]->owner == p->number) {
            total += tableau_comp[i]->value;
        }
    }
    return total;
}


// Renvoie 1 si au moins un terrain du groupe de couleur possède des bâtiments
int engine_group_has_buildings(Property *tableau_prop[], Color couleur) {
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i]->color == couleur && tableau_prop[i]->level > 0) {
            return 1; // Un bâtiment a été trouvé dans le groupe
        }
    }
    return 0; // Le groupe est totalement nu
}