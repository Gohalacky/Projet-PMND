#include "bridge.h"
#include "Struct.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bridge_init(ContexteJeu* ctx, const EtatPartie* partie) {
    memset(ctx, 0, sizeof(ContexteJeu));

    int nbp = chargement_propriete_loyer(ctx->tableau_prop, "loyer_insapoly.txt");
    int nbg = chargement_GARE_loyer(ctx->tableau_gar,"gares.txt");
    int nbc = chargement_COMPAGNIE_loyer(ctx->tableau_comp, "compagnie.txt");

    if (nbp <= 0 || nbg <= 0 || nbc <= 0) {
        printf("Erreur bridge_init : chargement des fichiers échoué ""(prop=%d, gares=%d, comp=%d)\n", nbp, nbg, nbc);
        return -1;
    }

    init_plateau(&ctx->plateau, ctx->tableau_prop, ctx->tableau_gar, ctx->tableau_comp);

    for (int i = 0; i < partie->nbJoueurs; i++) {
        ctx->joueurs[i] = init_player(partie->joueurs[i].nom, i + 1);
        if (!ctx->joueurs[i]) {
            printf("Erreur bridge_init : init_player(%d) échoué\n", i);
            return -1;
        }
        ctx->joueurs[i]->credit = partie->joueurs[i].credits;
    }

    ctx->admin_mode = 0;
    return 0;
}

void bridge_free(ContexteJeu* ctx) {
    for (int i = 0; i < 4; i++) {
        if (ctx->joueurs[i]) { free(ctx->joueurs[i]); ctx->joueurs[i] = NULL; }
    }
    for (int i = 0; i < NB_PROPRIETE; i++){
        if (ctx->tableau_prop[i]){ 
            free(ctx->tableau_prop[i]); ctx->tableau_prop[i] = NULL; 
        }
    }
        
    for (int i = 0; i < NB_GARE; i++){
        if (ctx->tableau_gar[i]){
            free(ctx->tableau_gar[i]);  ctx->tableau_gar[i]  = NULL; 
        }
    }
        
    for (int i = 0; i < NB_COMPAGNIE; i++){
        if (ctx->tableau_comp[i]){ 
            free(ctx->tableau_comp[i]); ctx->tableau_comp[i] = NULL; 
        }
    }
        
}

int bridge_roll_and_move(ContexteJeu* ctx, EtatPartie* partie) {
    int ja = partie->joueursActif;
    int compteur_doubles = 0;
    Player* p = ctx->joueurs[ja];
    if (!p) return 0;
    ctx->dernier_de = engine_roll_and_move(p, &ctx->plateau,&compteur_doubles);
    bridge_sync(ctx, partie);
    return ctx->dernier_de;
}

void bridge_play_turn(ContexteJeu* ctx, EtatPartie* partie) {
    int ja = partie->joueursActif;
    Player* p = ctx->joueurs[ja];
    if (!p) return;
    engine_resolve_case(p, ctx->joueurs, ctx->tableau_prop, ctx->tableau_gar,ctx->tableau_comp, &ctx->plateau, ctx->dernier_de);
    bridge_sync(ctx, partie);
}

int bridge_next_turn(EtatPartie* partie) {
    partie->joueursActif = (partie->joueursActif + 1) % partie->nbJoueurs;
    if (partie->joueursActif == 0)partie->tourActuel++;
    if (partie->tourMax > 0 && partie->tourActuel > partie->tourMax) return 1;
    return 0;
}

void bridge_sync(const ContexteJeu* ctx, EtatPartie* partie) {
    for (int i = 0; i < partie->nbJoueurs; i++) {
        Player* p = ctx->joueurs[i];
        if (!p) continue;
        partie->joueurs[i].credits = p->credit;
        partie->joueurs[i].noteEco = (float)p->eco_index;
    }
}

const char* bridge_get_case_joueur_actif(const ContexteJeu* ctx, const EtatPartie* partie) {
    return engine_get_case_joueur_actif(partie->joueursActif,(Player**)ctx->joueurs,&ctx->plateau);
}

static Property* _get_current_property(const ContexteJeu* ctx, const EtatPartie* partie) {
    int ja = partie->joueursActif;
    if (!ctx->joueurs[ja]) return NULL;
    int pos = ctx->joueurs[ja]->position;
    const Case* c = &ctx->plateau.cases[pos];
    if (c->type != CASE_TERRAIN) return NULL;
    return (Property*)c->data;
}

int bridge_get_current_prop_info(const ContexteJeu* ctx, const EtatPartie* partie,char name_out[50], int* prix_out,int* credits_joueur_out, int* owner_out) {
    Property* prop = _get_current_property(ctx, partie);
    if (!prop) return 0;

    strncpy(name_out, prop->name, 49);
    name_out[49]      = '\0';
    *prix_out         = prop->value;
    *credits_joueur_out = ctx->joueurs[partie->joueursActif]->credit;
    *owner_out        = prop->owner;
    return 1;
}

int bridge_can_buy_current(const ContexteJeu* ctx, const EtatPartie* partie) {
    Property* prop = _get_current_property(ctx, partie);
    if (!prop || prop->owner != 0) return 0;
    int ja = partie->joueursActif;
    return ctx->joueurs[ja] && ctx->joueurs[ja]->credit >= prop->value;
}

int bridge_get_owned_props(ContexteJeu* ctx, const EtatPartie* partie,PropInfo result[], int max) {
    int ja  = partie->joueursActif;
    Player* joueur = ctx->joueurs[ja];
    if (!joueur) return 0;

    int count = 0;
    for (int i = 0; i < NB_PROPRIETE && count < max; i++) {
        Property* p = ctx->tableau_prop[i];
        if (!p || p->owner != joueur->number) continue;

        strncpy(result[count].name, p->name, 49);
        result[count].name[49]   = '\0';
        result[count].level      = p->level;
        result[count].prop_index = i;
        result[count].canBuild   = (full_color(ctx->tableau_prop, joueur, p->color) && p->level < 3 && check_uniformity(ctx->tableau_prop, p, 1));
        count++;
    }
    return count;
}

int bridge_has_buildable(ContexteJeu* ctx, const EtatPartie* partie) {
    int ja = partie->joueursActif;
    Player* joueur = ctx->joueurs[ja];
    if (!joueur) return 0;

    for (int i = 0; i < NB_PROPRIETE; i++) {
        Property* p = ctx->tableau_prop[i];
        if (!p || p->owner != joueur->number) continue;
        if (full_color(ctx->tableau_prop, joueur, p->color)
            && p->level < 3
            && check_uniformity(ctx->tableau_prop, p, 1))
            return 1;
    }
    return 0;
}

int bridge_buy_current(ContexteJeu* ctx, EtatPartie* partie) {
    Property* prop = _get_current_property(ctx, partie);
    int ja = partie->joueursActif;
    Player* joueur = ctx->joueurs[ja];

    if (!prop || !joueur || prop->owner != 0 || joueur->credit < prop->value)
        return 0;

    player_remove_credits(joueur, prop->value);
    player_add_property(joueur, prop);
    bridge_sync(ctx, partie);
    return 1;
}

int bridge_build(ContexteJeu* ctx, EtatPartie* partie, int build_type) {
    int ja = partie->joueursActif;
    Player* joueur = ctx->joueurs[ja];
    if (!joueur || build_type < 1 || build_type > 3) return 0;

    for (int i = 0; i < NB_PROPRIETE; i++) {
        Property* p = ctx->tableau_prop[i];
        if (!p || p->owner != joueur->number) continue;
        if (full_color(ctx->tableau_prop, joueur, p->color) && p->level < 3 && check_uniformity(ctx->tableau_prop, p, 1) && joueur->credit >= p->construction_cost) {
            add_level(ctx->tableau_prop, p, joueur, build_type);
            bridge_sync(ctx, partie);
            return 1;
        }
    }
    return 0;
}