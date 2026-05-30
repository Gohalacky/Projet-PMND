/**
 * @file game_engine.h
 * @brief Moteur de jeu INSA'poly pour l'interface graphique SDL2.
 *
 * Ce module expose les fonctions pures du moteur de jeu, sans entrées/sorties
 * console. Il est utilisé par la couche Interface (bridge.c, events.c) pour
 * piloter la logique de jeu à partir des événements SDL2 (clics, boutons).
 *
 * Les fonctions retournent des codes entiers (1 = succès, 0 = échec) afin
 * de permettre à l'Interface de réagir visuellement sans connaître les règles.
 *
 * @note Pour la version console, voir game_engine_console.h qui expose
 *       des fonctions équivalentes avec des interactions printf/scanf intégrées.
 */
#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "../Model/player.h"
#include "../Model/property.h"
#include "plateau.h"
#include "../Model/prison.h"
#include "../Model/trade.h"
#include "../Model/hypo.h"

/* ========================================================================= */
/* DÉS ET DÉPLACEMENT                                                        */
/* ========================================================================= */

/**
 * @brief Génère un lancer de deux dés aléatoires.
 *
 * @param compteur_doubles Pointeur sur le compteur de doubles consécutifs.
 *                         Incrémenté si double, remis à 0 sinon.
 * @return Somme des deux dés (entre 2 et 12).
 */
int engine_roll_dice(int *compteur_doubles);

/**
 * @brief Lance les dés et déplace le joueur sur le plateau.
 *
 * Combine engine_roll_dice() et player_move(). Gère le passage par la case
 * Départ (+200 ECTS) et retourne la somme des dés pour la résolution de case.
 *
 * @param p                Joueur à déplacer.
 * @param plateau          Plateau de jeu (pour les bornes de déplacement).
 * @param compteur_doubles Pointeur sur le compteur de doubles consécutifs.
 * @return Somme des deux dés utilisée pour le déplacement.
 */
int engine_roll_and_move(Player *p, const Plateau *plateau, int *compteur_doubles);

/* ========================================================================= */
/* TOUR DE JEU                                                                */
/* ========================================================================= */

/**
 * @brief Orchestre le tour complet d'un joueur (version SDL2, sans console).
 *
 * Gère la prison, le lancer de dés, le déplacement et la résolution de case.
 * Contrairement à la version console, cette fonction ne produit aucune sortie
 * texte : les résultats sont renvoyés à l'Interface via engine_resolve_case_logic().
 *
 * @param p             Joueur dont c'est le tour.
 * @param all_players   Tableau de tous les joueurs.
 * @param tableau_prop  Tableau des propriétés du plateau.
 * @param tableau_gare  Tableau des gares du plateau.
 * @param tableau_comp  Tableau des compagnies du plateau.
 * @param plateau       Structure du plateau de jeu.
 * @param jail          Structure de gestion de la prison.
 * @param nb_joueurs    Nombre total de joueurs dans la partie.
 */
void engine_play_turn(Player *p, Player *all_players[], Property *tableau_prop[],
                      Gares *tableau_gare[], Compagnies *tableau_comp[],
                      Plateau *plateau, Prison *jail, int nb_joueurs);

/* ========================================================================= */
/* RÉSOLUTION DE CASE                                                         */
/* ========================================================================= */

/**
 * @brief Résout les effets de la case d'arrivée (version avec affichage console).
 *
 * @param p             Joueur arrivant sur la case.
 * @param all_players   Tableau de tous les joueurs.
 * @param tableau_prop  Tableau des propriétés.
 * @param tableau_gare  Tableau des gares.
 * @param tableau_comp  Tableau des compagnies.
 * @param plateau       Structure du plateau de jeu.
 * @param jail          Structure de gestion de la prison.
 * @param total_des     Somme des dés (pour le loyer des compagnies).
 */
void engine_resolve_case(Player *p, Player *all_players[], Property *tableau_prop[],
                         Gares *tableau_gare[], Compagnies *tableau_comp[],
                         Plateau *plateau, Prison *jail, int total_des);

/**
 * @brief Résout les effets de la case d'arrivée et retourne un code d'état.
 *
 * Version sans effets de bord textuels, destinée à l'Interface SDL2.
 * Retourne un code entier que bridge.c utilise pour déclencher l'animation
 * ou la fenêtre pop-up appropriée.
 *
 * @param p             Joueur arrivant sur la case.
 * @param all_players   Tableau de tous les joueurs.
 * @param tableau_prop  Tableau des propriétés.
 * @param tableau_gare  Tableau des gares.
 * @param tableau_comp  Tableau des compagnies.
 * @param plateau       Structure du plateau de jeu.
 * @param jail          Structure de gestion de la prison.
 * @param total_des     Somme des dés (pour le loyer des compagnies).
 * @return Code d'état : 0 = rien, 1 = achat disponible, 2 = loyer payé,
 *         3 = carte piochée, 4 = prison, 5 = taxe.
 */
int engine_resolve_case_logic(Player *p, Player *all_players[], Property *tableau_prop[],
                              Gares *tableau_gare[], Compagnies *tableau_comp[],
                              Plateau *plateau, Prison *jail, int total_des);

/* ========================================================================= */
/* ACHAT / CONSTRUCTION / VENTE                                               */
/* ========================================================================= */

/**
 * @brief Achète un bien (terrain, gare ou compagnie) pour le joueur.
 *
 * Débite le prix d'achat et affecte le bien au joueur si les fonds sont suffisants.
 *
 * @param p     Joueur acheteur.
 * @param asset Pointeur vers le bien (Property, Gares ou Compagnies).
 * @param type  Type du bien : 1 = terrain, 2 = gare, 3 = compagnie.
 * @return 1 si l'achat a réussi, 0 si fonds insuffisants ou bien déjà possédé.
 */
int engine_buy_asset(Player *p, void *asset, int type);

/**
 * @brief Construit ou améliore un bâtiment sur une propriété.
 *
 * Vérifie la règle d'uniformité (tous les terrains du groupe au même niveau),
 * le monopole de couleur, et déduit le coût de construction.
 *
 * @param p             Joueur propriétaire.
 * @param tableau_prop  Tableau des propriétés (pour la règle d'uniformité).
 * @param prop          Propriété à améliorer.
 * @param type_batiment Type de bâtiment : 1 = Maison, 2 = Social, 3 = Parc.
 * @return 1 si la construction a réussi, 0 sinon.
 */
int engine_build_property(Player *p, Property *tableau_prop[], Property *prop, int type_batiment);

/**
 * @brief Vend un bâtiment d'une propriété et rembourse partiellement le joueur.
 *
 * Vérifie la règle d'uniformité avant de descendre d'un niveau.
 *
 * @param p            Joueur propriétaire.
 * @param tableau_prop Tableau des propriétés (pour la règle d'uniformité).
 * @param prop         Propriété dont on vend un bâtiment.
 * @return 1 si la vente a réussi, 0 sinon (uniformité non respectée ou niveau 0).
 */
int engine_sell_building(Player *p, Property *tableau_prop[], Property *prop);

/**
 * @brief Vérifie si un groupe de couleur possède des bâtiments.
 *
 * Utilisé pour la règle d'uniformité avant toute construction ou vente.
 *
 * @param tableau_prop Tableau des propriétés.
 * @param couleur      Couleur du groupe à vérifier.
 * @return 1 si au moins une propriété du groupe a un bâtiment (niveau > 0), 0 sinon.
 */
int engine_group_has_buildings(Property *tableau_prop[], Color couleur);

/* ========================================================================= */
/* HYPOTHÈQUES                                                                */
/* ========================================================================= */

/**
 * @brief Hypothèque une propriété et crédite 50% de sa valeur au joueur.
 *
 * Vérifie que la propriété n'a pas de bâtiments avant d'hypothéquer.
 *
 * @param p            Joueur propriétaire.
 * @param tableau_prop Tableau des propriétés (pour la règle d'uniformité du groupe).
 * @param prop         Propriété à hypothéquer.
 * @return 1 si l'hypothèque a réussi, 0 sinon.
 */
int engine_mortgage_prop(Player *p, Property *tableau_prop[], Property *prop);

/**
 * @brief Rachète l'hypothèque d'une propriété (coût : valeur × 1.1).
 *
 * @param p    Joueur propriétaire.
 * @param prop Propriété à désengager.
 * @return 1 si le rachat a réussi, 0 si fonds insuffisants.
 */
int engine_unmortgage_prop(Player *p, Property *prop);

/* ========================================================================= */
/* DETTE ET BANQUEROUTE                                                       */
/* ========================================================================= */

/**
 * @brief Vérifie si un joueur possède encore des biens mobilisables.
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
 * @brief Déclare un joueur en faillite et libère tout son patrimoine.
 *
 * Remet tous ses biens à la banque (owner = 0) et marque le joueur
 * comme éliminé (credit = -1).
 *
 * @param p            Joueur déclaré en faillite.
 * @param tableau_prop Tableau des propriétés.
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 */
void engine_handle_bankruptcy(Player *p, Property *tableau_prop[], Gares *tableau_gare[],
                              Compagnies *tableau_comp[]);

/**
 * @brief Vérifie si un joueur est en état de banqueroute irrémédiable.
 *
 * Un joueur est en banqueroute s'il a un crédit négatif ET aucun bien
 * à vendre ou hypothéquer pour couvrir sa dette.
 *
 * @param p            Joueur à vérifier.
 * @param tableau_prop Tableau des propriétés.
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 * @return 1 si en banqueroute, 0 sinon.
 */
int engine_check_bankruptcy_status(Player *p, Property *tableau_prop[], Gares *tableau_gare[],
                                   Compagnies *tableau_comp[]);

/* ========================================================================= */
/* VALORISATION                                                               */
/* ========================================================================= */

/**
 * @brief Calcule la valeur totale du patrimoine d'un joueur.
 *
 * Additionne le solde en ECTS et la valeur de revente de tous les biens
 * possédés (terrains, gares, compagnies, bâtiments inclus).
 *
 * @param p            Joueur à évaluer.
 * @param tableau_prop Tableau des propriétés.
 * @param tableau_gare Tableau des gares.
 * @param tableau_comp Tableau des compagnies.
 * @return Valeur totale en ECTS, ou -1 si le joueur est éliminé.
 */
int engine_get_player_total_value(Player *p, Property *tableau_prop[], Gares *tableau_gare[],
                                  Compagnies *tableau_comp[]);

/* ========================================================================= */
/* ÉCHANGES (TRADE)                                                           */
/* ========================================================================= */

/**
 * @brief Alloue une offre d'échange vide liée au joueur p.
 *
 * @note La mémoire allouée doit être libérée avec engine_trade_free().
 * @param p Joueur initiateur ou cible de l'échange.
 * @return Pointeur vers la TradeOffer allouée.
 */
TradeOffer *engine_trade_init_offer(Player *p);

/**
 * @brief Libère la mémoire d'une offre d'échange.
 *
 * @param off Offre à libérer.
 */
void engine_trade_free(TradeOffer *off);

/**
 * @brief Ajoute un terrain à une offre d'échange (niveau 0 requis).
 * @param off  Offre à modifier.
 * @param prop Terrain à ajouter (doit être sans bâtiment).
 * @return 1 si succès, 0 si le terrain a des bâtiments ou n'appartient pas au joueur.
 */
int engine_trade_add_prop(TradeOffer *off, Property *prop);

/**
 * @brief Retire un terrain d'une offre d'échange.
 * @param off  Offre à modifier.
 * @param prop Terrain à retirer.
 * @return 1 si succès, 0 si le terrain n'était pas dans l'offre.
 */
int engine_trade_remove_prop(TradeOffer *off, Property *prop);

/**
 * @brief Ajoute une gare à une offre d'échange.
 * @param off  Offre à modifier.
 * @param gare Gare à ajouter.
 * @return 1 si succès, 0 sinon.
 */
int engine_trade_add_gare(TradeOffer *off, Gares *gare);

/**
 * @brief Retire une gare d'une offre d'échange.
 * @param off  Offre à modifier.
 * @param gare Gare à retirer.
 * @return 1 si succès, 0 sinon.
 */
int engine_trade_remove_gare(TradeOffer *off, Gares *gare);

/**
 * @brief Ajoute une compagnie à une offre d'échange.
 * @param off  Offre à modifier.
 * @param comp Compagnie à ajouter.
 * @return 1 si succès, 0 sinon.
 */
int engine_trade_add_comp(TradeOffer *off, Compagnies *comp);

/**
 * @brief Retire une compagnie d'une offre d'échange.
 * @param off  Offre à modifier.
 * @param comp Compagnie à retirer.
 * @return 1 si succès, 0 sinon.
 */
int engine_trade_remove_comp(TradeOffer *off, Compagnies *comp);

/**
 * @brief Fixe la compensation financière associée à une offre d'échange.
 *
 * @param off    Offre à modifier.
 * @param amount Montant en ECTS (positif = le joueur de l'offre donne de l'argent).
 */
void engine_trade_set_credits(TradeOffer *off, int amount);

/**
 * @brief Vérifie la légalité d'un échange avant de l'exécuter.
 *
 * Contrôle que chaque bien appartient bien au bon joueur, qu'aucun terrain
 * échangé n'a de bâtiment, et que les deux joueurs ont les fonds nécessaires.
 *
 * @param off_actif   Offre du joueur initiateur.
 * @param off_cible   Offre du joueur cible.
 * @param tableau_prop Tableau des propriétés (pour la vérification des bâtiments).
 * @return 1 si l'échange est valide, 0 sinon.
 */
int engine_trade_validate(TradeOffer *off_actif, TradeOffer *off_cible,
                          Property *tableau_prop[]);

/**
 * @brief Exécute un échange validé entre deux joueurs.
 *
 * Transfère les biens et les ECTS entre les deux joueurs.
 *
 * @warning Appeler uniquement après engine_trade_validate() == 1
 *          ET confirmation du joueur cible.
 * @param off_actif    Offre du joueur initiateur.
 * @param off_cible    Offre du joueur cible.
 * @param tableau_prop Tableau des propriétés.
 */
void engine_trade_execute(TradeOffer *off_actif, TradeOffer *off_cible,
                          Property *tableau_prop[]);

/* ========================================================================= */
/* UTILITAIRES INTERFACE                                                      */
/* ========================================================================= */

/**
 * @brief Retourne le nom de la case sur laquelle se trouve le joueur actif.
 *
 * Utilisé par l'Interface SDL2 pour afficher le nom de la case dans les
 * popups et bulles d'information.
 *
 * @param joueurActif Index du joueur actif dans le tableau joueurs[].
 * @param joueurs     Tableau de tous les joueurs.
 * @param plateau     Structure du plateau de jeu.
 * @return Chaîne de caractères constante contenant le nom de la case.
 */
const char *engine_get_case_joueur_actif(int joueurActif, Player *joueurs[],
                                         const Plateau *plateau);

#endif