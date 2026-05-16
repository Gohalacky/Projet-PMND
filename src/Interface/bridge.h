#ifndef BRIDGE_H
#define BRIDGE_H

#include "Struct.h"
#include "../Model/player.h"
#include "../Model/property.h"
#include "../Model/prison.h"
#include "../Model/cartes.h"
#include "../Model/evenements.h"
#include "../Controller/plateau.h"
#include "../Controller/game_engine.h"
#include "../Controller/game_state.h"
#include "../Controller/engine_logic.h"

typedef struct{
    Player* joueurs[4];
    Property* tableau_prop[NB_PROPRIETE];
    Gares* tableau_gar[NB_GARE];
    Compagnies* tableau_comp[NB_COMPAGNIE];
    Plateau plateau;
    Prison* jail;
    int admin_mode;
    int dernier_de;
    int compteur_doubles;
    GameContext game_ctx;   /* pioches cartes + evenements + flags */
}ContexteJeu;

typedef struct{
    char name[50];
    int level;
    int canBuild;
    int prop_index;
}PropInfo;

/** Phase courante de la fenêtre de trade */
typedef enum {
    TRADE_PHASE_SELECT_TARGET = 0,
    TRADE_PHASE_BUILD_OFFER,
    TRADE_PHASE_BUILD_DEMAND,
    TRADE_PHASE_SET_CREDITS,
    TRADE_PHASE_RECAP,
    TRADE_PHASE_DONE
} TradePhase;

typedef struct {
    char  name[50];
    int   value;
    int   type;
    int   index;
    int   selected;
} TradeAssetInfo;

typedef struct {
    int joueur_actif_idx;
    int joueur_cible_idx;
    TradeOffer *off_actif;
    TradeOffer *off_cible;
    int credits_actif;
    int credits_cible;
    TradePhase phase;
    int accepted;
    char message[128];
} TradeState;

/** Informations sur la dernière carte piochée (pour la popup Vue) */
typedef struct {
    char titre[64];
    char description[128];
    int  paquet;    /* 0=Chance, 1=Communauté */
    int  valeur;
    int  effet;     /* TypeEffetCarte : 0=GAIN, 1=PERTE, 2=TAXE_ECO, ... */
} CarteInfo;

/** Informations sur l'événement actif (pour la Vue) */
typedef struct {
    int  actif;
    char nom[64];
    int  tours_restants;
    double mult_loyers;
    double mult_construction;
    int  interdit_construire;
} EvenementInfo;

/* --- Init / Free --- */
int  bridge_init(ContexteJeu* ctx, const EtatPartie* partie);
void bridge_free(ContexteJeu* ctx);

/* --- Dés / Mouvement --- */
int  bridge_roll_and_move(ContexteJeu* ctx, EtatPartie* partie);
int  bridge_is_double(const ContexteJeu* ctx);
void bridge_reset_doubles(ContexteJeu* ctx);

/* --- Tour --- */
/**
 * Résout la case du joueur actif via game_engine + cartes/evenements.
 * Si une carte est piochée, remplit *carte_out et retourne 1.
 * Retourne 0 sinon.
 */
int  bridge_play_turn(ContexteJeu* ctx, EtatPartie* partie, CarteInfo* carte_out);
int  bridge_next_turn(EtatPartie* partie);
int  bridge_check_bankruptcy_gameover(const EtatPartie* partie);
void bridge_sync(const ContexteJeu* ctx, EtatPartie* partie);

/**
 * Détermine le gagnant de la partie.
 * - Si tourMax > 0 : gagnant = joueur avec la plus grande valeur totale (credit + biens)
 * - Si faillite : dernier joueur encore actif (credit >= 0)
 * Remplit winner_name avec le nom du gagnant et winner_value avec sa valeur.
 * Retourne l'index du gagnant, ou -1 si indéterminé.
 */
int bridge_get_winner(const ContexteJeu* ctx, const EtatPartie* partie,
                      char winner_name[50], int* winner_value, int* winner_mode);

/* --- Evenements --- */
/** Remplit EvenementInfo depuis le GameContext. */
void bridge_get_evenement(const ContexteJeu* ctx, EvenementInfo* out);
/**
 * Tick de l'événement actif + tentative de déclenchement en fin de round.
 * À appeler après bridge_next_turn quand joueursActif revient à 0.
 */
void bridge_tick_evenement(ContexteJeu* ctx);

/* --- Infos plateau --- */
int  bridge_is_in_jail(const ContexteJeu* ctx, const EtatPartie* partie);
const char* bridge_get_case_joueur_actif(ContexteJeu* ctx, const EtatPartie* partie);

/* --- Propriétés --- */
int bridge_get_current_prop_info(const ContexteJeu* ctx, const EtatPartie* partie,char name_out[50], int* prix_out,int* credits_joueur_out, int* owner_out);
int bridge_can_buy_current(const ContexteJeu* ctx, const EtatPartie* partie);
int bridge_get_owned_props(ContexteJeu* ctx, const EtatPartie* partie,PropInfo result[], int max);
int bridge_has_buildable(ContexteJeu* ctx, const EtatPartie* partie);
int bridge_buy_current(ContexteJeu* ctx, EtatPartie* partie);
int bridge_build(ContexteJeu* ctx, EtatPartie* partie, int build_type);

/* --- Trade --- */
void bridge_trade_init(ContexteJeu* ctx, const EtatPartie* partie, TradeState* ts);
void bridge_trade_free(TradeState* ts);
int  bridge_trade_get_assets(ContexteJeu* ctx, int joueur_idx,TradeAssetInfo out[], int max);
int  bridge_trade_get_targets(ContexteJeu* ctx, const EtatPartie* partie,char out_names[][32], int out_idx[], int max);
void bridge_trade_select_target(ContexteJeu* ctx, TradeState* ts, int cible_idx);
int  bridge_trade_toggle_asset(ContexteJeu* ctx, TradeState* ts,const TradeAssetInfo* asset);
void bridge_trade_set_credits_actif(TradeState* ts, int amount);
void bridge_trade_set_credits_cible(TradeState* ts, int amount);
void bridge_trade_next_phase(ContexteJeu* ctx, TradeState* ts);
void bridge_trade_confirm(ContexteJeu* ctx, TradeState* ts, int cible_accepte);
int  bridge_trade_is_selected(const TradeState* ts, const TradeAssetInfo* asset);
int  bridge_trade_get_offer_summary(const TradeState* ts, int actif,TradeAssetInfo out[], int max,int* credits_out);

#endif