/**
 * @file bridge.h
 * @brief Couche intermédiaire entre l'Interface SDL et le Modèle/Contrôleur.
 *
 * bridge.c encapsule tous les appels au Modèle (player.h, prison.h, cartes.h…)
 * et au Contrôleur (game_engine.h, engine_logic.h…).  La Vue ne manipule jamais
 * directement les types du Modèle : elle passe exclusivement par les fonctions
 * bridge_* et par les paramètres que bridge_sync() copie dans EtatPartie.
 */

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

/**
 * @brief Contexte de jeu global : regroupe tous les pointeurs du Modèle et du Contrôleur.
 *
 * Initialisé par bridge_init(), libéré par bridge_free().
 * La Vue ne lit que @c dernier_de et @c compteur_doubles ; tout le reste
 * est accédé via les fonctions bridge_*.
 */
typedef struct {
    Player*     joueurs[4];                    /**< Joueurs actifs. */
    Property*   tableau_prop[NB_PROPRIETE];    /**< Toutes les propriétés du plateau. */
    Gares*      tableau_gar[NB_GARE];          /**< Gares du plateau. */
    Compagnies* tableau_comp[NB_COMPAGNIE];    /**< Compagnies du plateau. */
    Plateau     plateau;                       /**< Plateau (cases ordonnées). */
    Prison*     jail;                          /**< Gestion de la prison. */
    int         admin_mode;                    /**< 1 = mode administrateur (debug dans la console). */
    int         dernier_de;                    /**< Résultat du dernier lancer (somme des deux dés). */
    int         compteur_doubles;              /**< Nombre de doubles consécutifs ce tour (0–3). */
    GameContext game_ctx;                      /**< Pioches cartes, événements et flags. */
} ContexteJeu;

/**
 * @brief Informations sur une propriété, utilisées par la fenêtre propriétés.
 */
typedef struct {
    char name[50];  /**< Nom de la propriété. */
    int  level;     /**< Niveau de construction actuel (0 = aucun). */
    int  canBuild;  /**< 1 si le joueur peut construire dessus. */
    int  prop_index;/**< Index dans tableau_prop[]. */
} PropInfo;

/**
 * @brief Phase courante de l'échange entre joueurs.
 */
typedef enum {
    TRADE_PHASE_SELECT_TARGET = 0, /**< Sélection du joueur cible. */
    TRADE_PHASE_BUILD_OFFER,       /**< Sélection des biens offerts par l'actif. */
    TRADE_PHASE_BUILD_DEMAND,      /**< Sélection des biens demandés à la cible. */
    TRADE_PHASE_SET_CREDITS,       /**< Saisie des compensations financières. */
    TRADE_PHASE_RECAP,             /**< Récapitulatif et acceptation/refus. */
    TRADE_PHASE_DONE               /**< Échange terminé (résultat affiché). */
} TradePhase;

/**
 * @brief Description d'un bien (propriété ou gare) dans une offre d'échange.
 */
typedef struct {
    char name[50]; /**< Nom du bien. */
    int  value;    /**< Valeur estimée. */
    int  type;     /**< 0 = propriété, 1 = gare, 2 = compagnie. */
    int  index;    /**< Index dans le tableau correspondant. */
    int  selected; /**< 1 si ce bien est inclus dans l'offre courante. */
} TradeAssetInfo;

/**
 * @brief État complet d'une transaction d'échange en cours.
 *
 * Initialisé par bridge_trade_init(), libéré par bridge_trade_free().
 * Modifié par les fonctions bridge_trade_* au fil des phases.
 */
typedef struct {
    int        joueur_actif_idx; /**< Index du joueur initiateur. */
    int        joueur_cible_idx; /**< Index du joueur cible (-1 si non sélectionné). */
    TradeOffer *off_actif;       /**< Offre du joueur actif (allouée dynamiquement). */
    TradeOffer *off_cible;       /**< Offre du joueur cible (allouée dynamiquement). */
    int        credits_actif;    /**< Compensation en crédits proposée par l'actif. */
    int        credits_cible;    /**< Compensation en crédits proposée par la cible. */
    TradePhase phase;            /**< Phase courante de l'échange. */
    int        accepted;         /**< 1 si la cible a accepté l'échange. */
    char       message[128];     /**< Message de résultat affiché en phase DONE. */
} TradeState;

/**
 * @brief Copie de la dernière carte piochée.
 */
typedef struct {
    char titre[64];        /**< Titre de la carte. */
    char description[128]; /**< Description de l'effet. */
    int  paquet;           /**< 0 = Chance, 1 = Communauté. */
    int  valeur;           /**< Valeur monétaire associée. */
    int  effet;            /**< TypeEffetCarte : 0=GAIN, 1=PERTE, 2=TAXE_ECO, … */
} CarteInfo;

/**
 * @brief Copie de l'événement global actif.
 */
typedef struct {
    int    actif;               /**< 1 si un événement est en cours. */
    char   nom[64];             /**< Nom de l'événement. */
    int    tours_restants;      /**< Tours avant fin de l'événement. */
    double mult_loyers;         /**< Multiplicateur de loyers (1.0 = normal). */
    double mult_construction;   /**< Multiplicateur de coût de construction. */
    int    interdit_construire; /**< 1 si la construction est bloquée. */
    char   cases_affectees[200];/**< Description des cases affectées par l'événement. */
} EvenementInfo;

/**
 * @brief Initialise le contexte de jeu complet.
 *
 * Charge les fichiers de données (propriétés, gares, compagnies), initialise
 * le plateau, crée les joueurs et la prison.
 *
 * @param ctx    Contexte à initialiser (contenu écrasé).
 * @param partie État de la partie fournissant nbJoueurs.
 * @return 0 en cas de succès, -1 si un fichier n'a pas pu être chargé.
 */
int bridge_init(ContexteJeu* ctx, const EtatPartie* partie);

/**
 * @brief Libère toutes les ressources allouées par bridge_init().
 * @param ctx Contexte à libérer.
 */
void bridge_free(ContexteJeu* ctx);

/**
 * @brief Lance les dés et déplace le joueur actif.
 *
 * Appelle engine_roll_dice() et player_move().  Stocke le résultat dans
 * ctx->dernier_de et met à jour ctx->compteur_doubles.
 *
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie (joueursActif utilisé).
 * @return Résultat du lancer (somme des deux dés, 2–12).
 */
int bridge_roll_and_move(ContexteJeu* ctx, EtatPartie* partie);

/**
 * @brief Indique si le dernier lancer était un double.
 * @param ctx Contexte de jeu.
 * @return 1 si double, 0 sinon.
 */
int bridge_is_double(const ContexteJeu* ctx);

/**
 * @brief Réinitialise le compteur de doubles consécutifs à 0.
 * @param ctx Contexte de jeu.
 */
void bridge_reset_doubles(ContexteJeu* ctx);

/**
 * @brief Résout la case du joueur actif (achat, loyer, carte, événement…).
 *
 * Si une carte est piochée, remplit @p carte_out avec ses informations et retourne 1.
 * Dans ce cas, bridge_sync() est appelé en interne.
 *
 * @param ctx       Contexte de jeu.
 * @param partie    État de la partie.
 * @param carte_out Structure à remplir si une carte est piochée (peut être NULL si non utilisée).
 * @return 1 si une carte a été piochée, 0 sinon.
 */
int bridge_play_turn(ContexteJeu* ctx, EtatPartie* partie, CarteInfo* carte_out);

/**
 * @brief Passe au joueur suivant et incrémente le tour si nécessaire.
 * @param partie État de la partie (joueursActif et tourActuel mis à jour).
 * @return 1 si la partie est terminée (tourMax atteint), 0 sinon.
 */
int bridge_next_turn(EtatPartie* partie);

/**
 * @brief Vérifie si tous les joueurs sauf un sont en faillite.
 * @param partie État de la partie.
 * @return 1 si la condition de fin par faillite est remplie, 0 sinon.
 */
int bridge_check_bankruptcy_gameover(const EtatPartie* partie);

/**
 * @brief Synchronise EtatPartie avec l'état courant du Modèle.
 *
 * Copie credits, noteEco, position et nom de chaque joueur depuis Player*
 * vers partie->joueurs[].  À appeler après toute modification du Modèle.
 *
 * @param ctx    Contexte de jeu (source, lecture seule).
 * @param partie État de la partie à mettre à jour.
 */
void bridge_sync(const ContexteJeu* ctx, EtatPartie* partie);

/**
 * @brief Détermine le gagnant de la partie.
 *
 * - Si tourMax > 0 : gagnant = joueur avec la plus grande valeur totale (crédits + valeur des biens).
 * - Si fin par faillite : dernier joueur avec crédits >= 0.
 *
 * @param ctx          Contexte de jeu.
 * @param partie       État de la partie.
 * @param winner_name  Buffer rempli avec le nom du gagnant.
 * @param winner_value Rempli avec la valeur totale du gagnant.
 * @param winner_mode  Rempli avec 0 (victoire aux tours) ou 1 (victoire par faillite).
 * @return Index du gagnant (0-based), ou -1 si indéterminé.
 */
int bridge_get_winner(const ContexteJeu* ctx, const EtatPartie* partie,
                      char winner_name[50], int* winner_value, int* winner_mode);

/**
 * @brief Remplit @p out avec l'état de l'événement global actif.
 * @param ctx Contexte de jeu.
 * @param out Structure à remplir (out->actif == 0 si aucun événement).
 */
void bridge_get_evenement(const ContexteJeu* ctx, EvenementInfo* out);

/**
 * @brief Avance le compteur de l'événement actif et tente d'en déclencher un nouveau.
 * À appeler après bridge_next_turn() quand joueursActif revient à 0 (fin de round).
 * @param ctx Contexte de jeu.
 */
void bridge_tick_evenement(ContexteJeu* ctx);

/**
 * @brief Indique si le joueur actif est actuellement en prison.
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @return 1 si en prison, 0 sinon.
 */
int bridge_is_in_jail(const ContexteJeu* ctx, const EtatPartie* partie);

/**
 * @brief Retourne le nom de la case sur laquelle se trouve le joueur actif.
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @return Pointeur vers une chaîne statique.
 */
const char* bridge_get_case_joueur_actif(ContexteJeu* ctx, const EtatPartie* partie);

/**
 * @brief Retourne le nombre de tours restants en prison avant sortie forcée.
 *
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @return Nombre de tours restants (0 = doit sortir ce tour, 3 = vient d'arriver).
 */
int bridge_jail_tours_restants(const ContexteJeu* ctx, const EtatPartie* partie);

/**
 * @brief Indique si le joueur actif possède une carte "Sortie de prison".
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @return 1 si une carte est disponible, 0 sinon.
 */
int bridge_jail_has_sortie_card(const ContexteJeu* ctx, const EtatPartie* partie);

/**
 * @brief Lance les dés pour tenter un double et sortir de prison.
 *
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @return 1 (libéré par double), 2 (libéré au 3e tour, sortie payante), 0 (raté).
 */
int bridge_jail_try_double(ContexteJeu* ctx, EtatPartie* partie);

/**
 * @brief Utilise la carte "Sortie de prison" pour libérer le joueur actif.
 *
 * Remet la carte dans le paquet et appelle get_out_of_jail().
 *
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @return 1 si la libération a réussi, 0 si aucune carte disponible.
 */
int bridge_jail_use_card(ContexteJeu* ctx, EtatPartie* partie);

/**
 * @brief Remplit les informations sur la propriété de la case courante du joueur actif.
 * @param ctx                   Contexte de jeu.
 * @param partie                État de la partie.
 * @param name_out              Buffer rempli avec le nom de la propriété.
 * @param prix_out              Prix d'achat.
 * @param credits_joueur_out    Crédits courants du joueur actif.
 * @param owner_out             Index du propriétaire actuel (-1 si libre).
 * @return 1 si la case est une propriété achetable, 0 sinon.
 */
int bridge_get_current_prop_info(const ContexteJeu* ctx, const EtatPartie* partie,
                                 char name_out[50], int* prix_out,
                                 int* credits_joueur_out, int* owner_out);

/**
 * @brief Indique si le joueur actif peut acheter la propriété de sa case courante.
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @return 1 si achetable (case = propriété libre + fonds suffisants), 0 sinon.
 */
int bridge_can_buy_current(const ContexteJeu* ctx, const EtatPartie* partie);

/**
 * @brief Remplit @p result avec la liste des propriétés possédées par le joueur actif.
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @param result Tableau à remplir.
 * @param max    Taille maximale du tableau.
 * @return Nombre de propriétés trouvées.
 */
int bridge_get_owned_props(ContexteJeu* ctx, const EtatPartie* partie,
                           PropInfo result[], int max);

/**
 * @brief Indique si le joueur actif possède au moins une propriété constructible.
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @return 1 si au moins une propriété peut recevoir un bâtiment, 0 sinon.
 */
int bridge_has_buildable(ContexteJeu* ctx, const EtatPartie* partie);

/**
 * @brief Achète la propriété de la case courante pour le joueur actif.
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @return 0 en cas de succès, -1 si l'achat est impossible.
 */
int bridge_buy_current(ContexteJeu* ctx, EtatPartie* partie);

/**
 * @brief Construit un bâtiment sur la propriété sélectionnée dans la fenêtre propriétés.
 * @param ctx        Contexte de jeu.
 * @param partie     État de la partie.
 * @param build_type Type de bâtiment : 0 = standard, 1 = social, 2 = parc.
 * @return 0 en cas de succès, -1 si la construction est impossible.
 */
int bridge_build(ContexteJeu* ctx, EtatPartie* partie, int build_type);

/**
 * @brief Initialise une transaction d'échange et alloue les offres.
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie (joueursActif = initiateur).
 * @param ts     TradeState à initialiser.
 */
void bridge_trade_init(ContexteJeu* ctx, const EtatPartie* partie, TradeState* ts);

/**
 * @brief Libère les offres allouées par bridge_trade_init().
 * @param ts TradeState à libérer.
 */
void bridge_trade_free(TradeState* ts);

/**
 * @brief Remplit @p out avec les biens d'un joueur disponibles pour l'échange.
 * @param ctx        Contexte de jeu.
 * @param joueur_idx Index du joueur.
 * @param out        Tableau à remplir.
 * @param max        Taille maximale du tableau.
 * @param ts         État de l'échange courant (pour marquer les biens déjà sélectionnés).
 * @return Nombre de biens trouvés.
 */
int bridge_trade_get_assets(ContexteJeu* ctx, int joueur_idx,
                            TradeAssetInfo out[], int max, const TradeState* ts);

/**
 * @brief Remplit les tableaux des joueurs disponibles comme cibles d'échange.
 * @param ctx       Contexte de jeu.
 * @param partie    État de la partie.
 * @param out_names Tableau de noms à remplir (out_names[i] = nom du joueur i).
 * @param out_idx   Tableau d'index à remplir.
 * @param max       Taille maximale des tableaux.
 * @return Nombre de cibles disponibles.
 */
int bridge_trade_get_targets(ContexteJeu* ctx, const EtatPartie* partie,
                             char out_names[][32], int out_idx[], int max);

/**
 * @brief Sélectionne le joueur cible de l'échange et passe à la phase suivante.
 * @param ctx       Contexte de jeu.
 * @param ts        État de l'échange.
 * @param cible_idx Index du joueur cible.
 */
void bridge_trade_select_target(ContexteJeu* ctx, TradeState* ts, int cible_idx);

/**
 * @brief Coche ou décoche un bien dans l'offre de la phase courante.
 * @param ctx   Contexte de jeu.
 * @param ts    État de l'échange.
 * @param asset Bien à basculer.
 * @return 1 si le bien est maintenant sélectionné, 0 s'il vient d'être décoché.
 */
int bridge_trade_toggle_asset(ContexteJeu* ctx, TradeState* ts, const TradeAssetInfo* asset);

/**
 * @brief Définit la compensation en crédits proposée par le joueur actif.
 * @param ts     État de l'échange.
 * @param amount Montant en crédits (>= 0).
 */
void bridge_trade_set_credits_actif(TradeState* ts, int amount);

/**
 * @brief Définit la compensation en crédits proposée par le joueur cible.
 * @param ts     État de l'échange.
 * @param amount Montant en crédits (>= 0).
 */
void bridge_trade_set_credits_cible(TradeState* ts, int amount);

/**
 * @brief Avance l'échange à la phase suivante.
 * @param ctx Contexte de jeu.
 * @param ts  État de l'échange.
 */
void bridge_trade_next_phase(ContexteJeu* ctx, TradeState* ts);

/**
 * @brief Finalise l'échange selon la réponse de la cible.
 *
 * Si @p cible_accepte == 1, transfère les biens et crédits entre les deux joueurs.
 * Met à jour ts->accepted et ts->message.
 *
 * @param ctx           Contexte de jeu.
 * @param ts            État de l'échange.
 * @param cible_accepte 1 = la cible accepte, 0 = la cible refuse.
 */
void bridge_trade_confirm(ContexteJeu* ctx, TradeState* ts, int cible_accepte);

/**
 * @brief Indique si un bien est actuellement sélectionné dans l'offre courante.
 * @param ts    État de l'échange (lecture seule).
 * @param asset Bien à tester.
 * @return 1 si sélectionné, 0 sinon.
 */
int bridge_trade_is_selected(const TradeState* ts, const TradeAssetInfo* asset);

/**
 * @brief Remplit @p out avec le résumé de l'offre d'un joueur (récapitulatif).
 * @param ts          État de l'échange (lecture seule).
 * @param actif       1 = résumé du joueur actif, 0 = résumé du joueur cible.
 * @param out         Tableau à remplir avec les biens de l'offre.
 * @param max         Taille maximale du tableau.
 * @param credits_out Rempli avec la compensation en crédits.
 * @return Nombre de biens dans l'offre.
 */
int bridge_trade_get_offer_summary(const TradeState* ts, int actif,
                                   TradeAssetInfo out[], int max, int* credits_out);

#endif
