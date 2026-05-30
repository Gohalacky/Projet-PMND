/**
 * @file game_engine_console.h
 * @brief Moteur de jeu INSA'poly en mode console (entrées/sorties texte).
 *
 * Ce module orchestre le déroulement complet d'une partie en mode console :
 * lancer de dés, déplacement des joueurs, résolution des cases, gestion des
 * achats, enchères, constructions, hypothèques, échanges et faillites.
 *
 * Il constitue la couche Contrôleur entre le Modèle (règles métier) et
 * l'affichage texte (printf/scanf). L'interface graphique SDL2 utilise
 * game_engine.h, qui expose les mêmes fonctions sans les sorties console.
 *
 * @note Toutes les fonctions qui modifient l'état du jeu reçoivent un
 *       pointeur vers le GameContext pour accéder aux pioches et événements.
 */
#ifndef GAME_ENGINE_CONSOLE_H
#define GAME_ENGINE_CONSOLE_H

#include "../Model/player.h"
#include "../Model/property.h"
#include "plateau.h"
#include "game_state.h"
#include "../Model/prison.h"
#include "../Model/trade.h"
#include "../Model/hypo.h"

/**
 * @brief Simule un lancer de deux dés avec gestion du mode Admin.
 *
 * En mode normal, génère deux valeurs aléatoires entre 1 et 6.
 * En mode Admin, demande la valeur de chaque dé à l'utilisateur via scanf,
 * permettant de reproduire des scénarios précis pour les tests.
 *
 * @param admin_mode      Si 1, active la saisie manuelle des dés.
 * @param compteur_doubles Pointeur sur le compteur de doubles consécutifs du tour.
 *                         Incrémenté si les deux dés sont identiques, remis à 0 sinon.
 * @return Somme des deux dés (entre 2 et 12 en mode normal).
 */
int engine_roll_dice(int admin_mode, int *compteur_doubles);

/**
 * @brief Gère l'intégralité du tour d'un joueur en mode console.
 *
 * Enchaîne dans l'ordre :
 * -# Gestion de la prison (sortie forcée au 3e tour, paiement, double, carte).
 * -# Menu pré-lancer (construction, échange, état).
 * -# Lancer de dés et déplacement via player_move().
 * -# Résolution de la case d'arrivée via engine_resolve_case().
 * -# Gestion des doubles (rejouer jusqu'à 3 doubles maximum).
 * -# Mise à jour de l'indice écologique du joueur.
 * -# Reset du flag bloque_construction pour ce joueur.
 *
 * @param p             Joueur dont c'est le tour.
 * @param all_players   Tableau de tous les joueurs (pour loyers et enchères).
 * @param tableau_prop  Tableau des propriétés du plateau.
 * @param tableau_gare  Tableau des gares du plateau.
 * @param tableau_comp  Tableau des compagnies du plateau.
 * @param plateau       Structure du plateau de jeu.
 * @param jail          Structure de gestion de la prison.
 * @param admin_mode    Si 1, active la saisie manuelle des dés.
 * @param nb_joueurs    Nombre total de joueurs dans la partie.
 * @param ctx           Contexte global (pioches, événements, flags).
 */
void engine_play_turn(Player *p, Player *all_players[], Property *tableau_prop[],
                      Gares *tableau_gare[], Compagnies *tableau_comp[],
                      Plateau *plateau, Prison *jail, int admin_mode, int nb_joueurs,
                      GameContext *ctx);

/**
 * @brief Résout les effets de la case sur laquelle le joueur vient d'atterrir.
 *
 * Selon le type de la case (terrain, gare, compagnie, taxe, chance, communauté,
 * allez en prison…), déclenche l'action appropriée : achat, paiement de loyer
 * (avec multiplicateur événementiel si actif), pioche de carte, ou emprisonnement.
 * En cas de dette après résolution, appelle engine_resolve_debt().
 *
 * @param p             Joueur qui vient d'atterrir sur la case.
 * @param all_players   Tableau de tous les joueurs (propriétaires potentiels).
 * @param tableau_prop  Tableau des propriétés du plateau.
 * @param tableau_gare  Tableau des gares du plateau.
 * @param tableau_comp  Tableau des compagnies du plateau.
 * @param plateau       Structure du plateau de jeu.
 * @param jail          Structure de gestion de la prison.
 * @param total_des     Somme des deux dés (utilisée pour le loyer des compagnies).
 * @param ctx           Contexte global (événements actifs pour multiplicateurs).
 */
void engine_resolve_case(Player *p, Player *all_players[], Property *tableau_prop[],
                         Gares *tableau_gare[], Compagnies *tableau_comp[],
                         Plateau *plateau, Prison *jail, int total_des,
                         GameContext *ctx);

/**
 * @brief Affiche le dossier complet d'un joueur dans la console.
 *
 * Affiche le solde, l'indice écologique personnel et global, la position,
 * et le patrimoine immobilier détaillé (type de bâtiment, niveau, loyer actuel).
 *
 * @param p            Joueur dont on affiche l'état.
 * @param tableau_prop Tableau des propriétés (pour filtrer par propriétaire).
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 * @param ctx          Contexte global (pour l'indice éco global).
 */
void display_player_status(Player *p, Property *tableau_prop[], Gares *tableau_gare[],
                           Compagnies *tableau_comp[], GameContext *ctx);

/**
 * @brief Propose l'achat d'un bien libre au joueur, puis lance une enchère si refus.
 *
 * Si le joueur accepte et a les fonds, le bien lui est attribué au prix affiché.
 * Si le joueur refuse ou n'a pas les fonds, une vente aux enchères est lancée
 * entre tous les joueurs (y compris le refusant, conformément aux règles officielles).
 * L'offre minimale est vérifiée contre le solde du joueur via process_bid().
 *
 * @param p           Joueur qui a déclenché l'opportunité d'achat.
 * @param asset       Pointeur vers le bien (Property, Gares ou Compagnies).
 * @param type        Type du bien : 1 = terrain, 2 = gare, 3 = compagnie.
 * @param all_players Tableau de tous les joueurs pour les enchères.
 * @param nb_joueurs  Nombre total de joueurs.
 */
void engine_handle_purchase(Player *p, void *asset, int type,
                            Player *all_players[], int nb_joueurs);

/**
 * @brief Interface de négociation de terrain entre deux joueurs.
 *
 * Permet au joueur actif de proposer un échange (terrain contre terrain,
 * terrain contre ECTS, ou combinaison) à un autre joueur. Les deux parties
 * doivent confirmer avant que la transaction soit effectuée.
 *
 * @param p_actif      Joueur qui initie l'échange.
 * @param all_players  Tableau de tous les joueurs.
 * @param nb_joueurs   Nombre total de joueurs.
 * @param tableau_prop Tableau des propriétés.
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 */
void engine_handle_trade(Player *p_actif, Player *all_players[], int nb_joueurs,
                         Property *tableau_prop[], Gares *tableau_gare[],
                         Compagnies *tableau_comp[]);

/**
 * @brief Menu de gestion des biens d'un joueur (construction, hypothèque, vente).
 *
 * Propose quatre actions :
 * -# Construire / Améliorer (bloqué si flag bloque_construction ou événement actif).
 * -# Hypothéquer un bien (récupère 50% de la valeur).
 * -# Racheter une hypothèque (coût : valeur hypothèque × 1.1).
 * -# Vendre un bâtiment (remboursement partiel, règle d'uniformité vérifiée).
 *
 * @param p            Joueur qui gère ses biens.
 * @param tableau_prop Tableau des propriétés.
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 * @param ctx          Contexte global (flags de blocage et multiplicateurs d'événement).
 */
void engine_manage_assets(Player *p, Property *tableau_prop[], Gares *tableau_gare[],
                          Compagnies *tableau_comp[], GameContext *ctx);

/**
 * @brief Déclare un joueur en faillite et libère son patrimoine.
 *
 * Remet toutes ses propriétés, gares et compagnies à la banque (owner = 0),
 * et marque le joueur comme éliminé (credit = -1).
 *
 * @param p            Joueur déclaré en faillite.
 * @param tableau_prop Tableau des propriétés.
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 */
void engine_handle_bankruptcy(Player *p, Property *tableau_prop[], Gares *tableau_gare[],
                              Compagnies *tableau_comp[]);

/**
 * @brief Calcule la valeur totale du patrimoine d'un joueur.
 *
 * Additionne le solde en ECTS et la valeur de revente de tous les biens
 * possédés (terrains, gares, compagnies, valeur des constructions incluse).
 *
 * @param p            Joueur à évaluer.
 * @param tableau_prop Tableau des propriétés.
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 * @return Valeur totale en ECTS, ou -1 si le joueur est éliminé.
 */
int engine_get_player_total_value(Player *p, Property *tableau_prop[], Gares *tableau_gare[],
                                  Compagnies *tableau_comp[]);

/**
 * @brief Boucle de résolution de dette : force le joueur à vendre ou hypothéquer.
 *
 * Tant que le crédit du joueur est ≤ 0, ouvre le menu de gestion des biens.
 * Si le joueur n'a plus aucun bien à vendre ou hypothéquer, déclenche
 * engine_handle_bankruptcy().
 *
 * @param p            Joueur endetté.
 * @param tableau_prop Tableau des propriétés.
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 */
void engine_resolve_debt(Player *p, Property *tableau_prop[], Gares *tableau_gare[],
                         Compagnies *tableau_comp[]);

/**
 * @brief Vérifie si un joueur possède encore des biens mobilisables.
 *
 * Retourne 1 si le joueur possède au moins un terrain, une gare ou une compagnie
 * non hypothéquée ou un bâtiment vendable. Utilisé par engine_resolve_debt()
 * pour décider si la faillite doit être déclenchée.
 *
 * @param p            Joueur à vérifier.
 * @param tableau_prop Tableau des propriétés.
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 * @return 1 si le joueur a des ressources, 0 sinon.
 */
int engine_has_assets_left(Player *p, Property *tableau_prop[], Gares *tableau_gare[],
                           Compagnies *tableau_comp[]);

/**
 * @brief Vide le tampon d'entrée standard (stdin).
 *
 * Appelée après chaque scanf() pour consommer les caractères résiduels
 * (notamment le saut de ligne) et éviter les lectures parasites.
 */
void vider_buffer();

/**
 * @brief Vérifie si un groupe de couleur possède des bâtiments.
 *
 * Utilisé pour la règle d'uniformité : avant de vendre un bâtiment d'un terrain,
 * on vérifie que tous les terrains du groupe sont au même niveau ou au niveau
 * supérieur.
 *
 * @param tableau_prop Tableau des propriétés.
 * @param couleur      Couleur du groupe à vérifier.
 * @return 1 si au moins un terrain du groupe a un bâtiment (niveau > 0), 0 sinon.
 */
int engine_group_has_buildings(Property *tableau_prop[], Color couleur);

#endif
