#ifndef BRIDGE_H
#define BRIDGE_H

#include "Struct.h"
#include "../Modele/player.h"
#include "../Modele/property.h"
#include "../Modele/prison.h"
#include "../Controlleur/plateau.h"
#include "../Controlleur/game_engine.h"

typedef struct{
    Player* joueurs[4];
    Property* tableau_prop[NB_PROPRIETE];
    Gares* tableau_gar[NB_GARE];
    Compagnies* tableau_comp[NB_COMPAGNIE];
    Plateau plateau;
    Prison* jail;           /* gestion de la prison */
    int admin_mode;         /* 0 dés aléatoires ou 1 mode admin */
    int dernier_de;
    int compteur_doubles;   /* compteur de doubles pour le tour en cours */
}ContexteJeu;

typedef struct{
    char name[50];
    int level;
    int canBuild;
    int prop_index;
}PropInfo;

/** Phase courante de la fenêtre de trade */
typedef enum {
    TRADE_PHASE_SELECT_TARGET = 0, /**< Choix du joueur cible */
    TRADE_PHASE_BUILD_OFFER,       /**< Constitution de l'offre (ce qu'on donne) */
    TRADE_PHASE_BUILD_DEMAND,      /**< Constitution de la demande (ce qu'on reçoit) */
    TRADE_PHASE_SET_CREDITS,       /**< Saisie de la compensation financière */
    TRADE_PHASE_RECAP,             /**< Récapitulatif avant validation */
    TRADE_PHASE_DONE               /**< Échange terminé (succès ou refus) */
} TradePhase;

/** Résumé d'un bien pour l'affichage dans la Vue (pas de pointeur Modèle exposé) */
typedef struct {
    char  name[50];
    int   value;
    int   type; /* 1=terrain, 2=gare, 3=compagnie */
    int   index; /* index dans le tableau global */
    int   selected; /* 1 si déjà dans l'offre */
} TradeAssetInfo;

/** État complet de l'échange courant, géré par le Bridge */
typedef struct {
    /* Participants */
    int joueur_actif_idx;  /**< Index dans ctx->joueurs de l'initiateur */
    int joueur_cible_idx;  /**< Index dans ctx->joueurs de la cible (-1 = non choisi) */

    /* Offres Modèle (opaques pour la Vue) */
    TradeOffer *off_actif; /**< Ce que l'initiateur donne */
    TradeOffer *off_cible; /**< Ce que la cible donne */

    /* Compensation financière */
    int credits_actif; /**< ECTS versés par l'initiateur */
    int credits_cible; /**< ECTS versés par la cible */

    /* Phase courante */
    TradePhase phase;

    /* Résultat (rempli en TRADE_PHASE_DONE) */
    int accepted;   /**< 1 si accepté, 0 si refusé, -1 si invalide */
    char message[128]; /**< Message de retour pour la Vue */
} TradeState;

int bridge_init(ContexteJeu* ctx, const EtatPartie* partie);
void bridge_free(ContexteJeu* ctx);

int bridge_roll_and_move(ContexteJeu* ctx, EtatPartie* partie);
int bridge_is_double(const ContexteJeu* ctx);
void bridge_reset_doubles(ContexteJeu* ctx);
void bridge_play_turn(ContexteJeu* ctx, EtatPartie* partie);
int bridge_next_turn(EtatPartie* partie);
void bridge_sync(const ContexteJeu* ctx, EtatPartie* partie);
int bridge_is_in_jail(const ContexteJeu* ctx, const EtatPartie* partie);

const char* bridge_get_case_joueur_actif(ContexteJeu* ctx, const EtatPartie* partie);

int bridge_get_current_prop_info(const ContexteJeu* ctx, const EtatPartie* partie, char name_out[50], int* prix_out, int* credits_joueur_out, int* owner_out);
int bridge_can_buy_current(const ContexteJeu* ctx, const EtatPartie* partie);
int bridge_get_owned_props(ContexteJeu* ctx, const EtatPartie* partie, PropInfo result[], int max);
int bridge_has_buildable(ContexteJeu* ctx, const EtatPartie* partie);
int bridge_buy_current(ContexteJeu* ctx, EtatPartie* partie);
int bridge_build(ContexteJeu* ctx, EtatPartie* partie, int build_type);

/** Initialise un TradeState pour un nouvel échange initié par le joueur actif. */
void bridge_trade_init(ContexteJeu* ctx, const EtatPartie* partie, TradeState* ts);

/** Libère les offres internes du TradeState. */
void bridge_trade_free(TradeState* ts);

/**
 * Remplit un tableau de TradeAssetInfo avec les biens échangeables du joueur idx.
 * @return Nombre de biens trouvés.
 */
int bridge_trade_get_assets(ContexteJeu* ctx, int joueur_idx, TradeAssetInfo out[], int max);

/**
 * Remplit un tableau des joueurs cibles possibles (tous sauf l'actif et les éliminés).
 * out_names : tableau de chaînes, out_idx : indices dans ctx->joueurs.
 * @return Nombre de cibles.
 */
int bridge_trade_get_targets(ContexteJeu* ctx, const EtatPartie* partie,
                             char out_names[][32], int out_idx[], int max);

/** Choisit le joueur cible (idx dans ctx->joueurs). Passe en TRADE_PHASE_BUILD_OFFER. */
void bridge_trade_select_target(ContexteJeu* ctx, TradeState* ts, int cible_idx);

/**
 * Bascule la sélection d'un bien dans l'offre active (off_actif ou off_cible selon la phase).
 * @param asset  Données du bien (depuis bridge_trade_get_assets).
 * @return 1 si ajouté, 0 si retiré, -1 si erreur.
 */
int bridge_trade_toggle_asset(ContexteJeu* ctx, TradeState* ts, const TradeAssetInfo* asset);

/** Fixe la compensation financière pour le joueur actif (phase BUILD_OFFER/SET_CREDITS). */
void bridge_trade_set_credits_actif(TradeState* ts, int amount);

/** Fixe la compensation financière pour le joueur cible. */
void bridge_trade_set_credits_cible(TradeState* ts, int amount);

/** Passe à la phase suivante. */
void bridge_trade_next_phase(ContexteJeu* ctx, TradeState* ts);

/**
 * Valide et exécute l'échange (à appeler depuis TRADE_PHASE_RECAP après confirmation).
 * Remplit ts->accepted et ts->message.
 * Passe en TRADE_PHASE_DONE.
 */
void bridge_trade_confirm(ContexteJeu* ctx, TradeState* ts, int cible_accepte);

/**
 * Vérifie si un bien est actuellement dans l'offre active (utile pour la Vue).
 * @return 1 si sélectionné, 0 sinon.
 */
int bridge_trade_is_selected(const TradeState* ts, const TradeAssetInfo* asset);

/**
 * Remplit des tableaux résumant ce que chaque joueur donne (pour le récapitulatif).
 * @return Nombre de biens dans l'offre indiquée.
 */
int bridge_trade_get_offer_summary(const TradeState* ts, int actif,
                                   TradeAssetInfo out[], int max, int* credits_out);

#endif