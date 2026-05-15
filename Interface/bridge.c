#include "bridge.h"
#include "Struct.h"
#include "../Modele/prison.h"
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

    ctx->jail = init_prison();
    if (!ctx->jail) {
        printf("Erreur bridge_init : init_prison() échoué\n");
        return -1;
    }

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
    if (ctx->jail) { free(ctx->jail); ctx->jail = NULL; }
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
    Player* p = ctx->joueurs[ja];
    if (!p) return 0;
    ctx->dernier_de = engine_roll_and_move(p, &ctx->plateau, &ctx->compteur_doubles);
    bridge_sync(ctx, partie);
    return ctx->dernier_de;
}

int bridge_is_double(const ContexteJeu* ctx) {
    return ctx->compteur_doubles > 0;
}

void bridge_reset_doubles(ContexteJeu* ctx) {
    ctx->compteur_doubles = 0;
}

void bridge_play_turn(ContexteJeu* ctx, EtatPartie* partie) {
    int ja = partie->joueursActif;
    Player* p = ctx->joueurs[ja];
    if (!p) return;
    engine_resolve_case(p, ctx->joueurs, ctx->tableau_prop, ctx->tableau_gar,ctx->tableau_comp, &ctx->plateau, ctx->jail, ctx->dernier_de);
    bridge_sync(ctx, partie);
}

int bridge_next_turn(EtatPartie* partie) {
    partie->joueursActif = (partie->joueursActif + 1) % partie->nbJoueurs;
    if (partie->joueursActif == 0) partie->tourActuel++;
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

const char* bridge_get_case_joueur_actif(ContexteJeu* ctx, const EtatPartie* partie) {
    return engine_get_case_joueur_actif(partie->joueursActif, ctx->joueurs, &ctx->plateau);
}

static void* _get_current_buyable(const ContexteJeu* ctx, const EtatPartie* partie, TypeCase* type_out) {
    int ja = partie->joueursActif;
    if (!ctx->joueurs[ja]) return NULL;
    int pos = ctx->joueurs[ja]->position;
    const Case* c = &ctx->plateau.cases[pos];
    if (c->type == CASE_TERRAIN || c->type == CASE_GARE || c->type == CASE_COMPAGNIE) {
        if (type_out) *type_out = c->type;
        return c->data;
    }
    return NULL;
}

static Property* _get_current_property(const ContexteJeu* ctx, const EtatPartie* partie) {
    TypeCase t;
    void* data = _get_current_buyable(ctx, partie, &t);
    if (!data || t != CASE_TERRAIN) return NULL;
    return (Property*)data;
}

int bridge_get_current_prop_info(const ContexteJeu* ctx, const EtatPartie* partie, char name_out[50], int* prix_out, int* credits_joueur_out, int* owner_out) {
    TypeCase t;
    void* data = _get_current_buyable(ctx, partie, &t);
    if (!data) return 0;

    if (t == CASE_TERRAIN) {
        Property* prop = (Property*)data;
        strncpy(name_out, prop->name, 49);
        *prix_out   = prop->value;
        *owner_out  = prop->owner;
    } else if (t == CASE_GARE) {
        Gares* g = (Gares*)data;
        strncpy(name_out, g->name, 49);
        *prix_out   = g->value;
        *owner_out  = g->owner;
    } else {
        Compagnies* comp = (Compagnies*)data;
        strncpy(name_out, comp->name, 49);
        *prix_out   = comp->value;
        *owner_out  = comp->owner;
    }
    name_out[49]        = '\0';
    *credits_joueur_out = ctx->joueurs[partie->joueursActif]->credit;
    return 1;
}

int bridge_can_buy_current(const ContexteJeu* ctx, const EtatPartie* partie) {
    TypeCase t;
    void* data = _get_current_buyable(ctx, partie, &t);
    if (!data) return 0;
    int ja = partie->joueursActif;
    if (!ctx->joueurs[ja]) return 0;

    int owner, value;
    if (t == CASE_TERRAIN) {
        owner = ((Property*)data)->owner;
        value = ((Property*)data)->value;
    } else if (t == CASE_GARE) {
        owner = ((Gares*)data)->owner;
        value = ((Gares*)data)->value;
    } else {
        owner = ((Compagnies*)data)->owner;
        value = ((Compagnies*)data)->value;
    }
    return owner == 0 && ctx->joueurs[ja]->credit >= value;
}

int bridge_get_owned_props(ContexteJeu* ctx, const EtatPartie* partie,
                            PropInfo result[], int max) {
    int ja = partie->joueursActif;
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
        result[count].canBuild   = (full_color(ctx->tableau_prop, joueur, p->color)
                                    && p->level < 3
                                    && check_uniformity(ctx->tableau_prop, p, 1));
        count++;
    }

    for (int i = 0; i < NB_GARE && count < max; i++) {
        Gares* g = ctx->tableau_gar[i];
        if (!g || g->owner != joueur->number) continue;
        strncpy(result[count].name, g->name, 49);
        result[count].name[49]   = '\0';
        result[count].level      = 0;   
        result[count].prop_index = i;
        result[count].canBuild   = 0;  
        count++;
    }
    for (int i = 0; i < NB_COMPAGNIE && count < max; i++) {
        Compagnies* c = ctx->tableau_comp[i];
        if (!c || c->owner != joueur->number) continue;
        strncpy(result[count].name, c->name, 49);
        result[count].name[49]   = '\0';
        result[count].level      = 0;
        result[count].prop_index = i;
        result[count].canBuild   = 0;
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
    TypeCase t;
    void* data = _get_current_buyable(ctx, partie, &t);
    int ja = partie->joueursActif;
    Player* joueur = ctx->joueurs[ja];
    if (!data || !joueur) return 0;

    if (t == CASE_TERRAIN) {
        Property* prop = (Property*)data;
        if (prop->owner != 0 || joueur->credit < prop->value) return 0;
        player_remove_credits(joueur, prop->value);
        player_add_property(joueur, prop);
    } else if (t == CASE_GARE) {
        Gares* g = (Gares*)data;
        if (g->owner != 0 || joueur->credit < g->value) return 0;
        player_remove_credits(joueur, g->value);
        player_add_gar(joueur, g);
    } else {
        Compagnies* comp = (Compagnies*)data;
        if (comp->owner != 0 || joueur->credit < comp->value) return 0;
        player_remove_credits(joueur, comp->value);
        player_add_comp(joueur, comp);
    }
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
int bridge_is_in_jail(const ContexteJeu* ctx, const EtatPartie* partie) {
    int ja = partie->joueursActif;
    Player* p = ctx->joueurs[ja];
    if (!p || !ctx->jail) return 0;
    return is_in_jail(p, ctx->jail);
}


void bridge_trade_init(ContexteJeu* ctx, const EtatPartie* partie, TradeState* ts) {
    if (!ctx || !partie || !ts) return;
    memset(ts, 0, sizeof(TradeState));
    ts->joueur_actif_idx = partie->joueursActif;
    ts->joueur_cible_idx = -1;
    ts->off_actif = engine_trade_init_offer(ctx->joueurs[partie->joueursActif]);
    ts->off_cible = NULL;
    ts->credits_actif = 0;
    ts->credits_cible = 0;
    ts->phase = TRADE_PHASE_SELECT_TARGET;
    ts->accepted = -1;
    ts->message[0] = '\0';
}

void bridge_trade_free(TradeState* ts) {
    if (!ts) return;
    engine_trade_free(ts->off_actif);
    engine_trade_free(ts->off_cible);
    ts->off_actif = NULL;
    ts->off_cible = NULL;
}

int bridge_trade_get_assets(ContexteJeu* ctx, int joueur_idx, TradeAssetInfo out[], int max) {
    if (!ctx || joueur_idx < 0 || joueur_idx >= 4 || !ctx->joueurs[joueur_idx]) return 0;
    Player* p = ctx->joueurs[joueur_idx];
    int count = 0;

    for (int i = 0; i < NB_PROPRIETE && count < max; i++) {
        Property* prop = ctx->tableau_prop[i];
        if (!prop || prop->owner != p->number || prop->level > 0 || prop->hypo) continue;
        strncpy(out[count].name, prop->name, 49);
        out[count].name[49] = '\0';
        out[count].value    = prop->value;
        out[count].type     = 1;
        out[count].index    = i;
        out[count].selected = 0;
        count++;
    }
    for (int i = 0; i < NB_GARE && count < max; i++) {
        Gares* g = ctx->tableau_gar[i];
        if (!g || g->owner != p->number || g->hypo) continue;
        strncpy(out[count].name, g->name, 49);
        out[count].name[49] = '\0';
        out[count].value    = g->value;
        out[count].type     = 2;
        out[count].index    = i;
        out[count].selected = 0;
        count++;
    }
    for (int i = 0; i < NB_COMPAGNIE && count < max; i++) {
        Compagnies* c = ctx->tableau_comp[i];
        if (!c || c->owner != p->number || c->hypo) continue;
        strncpy(out[count].name, c->name, 49);
        out[count].name[49] = '\0';
        out[count].value    = c->value;
        out[count].type     = 3;
        out[count].index    = i;
        out[count].selected = 0;
        count++;
    }
    return count;
}

int bridge_trade_get_targets(ContexteJeu* ctx, const EtatPartie* partie,
                              char out_names[][32], int out_idx[], int max) {
    if (!ctx || !partie) return 0;
    int count = 0;
    for (int i = 0; i < partie->nbJoueurs && count < max; i++) {
        if (i == partie->joueursActif) continue;
        Player* p = ctx->joueurs[i];
        if (!p || p->credit < 0) continue; 
        strncpy(out_names[count], p->name, 31);
        out_names[count][31] = '\0';
        out_idx[count] = i;
        count++;
    }
    return count;
}

void bridge_trade_select_target(ContexteJeu* ctx, TradeState* ts, int cible_idx) {
    if (!ctx || !ts) return;
    ts->joueur_cible_idx = cible_idx;
    ts->off_cible = engine_trade_init_offer(ctx->joueurs[cible_idx]);
    ts->phase = TRADE_PHASE_BUILD_OFFER;
}

int bridge_trade_toggle_asset(ContexteJeu* ctx, TradeState* ts, const TradeAssetInfo* asset) {
    if (!ctx || !ts || !asset) return -1;
    TradeOffer* off = (ts->phase == TRADE_PHASE_BUILD_OFFER) ? ts->off_actif : ts->off_cible;
    if (!off) return -1;

    if (asset->type == 1) {
        Property* prop = ctx->tableau_prop[asset->index];
        if (!prop) return -1;
        if (asset->selected)
            return engine_trade_remove_prop(off, prop) ? 0 : -1;
        else
            return engine_trade_add_prop(off, prop) ? 1 : -1;
    } else if (asset->type == 2) {
        Gares* g = ctx->tableau_gar[asset->index];
        if (!g) return -1;
        if (asset->selected)
            return engine_trade_remove_gare(off, g) ? 0 : -1;
        else
            return engine_trade_add_gare(off, g) ? 1 : -1;
    } else {
        Compagnies* c = ctx->tableau_comp[asset->index];
        if (!c) return -1;
        if (asset->selected)
            return engine_trade_remove_comp(off, c) ? 0 : -1;
        else
            return engine_trade_add_comp(off, c) ? 1 : -1;
    }
}

void bridge_trade_set_credits_actif(TradeState* ts, int amount) {
    if (!ts || !ts->off_actif) return;
    ts->credits_actif = amount;
    engine_trade_set_credits(ts->off_actif, amount);
}

void bridge_trade_set_credits_cible(TradeState* ts, int amount) {
    if (!ts || !ts->off_cible) return;
    ts->credits_cible = amount;
    engine_trade_set_credits(ts->off_cible, amount);
}

void bridge_trade_next_phase(ContexteJeu* ctx, TradeState* ts) {
    (void)ctx;
    if (!ts) return;
    if (ts->phase < TRADE_PHASE_DONE)
        ts->phase = (TradePhase)(ts->phase + 1);
}

void bridge_trade_confirm(ContexteJeu* ctx, TradeState* ts, int cible_accepte) {
    if (!ctx || !ts) return;
    if (!cible_accepte) {
        ts->accepted = 0;
        snprintf(ts->message, sizeof(ts->message), "L'echange a ete refuse.");
        ts->phase = TRADE_PHASE_DONE;
        return;
    }
    int valid = engine_trade_validate(ts->off_actif, ts->off_cible, ctx->tableau_prop);
    if (!valid) {
        ts->accepted = -1;
        snprintf(ts->message, sizeof(ts->message), "Echange invalide (fonds ou biens insuffisants).");
        ts->phase = TRADE_PHASE_DONE;
        return;
    }
    engine_trade_execute(ts->off_actif, ts->off_cible, ctx->tableau_prop);
    ts->accepted = 1;
    snprintf(ts->message, sizeof(ts->message), "Echange realise avec succes !");
    ts->phase = TRADE_PHASE_DONE;
}

int bridge_trade_is_selected(const TradeState* ts, const TradeAssetInfo* asset) {
    if (!ts || !asset) return 0;
    const TradeOffer* off = (ts->phase == TRADE_PHASE_BUILD_OFFER) ? ts->off_actif : ts->off_cible;
    if (!off) return 0;
    return asset->selected;
}

int bridge_trade_get_offer_summary(const TradeState* ts, int actif,
                                    TradeAssetInfo out[], int max, int* credits_out) {
    if (!ts || !credits_out) return 0;
    const TradeOffer* off = actif ? ts->off_actif : ts->off_cible;
    if (!off) { *credits_out = 0; return 0; }

    *credits_out = off->ects;
    int count = 0;

    for (int i = 0; i < off->nb_props && count < max; i++) {
        Property* p = off->props[i];
        if (!p) continue;
        strncpy(out[count].name, p->name, 49);
        out[count].name[49] = '\0';
        out[count].value = p->value;
        out[count].type  = 1;
        out[count].index = 0;
        out[count].selected = 1;
        count++;
    }
    for (int i = 0; i < off->nb_gares && count < max; i++) {
        Gares* g = off->gares[i];
        if (!g) continue;
        strncpy(out[count].name, g->name, 49);
        out[count].name[49] = '\0';
        out[count].value = g->value;
        out[count].type  = 2;
        out[count].index = 0;
        out[count].selected = 1;
        count++;
    }
    for (int i = 0; i < off->nb_comps && count < max; i++) {
        Compagnies* c = off->comps[i];
        if (!c) continue;
        strncpy(out[count].name, c->name, 49);
        out[count].name[49] = '\0';
        out[count].value = c->value;
        out[count].type  = 3;
        out[count].index = 0;
        out[count].selected = 1;
        count++;
    }
    return count;
}