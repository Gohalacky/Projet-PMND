#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include "game_engine_console.h"

/* AFFICHAGE DE L'ÉTAT DU JOUEUR */
void display_player_status(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    printf("\n==================================================");
    printf("\n   DOSSIER JOUEUR : %s", p->name);
    printf("\n==================================================");
    printf("\n Solde : %d ECTS |  Indice Éco : %d/6", p->credit, p->eco_index);
    printf("\n Position : Case n°%d", p->position);
    
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

/* ACHATS */
void engine_handle_purchase(Player *p, void *asset, int type) {
    int price = 0; char *name = "";
    if (type == 1) { price = ((Property*)asset)->value; name = ((Property*)asset)->name; }
    else if (type == 2) { price = ((Gares*)asset)->value; name = ((Gares*)asset)->name; }
    else if (type == 3) { price = ((Compagnies*)asset)->value; name = ((Compagnies*)asset)->name; }

    printf("\n [OPPORTUNITÉ] %s est libre pour %d ECTS.", name, price);
    if (p->credit >= price) {
        printf("\nVoulez-vous investir ? (y/n) : ");
        char c; scanf(" %c", &c);
        if (c == 'y' || c == 'Y') {
            player_remove_credits(p, price);
            if (type == 1) player_add_property(p, (Property*)asset);
            else if (type == 2) player_add_gar(p, (Gares*)asset);
            else if (type == 3) player_add_comp(p, (Compagnies*)asset);
            printf(" Acquisition réussie !\n");
        }
    } else printf("\n Fonds insuffisants.\n");
}

/* CONSTRUCTION (RÈGLE D'UNIFORMITÉ) */
void engine_manage_assets(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    printf("\n Gestion des biens ");
    printf("\n1. Construire / Améliorer");
    printf("\n2. Hypothéquer un bien (Gagner des ECTS)");
    printf("\n3. Racheter une hypothèque (Retrouver les loyers)");
    printf("\n4. Vendre un bâtiment (Remboursement)");
    printf("\n0. Retour");
    printf("\nChoix : ");
    
    int action; scanf("%d", &action);
    if (action == 0) return;

    // Construction 
    if (action == 1) {
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
                add_level(tableau_prop, sel, p, t);
                printf(" Chantier terminé !");
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
            if(tableau_prop[i]->owner == p->number && !tableau_prop[i]->hypo && tableau_prop[i]->level == 0) {
                printf("\n%d. [TERRAIN] %-15s (+%d ECTS)", global_count++, tableau_prop[i]->name, tableau_prop[i]->value/2);
                lp[cp++] = i;
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
                printf("\n%d. [TERRAIN] %-15s (%d ECTS)", global_count++, tableau_prop[i]->name, (int)((tableau_prop[i]->value/2)*1.1));
                lp[cp++] = i;
            }
        for(int i=0; i<NB_GARE; i++)
            if(tableau_gare[i]->owner == p->number && tableau_gare[i]->hypo) {
                printf("\n%d. [GARE]    %-15s (%d ECTS)", global_count++, tableau_gare[i]->name, (int)((tableau_gare[i]->value/2)*1.1));
                lg[cg++] = i;
            }

        for(int i=0; i<NB_COMPAGNIE; i++)
            if(tableau_comp[i]->owner == p->number && tableau_comp[i]->hypo) {
                printf("\n%d. [COMP]    %-15s (%d ECTS)", global_count++, tableau_comp[i]->name, (int)((tableau_comp[i]->value/2)*1.1));
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
    else if (action == 4) {
        printf("\n DÉCONSTRUCTION ");
        int list[NB_PROPRIETE], count = 0;
        for (int i = 0; i < NB_PROPRIETE; i++) {
            if (tableau_prop[i]->owner == p->number && tableau_prop[i]->level > 0) {
                if (check_uniformity(tableau_prop, tableau_prop[i], -1)) {
                    printf("\n%d. %s (Niveau %d -> %d | Remboursement: %d)", 
                           count + 1, tableau_prop[i]->name, tableau_prop[i]->level, tableau_prop[i]->level - 1, tableau_prop[i]->construction_cost);
                    list[count++] = i;
                }
            }
        }
        if (count == 0) { printf("\nAucun bâtiment à vendre.\n"); return; }
        int choix; printf("\nChoix : "); scanf("%d", &choix);
        if (choix > 0 && choix <= count) remove_level(tableau_prop, tableau_prop[list[choix-1]], p);
    }

}

/* RÉSOLUTION DE CASE */
void engine_resolve_case(Player *p, Player *all_players[], Property *tableau_prop[], 
                         Gares *tableau_gare[], Compagnies *tableau_comp[], 
                         Plateau *plateau, Prison *jail, int total_des) {
    (void)jail;
    Case *c = &plateau->cases[p->position];
    printf("\n Vous êtes sur : %s", c->nom_affichage);

    switch (c->type) {
        case CASE_TERRAIN: {
            Property *prop = (Property*)c->data;
            if (prop->owner == 0) engine_handle_purchase(p, prop, 1);
            else if (prop->owner != p->number) must_rent_Property(tableau_prop, prop, p, all_players[prop->owner-1]);
            break;
        }
        case CASE_GARE: {
            Gares *g = (Gares*)c->data;
            if (g->owner == 0) engine_handle_purchase(p, g, 2);
            else if (g->owner != p->number) must_rent_Gares(tableau_gare, g, p, all_players[g->owner-1]);
            break;
        }
        case CASE_COMPAGNIE: {
            Compagnies *cp = (Compagnies*)c->data;
            if (cp->owner == 0) engine_handle_purchase(p, cp, 3);
            else if (cp->owner != p->number) must_rent_Compagnies(tableau_comp, cp, p, all_players[cp->owner-1], total_des);
            break;
        }
        case CASE_TAXE: {
            // Si c'est la case 4 (Impôt), on retire 200, sinon 100 (Taxe de luxe)
            int montant = 0;
            if (p->position == 4) {
                montant= 200;
            }else{
                montant=100;
            } 
            printf("\n Vous devez payer l'administration : %d ECTS.", montant);
            player_remove_credits(p, montant);
            break;
        }
        case CASE_ALLEZ_PRISON: {
            printf("\n Direction la cellule ! Ne passez pas par la case départ.");
            p->position = 10;
            go_to_jail(p, jail);
            break;
        }
        default: printf("\n Rien à signaler."); break;
    }
}

/* ÉCHANGES */
void engine_handle_trade(Player *p_actif, Player *all_players[], int nb_joueurs) {
    printf("\n  NÉGOCIATIONS ");
    for (int i = 0; i < nb_joueurs; i++) {
        if (all_players[i]->number != p_actif->number) printf("\n%d. %s", i + 1, all_players[i]->name);
    }
    int t_idx; scanf("%d", &t_idx);
    if (t_idx < 1 || t_idx > nb_joueurs) return;
    Player *p_cible = all_players[t_idx-1];

    TradeOffer *off1 = init_empty_offer(p_actif);
    TradeOffer *off2 = init_empty_offer(p_cible);
    printf("ECTS offerts : "); int m1; scanf("%d", &m1); trade_set_credits(off1, m1);
    printf("ECTS demandés : "); int m2; scanf("%d", &m2); trade_set_credits(off2, m2);

    if (is_trade_valid(off1, off2)) {
        printf("Accepter ? (y/n) : "); char c; scanf(" %c", &c);
        if (c == 'y') execute_trade(off1, off2);
    }
    free(off1); free(off2);
}

/* TOUR COMPLET */
void engine_play_turn(Player *p, Player *all_players[], Property *tableau_prop[], 
                      Gares *tableau_gare[], Compagnies *tableau_comp[], 
                      Plateau *plateau, Prison *jail, int admin_mode, int nb_joueurs) {
    
    printf("\n\n##################################################");
    printf("\n# AU TOUR DE : %-33s #", p->name);
    printf("\n##################################################");

    // GESTION DE LA SORTIE DE PRISON
    if (is_in_jail(p, jail)) {
        printf("\n EN PRISON ! 1: Payer (50 ECTS) | 2: Tenter un double\nChoix : ");
        int c; scanf("%d", &c);
        if (c == 1) {
            pay_to_get_out(p, jail);
        } else {
            int d_jail = 0;
            engine_roll_dice(admin_mode, &d_jail);
            if (d_jail > 0) { // Si double (compteur > 0)
                printf("\n DOUBLE ! Vous êtes libre.");
                get_out_of_jail(p, jail);
            } else {
                printf("\n Pas de double. Vous restez en cellule.");
                add_tour_jail(p, jail);
                return; // Fin du tour si on échoue à sortir
            }
        }
    }

    int d_count = 0;   // Compteur de doubles pour ce tour
    int rejouer = 0;   // Flag pour la boucle de double

    do {
        rejouer = 0;
        int menu_boucle = 1;

        // MENU D'AVANT LANCER (On peut construire/échanger/Etat avant chaque lancer)
        while (menu_boucle) {
            printf("\n\n--- MENU (%s - %d ECTS) ---", p->name, p->credit);
            printf("\n1.  Lancer les dés%s", (d_count > 0 ? " (REJOUER)" : ""));
            printf("\n2.  Gestion des biens");
            printf("\n3.  Échanger");
            printf("\n4.  État");
            printf("\nVotre choix : ");
            int choix; scanf("%d", &choix);

            if (choix == 1) menu_boucle = 0;
            else if (choix == 2) engine_manage_assets(p, tableau_prop, tableau_gare, tableau_comp);
            else if (choix == 3) engine_handle_trade(p, all_players, nb_joueurs);
            else if (choix == 4) display_player_status(p, tableau_prop, tableau_gare, tableau_comp);
        }

        // LANCER DES DÉS
        int des = engine_roll_dice(admin_mode, &d_count);

        // VÉRIFICATION DE LA RÈGLE DES 3 DOUBLES
        if (d_count == 3) {
            printf("\n 3 DOUBLES D'AFFILÉE ! C'est suspect... Direction la PRISON !");
            // On déplace le joueur sur la case prison (position 10)
            p->position = 10; 
            go_to_jail(p, jail); 
            rejouer = 0; // Le tour s'arrête
        } else {
            // MOUVEMENT ET RÉSOLUTION
            player_move(p, des);
            engine_resolve_case(p, all_players, tableau_prop, tableau_gare, tableau_comp, plateau, jail, des);
            // Si après la résolution de la case, le joueur est en prison on annule le double
            if (is_in_jail(p, jail)) {
                rejouer = 0; 
            }
            // Si c'était un double, on prépare le tour suivant
            else if (d_count > 0) {
                printf("\n DOUBLE ! Vous avez le droit de rejouer.");
                rejouer = 1;
            }
        }

    } while (rejouer); // On recommence tant qu'il y a un double (et < 3)

    // FIN DU TOUR
    calcul_moyenne_eco_index_player(p, (Property*)tableau_prop, NB_PROPRIETE);
    printf("\n--- Fin du tour de %s ---\n", p->name);
}


