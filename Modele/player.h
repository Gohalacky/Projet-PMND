/**
 * @file player.h
 * @brief Modèle de gestion des joueurs pour INSA'poly[cite: 33].
 *
 * Ce module gère l'état et les actions des étudiants de l'INSA (les joueurs).
 * Il centralise la gestion de leur position sur le plateau[cite: 49], 
 * de leur portefeuille de crédits ECTS [cite: 58], et de leur indice écologique personnel.
 */
#ifndef PLAYER_H
#define PLAYER_H

/* Déclarations anticipées (Forward declarations) pour éviter les inclusions circulaires */
struct Property;
typedef struct Property Property;
struct Gares;
typedef struct Gares Gares;
struct Compagnies;
typedef struct Compagnies Compagnies;

/**
 * @enum EcoGrade
 * @brief Représente la note écologique d'un joueur ou d'une propriété, allant de A à F.
 */
typedef enum {
    GRADE_A = 1, /**< Note A : Excellent (Indice de départ des joueurs)  */
    GRADE_B = 2, /**< Note B : Très bien */
    GRADE_C = 3, /**< Note C : Bien */
    GRADE_D = 4, /**< Note D : Passable */
    GRADE_E = 5, /**< Note E : Mauvais */
    GRADE_F = 6  /**< Note F : Critique */
} EcoGrade;

/**
 * @struct Player
 * @brief Structure représentant un joueur dans la partie.
 */
typedef struct{
    char name[20];       /**< Nom du joueur */
    int number;          /**< Numéro d'identifiant unique du joueur */
    int credit;          /**< Solde actuel en crédits ECTS  */
    int position;        /**< Index de la case actuelle sur le plateau (0 à 39) [cite: 49] */
    EcoGrade eco_index;  /**< Indice écologique personnel du joueur  */
}Player;

/* ========================================================================= */
/* PROTOTYPES DES FONCTIONS                                                  */
/* ========================================================================= */

/**
 * @brief Alloue et initialise un nouveau joueur.
 * * @note À la création, un joueur reçoit 1500 crédits ECTS, est placé sur la case 
 * Départ (0), et possède un indice écologique personnel initialisé à GRADE_A.
 * * @param name Chaîne de caractères représentant le nom du joueur (max 19 caractères).
 * @param number Identifiant numérique du joueur.
 * @return Un pointeur vers la structure Player allouée, ou NULL en cas d'échec mémoire.
 */
Player* init_player(const char* name, int number);

/**
 * @brief Met à jour l'indice écologique personnel du joueur.
 * * Cet indice est calculé en effectuant la moyenne des notes écologiques 
 * de toutes les propriétés que le joueur possède actuellement.
 * Si le joueur ne possède aucune propriété, son indice redevient GRADE_A.
 * * @param p Pointeur vers le joueur à mettre à jour.
 * @param board Tableau représentant le plateau de jeu (liste des propriétés).
 * @param board_size Taille du plateau (nombre de propriétés évaluables).
 */
void calcul_moyenne_eco_index_player(Player *p, Property *board, int board_size);

/**
 * @brief Déplace le joueur sur le plateau d'un certain nombre de cases.
 * * La fonction gère la boucle du plateau (modulo 40). Si le joueur dépasse 
 * la case 39 et revient au début, un passage par la case Départ est notifié.
 * * @param p Pointeur vers le joueur à déplacer.
 * @param steps Nombre de cases à avancer (généralement le résultat des dés)[cite: 55].
 */
void player_move(Player *p, int steps);

/**
 * @brief Ajoute un montant de crédits ECTS au solde du joueur.
 * * @param p Pointeur vers le joueur bénéficiaire.
 * @param montant Somme en crédits ECTS à ajouter. Doit être strictement positif.
 */
void player_add_credits(Player *p, int montant);

/**
 * @brief Retire un montant de crédits ECTS au solde du joueur.
 * * Gère le cas où le joueur ne possède pas assez de fonds pour payer sa dette.
 * * @param p Pointeur vers le joueur qui doit payer.
 * @param montant Somme en crédits ECTS à prélever.
 * @return 1 si le paiement a réussi, 0 si le joueur n'a pas assez d'argent (Banqueroute).
 */
int player_remove_credits(Player *p, int montant);

/**
 * @brief Assigne une Propriété standard au joueur.
 * * @param p Pointeur vers le joueur acheteur.
 * @param prop Pointeur vers la propriété acquise.
 */
void player_add_property(Player *p, Property *prop);

/**
 * @brief Assigne une Compagnie de service au joueur.
 * * @param p Pointeur vers le joueur acheteur.
 * @param prop Pointeur vers la compagnie acquise.
 */
void player_add_comp(Player *p, Compagnies *prop);

/**
 * @brief Assigne une Gare au joueur.
 * * @param p Pointeur vers le joueur acheteur.
 * @param prop Pointeur vers la gare acquise.
 */
void player_add_gar(Player *p, Gares *prop);

/**
 * @brief Retire la possession d'une Propriété à un joueur.
 * * @note Réinitialise également la propriété en retirant toutes les constructions
 * éventuelles (remise à l'état "Vide", niveau 0).
 * * @param p Pointeur vers l'ancien propriétaire.
 * @param prop Pointeur vers la propriété à libérer.
 */
void player_remove_property(Player *p, Property *prop);

/**
 * @brief Retire la possession d'une Compagnie à un joueur.
 * * @param p Pointeur vers l'ancien propriétaire.
 * @param prop Pointeur vers la compagnie à libérer.
 */
void player_remove_comp(Player *p, Compagnies *prop);

/**
 * @brief Retire la possession d'une Gare à un joueur.
 * * @param p Pointeur vers l'ancien propriétaire.
 * @param prop Pointeur vers la gare à libérer.
 */
void player_remove_gar(Player *p, Gares *prop);


#endif