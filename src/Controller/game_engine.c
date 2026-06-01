#include <stdio.h>
#include <stdlib.h>
#include "game_engine.h"
#include "plateau.h"




const char* engine_get_case_joueur_actif(int joueurActif,Player* joueurs[],const Plateau* plateau) {
    if (!joueurs[joueurActif] || !plateau) return "Inconnue";
    int pos = joueurs[joueurActif]->position;
    return plateau->cases[pos].nom_affichage;
}


/**  
 * Simule un lancer de 2 dés de 6 faces.
 */
int engine_roll_dice( int *compteur_doubles) {
    
    int de1 = rand() % 6 + 1;
    int de2 = rand() % 6 + 1;
    
    // Si c'est un double, on incrémente la valeur pointée. 
    // Sinon, on la remet à zéro.
    if (de1 == de2) {
        (*compteur_doubles)++; // Attention aux parenthèses, elles sont obligatoires 
        printf("Double de %d !\n",de1);
    } else {
        *compteur_doubles = 0;
        printf("%d + %d = %d\n",de1,de2,de1+de2);
    }
    
    return de1 + de2;
}


void engine_play_turn(Player *p, Player *all_players[], Property *tableau_prop[],Gares *tableau_gare[], Compagnies *tableau_comp[], Plateau *plateau, Prison *jail, int nb_joueurs) {
    (void)nb_joueurs;
    if (!p || !plateau) return;
  
    int compteur_doubles =0; // compteur de double pour la case prison = 0 au début de tour
    do{                            // on fait notre tour une premiere fois, le do while sert si le premier lancé était un double de rejoué et de refaire son tour
        int total_des = engine_roll_dice(&compteur_doubles);
        printf("[Dés] : Résultat = %d\n", total_des);
        // Cas : joueur en prison + double -> sort et se deplace, mais ne rejoue pas
        if((compteur_doubles == 1) && is_in_jail(p,jail)){
            get_out_of_jail(p,jail);
            player_move(p, total_des);
            engine_resolve_case(p, all_players, tableau_prop, tableau_gare, tableau_comp, plateau, jail, total_des);
            break; // Sort de prison sur double mais ne rejoue pas
        }
        // Cas : joueur en prison + pas de double -> reste ou paie
        if((compteur_doubles == 0) && is_in_jail(p,jail)){
            if(istreetour_in_jail(p,jail))
                pay_to_get_out(p,jail);
            else{
                printf("Voulez vous payez 50 pour sortir de prison ? (1:Oui/0:Non)\n");
                int rep;
                if (scanf("%d", &rep) == 1 && rep == 1){
                    pay_to_get_out(p,jail);
                }
            }
            // Si toujours en prison apres tentative, le joueur ne bouge pas ce tour
            if(is_in_jail(p, jail)){
                add_tour_jail(p, jail);
                return;
            }
        }
        // Cas : 3 doubles d'affilee -> va en prison
        if((compteur_doubles == 3) && (is_in_jail(p,jail) == 0)){
            printf("%s a fait 3 doubles a la suite !", p->name);
            go_to_jail(p,jail);
        }
        if(is_in_jail(p,jail) == 0){
            player_move(p, total_des);
        }
 
        engine_resolve_case(p, all_players, tableau_prop, tableau_gare, tableau_comp, plateau, jail,total_des);
    }while((compteur_doubles == 1) || (compteur_doubles == 2));
}

int engine_roll_and_move(Player* p, const Plateau* plateau, int * compteur_doubles) {
    if (!p || !plateau) return 0;
    int total_des = engine_roll_dice(compteur_doubles);
    player_move(p, total_des);
    return total_des;
}

void engine_resolve_case(Player *p, Player *all_players[], Property *tableau_prop[],Gares *tableau_gare[], Compagnies *tableau_comp[],Plateau *plateau, Prison *jail, int total_des) {
 
    if (!p || !plateau) return;
 
    Case *current_case = &plateau->cases[p->position];
 
    switch (current_case->type) {
 
        case CASE_DEPART:
            player_add_credits(p, 200);
            break;
        
        case CASE_ALLEZ_PRISON:
            go_to_jail(p,jail);
            break ;

        case CASE_TERRAIN: {
            Property *prop = (Property *)current_case->data;
            if (!prop) break;

            if (prop->owner != 0 && prop->owner != p->number) {
                Player *proprio = all_players[prop->owner - 1];
                must_rent_Property(tableau_prop, prop, p, proprio);
            }
            break;
        }
 
        case CASE_GARE: {
            Gares *gare = (Gares *)current_case->data;
            if (!gare) break;

            if (gare->owner != 0 && gare->owner != p->number) {
                Player *proprio = all_players[gare->owner - 1];
                must_rent_Gares(tableau_gare, gare, p, proprio);
            } 
            break;
        }
 
        case CASE_COMPAGNIE: {
            Compagnies *comp = (Compagnies *)current_case->data;
            if (!comp) break;

            if (comp->owner!=0 && comp->owner != p->number) {
                Player *proprio = all_players[comp->owner - 1];
                must_rent_Compagnies(tableau_comp, comp, p, proprio, total_des);
            }
            break;
        }

        case CASE_TAXE: {
            int montant = (p->position == 4) ? 200 : 100;
            player_remove_credits(p, montant);
            printf("%s paie %d ECTS de taxe.\n", p->name, montant);
            break;
        }

        default:
            break;
    }
 
    // Mise à jour écologique
    Property * board_static[NB_PROPRIETE];
    int valid_count = 0;
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i] != NULL)
            board_static[valid_count++] = (tableau_prop[i]);
    }
    if (valid_count > 0)
        calcul_moyenne_eco_index_player(p, board_static, valid_count);
}

int engine_buy_asset(Player *p, void *asset, int type) {
    int price = 0;
    if (type == 1) price = ((Property*)asset)->value;
    else if (type == 2) price = ((Gares*)asset)->value;
    else if (type == 3) price = ((Compagnies*)asset)->value;

    if (p->credit >= price) {
        player_remove_credits(p, price);
        if (type == 1) player_add_property(p, (Property*)asset);
        else if (type == 2) player_add_gar(p, (Gares*)asset);
        else if (type == 3) player_add_comp(p, (Compagnies*)asset);
        return 1;
    }
    return 0; 
}

int engine_build_property(Player *p, Property *tableau_prop[], Property *prop, int type_batiment) {
    if (prop->owner != p->number) return 0;
    if (!full_color(tableau_prop, p, prop->color)) return 0; // Pas de monopole
    if (prop->level >= 3) return 0; // Niveau max atteint
    if (!check_uniformity(tableau_prop, prop, 1)) return 0; // Règle d'uniformité non respectée

    // Vérification du type de bâtiment déjà existant dans le groupe
    int type_impose = Vide;
    for (int j = 0; j < NB_PROPRIETE; j++) {
        if (tableau_prop[j]->color == prop->color && tableau_prop[j]->type != Vide) {
            type_impose = tableau_prop[j]->type;
            break;
        }
    }

    if (type_impose != Vide && type_batiment != type_impose) return 0; // Doit correspondre au type imposé

    if (p->credit >= prop->construction_cost) {
        add_level(tableau_prop, prop, p, type_batiment);
        return 1;
    }
    return 0; // Fonds insuffisants
}

int engine_sell_building(Player *p, Property *tableau_prop[], Property *prop) {
    if (prop->owner != p->number || prop->level == 0) return 0;
    
    if (check_uniformity(tableau_prop, prop, -1)) {
        remove_level(tableau_prop, prop, p);
        return 1;
    }
    return 0;
}

// Utilitaires de vérification (Garde la logique intacte)
int engine_group_has_buildings(Property *tableau_prop[], Color couleur) {
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i]->color == couleur && tableau_prop[i]->level > 0) {
            return 1;
        }
    }
    return 0;
}

/* Fonctions d'hypothèque pour la GUI. 
   L'interface gère la sélection, ces fonctions appliquent la logique. */
int engine_mortgage_prop(Player *p, Property *tableau_prop[], Property *prop) {
    if (prop->owner == p->number && !prop->hypo && prop->level == 0 && !engine_group_has_buildings(tableau_prop, prop->color)) {
        hypotheque_PROP(prop, p);
        return 1;
    }
    return 0;
}

int engine_unmortgage_prop(Player *p, Property *prop) {
    int cout_rachat = (int)((prop->value / 2) * 1.1);
    if (prop->owner == p->number && prop->hypo && p->credit >= cout_rachat) {
        rachathypo_PROP(prop, p);
        return 1;
    }
    return 0;
}

int engine_resolve_case_logic(Player *p, Player *all_players[], Property *tableau_prop[], 
                              Gares *tableau_gare[], Compagnies *tableau_comp[], 
                              Plateau *plateau, Prison *jail, int total_des) {
    Case *c = &plateau->cases[p->position];

    switch (c->type) {
        case CASE_TERRAIN: {
            Property *prop = (Property*)c->data;
            if (prop->owner == 0) return 1; 
            if (prop->owner != p->number) {
                must_rent_Property(tableau_prop, prop, p, all_players[prop->owner-1]);
                return 2;
            }
            break;
        }
        case CASE_GARE: {
            Gares *g = (Gares*)c->data;
            if (g->owner == 0) return 1;
            if (g->owner != p->number) {
                must_rent_Gares(tableau_gare, g, p, all_players[g->owner-1]);
                return 2;
            }
            break;
        }
        case CASE_COMPAGNIE: {
            Compagnies *cp = (Compagnies*)c->data;
            if (cp->owner == 0) return 1;
            if (cp->owner != p->number) {
                must_rent_Compagnies(tableau_comp, cp, p, all_players[cp->owner-1], total_des);
                return 2;
            }
            break;
        }
        case CASE_TAXE: {
            int montant = (p->position == 4) ? 200 : 100;
            player_remove_credits(p, montant);
            return 3;
        }
        case CASE_ALLEZ_PRISON: {
            p->position = 10;
            go_to_jail(p, jail);
            return 4;
        }
        default: break;
    }
    return 0;
}



int engine_has_assets_left(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    for (int i = 0; i < NB_PROPRIETE; i++)
        if (tableau_prop[i]->owner == p->number && (tableau_prop[i]->level > 0 || !tableau_prop[i]->hypo)) return 1;
    for (int i = 0; i < NB_GARE; i++)
        if (tableau_gare[i]->owner == p->number && !tableau_gare[i]->hypo) return 1;
    for (int i = 0; i < NB_COMPAGNIE; i++)
        if (tableau_comp[i]->owner == p->number && !tableau_comp[i]->hypo) return 1;

    return 0;
}

void engine_handle_bankruptcy(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    // Libération de tous les biens
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i]->owner == p->number) {
            tableau_prop[i]->owner = 0;
            tableau_prop[i]->level = 0;
            tableau_prop[i]->type = 0;    
            tableau_prop[i]->hypo = 0;    
        }
    }
    for (int i = 0; i < NB_GARE; i++) {
        if (tableau_gare[i]->owner == p->number) { tableau_gare[i]->owner = 0; tableau_gare[i]->hypo = 0; }
    }
    for (int i = 0; i < NB_COMPAGNIE; i++) {
        if (tableau_comp[i]->owner == p->number) { tableau_comp[i]->owner = 0; tableau_comp[i]->hypo = 0; }
    }
    p->credit = -1; // Marque le joueur comme éliminé
}

// À appeler par la GUI si p->credit < 0.
// Renvoie 1 si le joueur est éliminé (Game Over pour lui), 0 s'il peut encore vendre des trucs.
int engine_check_bankruptcy_status(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    if (p->credit < 0) {
        if (!engine_has_assets_left(p, tableau_prop, tableau_gare, tableau_comp)) {
            engine_handle_bankruptcy(p, tableau_prop, tableau_gare, tableau_comp);
            return 1; // Le joueur a perdu
        }
        return 0; // Le joueur doit encore vendre (la GUI doit forcer l'ouverture du menu de gestion)
    }
    return 0; 
}



int engine_get_player_total_value(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]) {
    if (p->credit < 0) return -1;

    int total = p->credit;
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i]->owner == p->number) {
            total += tableau_prop[i]->value;
            total += (tableau_prop[i]->level * tableau_prop[i]->construction_cost);
        }
    }
    for (int i = 0; i < NB_GARE; i++) {
        if (tableau_gare[i]->owner == p->number) total += tableau_gare[i]->value;
    }
    for (int i = 0; i < NB_COMPAGNIE; i++) {
        if (tableau_comp[i]->owner == p->number) total += tableau_comp[i]->value;
    }
    return total;
}



TradeOffer *engine_trade_init_offer(Player *p) {
    return init_empty_offer(p);
}

void engine_trade_free(TradeOffer *off) {
    if (off) free(off);
}

int engine_trade_add_prop(TradeOffer *off, Property *prop) {
    if (!off || !prop) return 0;
    /* On ne peut échanger que des terrains sans bâtiment */
    if (prop->level > 0) return 0;
    return trade_add_property(off, prop);
}

int engine_trade_remove_prop(TradeOffer *off, Property *prop) {
    return trade_remove_property(off, prop);
}

int engine_trade_add_gare(TradeOffer *off, Gares *gare) {
    return trade_add_gare(off, gare);
}

int engine_trade_remove_gare(TradeOffer *off, Gares *gare) {
    return trade_remove_gare(off, gare);
}

int engine_trade_add_comp(TradeOffer *off, Compagnies *comp) {
    return trade_add_compagnie(off, comp);
}

int engine_trade_remove_comp(TradeOffer *off, Compagnies *comp) {
    return trade_remove_compagnie(off, comp);
}

void engine_trade_set_credits(TradeOffer *off, int amount) {
    trade_set_credits(off, amount);
}

int engine_trade_validate(TradeOffer *off_actif, TradeOffer *off_cible,
                          Property *tableau_prop[]) {
    return is_trade_valid(off_actif, off_cible, tableau_prop);
}

void engine_trade_execute(TradeOffer *off_actif, TradeOffer *off_cible,
                          Property *tableau_prop[]) {
    execute_trade(off_actif, off_cible, tableau_prop);
}