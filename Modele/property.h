/**
 * @file property.h
 * @brief Modèle de gestion des propriétés, gares et compagnies pour INSA'poly.
 * * Ce module gère l'état et la logique des différentes cases achetables du plateau.
 * Il inclut la gestion des loyers en crédits ECTS, les coûts de construction,
 * et l'impact des aménagements sur l'indice écologique.
 */

#ifndef PROPERTY_H
#define PROPERTY_H
#define NB_PROPRIETE 22
#define NB_COMPAGNIE 2
#define NB_GARE 4
#define NB_ACCISITION 28
#include "player.h"
#include "tool.h"

/**
 * @enum Color
 * @brief Définit les familles de couleurs pour les propriétés et les types spéciaux.
 */
typedef enum Color{
    MARRON = 0, 
    GRIS = 1, 
    ROSE = 2, 
    ORANGE = 3, 
    ROUGE = 4,
    JAUNE = 5, 
    VERT = 6, 
    BLEU = 7,
    COMP = 8,   /**< Identifiant pour les Compagnies de service */
    GAR = 9     /**< Identifiant pour les Gares */
} Color;

extern const char* color_names[];

/**
 * @enum logement
 * @brief Type d'aménagement écologique construit sur une propriété.
 */
typedef enum logement{
    Vide = 0,   /**< Aucun aménagement */
    Maison = 1, /**< Logement standard (Faible indice éco, loyer cher) */
    Social = 2, /**< Logement social (Indice éco moyen, loyer moyen) */
    Parc = 3    /**< Parc (Fort indice éco, loyer très faible) */
} logement ;

/**
 * @struct Property
 * @brief Structure représentant une case Propriété standard du plateau.
 */
typedef struct Property {
    char name[50];          /**< Nom de la propriété */
    Color color;            /**< Couleur ou famille de la propriété */   
    int owner;              /**< ID du propriétaire (0 si non possédé, 1 pour J1, 2 pour J2, etc.) */ 
    int value;              /**< Prix d'achat de la propriété en crédits ECTS */
    int construction_cost;  /**< Coût en crédits pour construire un aménagement (Maison/Social/Parc) */
    int renovation_cost;    /**< Coût en crédits pour effectuer une rénovation écologique */
    int rent_empty;         /**< Loyer de base (terrain nu) */
    int rent_full_color;    /**< Loyer exigé si toute la famille de couleur est possédée (x2) */
    int rent_maison[3];     /**< Loyers applicables pour 1, 2 ou 3 maisons */
    int rent_logement[3];   /**< Loyers applicables pour 1, 2 ou 3 logements sociaux */
    int rent_parc[3];       /**< Loyers applicables pour 1, 2 ou 3 parcs */
    int level;              /**< Niveau de développement (nombre de bâtiments actuels) */
    logement type;          /**< Type d'aménagement actuel (0: Vide, 1: Maison, 2: Social, 3: Parc) */        
    int indice;             /**< Indice écologique actuel de la propriété (impactant la note de A à F) */
} Property;

/**
 * @struct Compagnies
 * @brief Structure représentant une Compagnie (ex: Électricité/Eau).
 */
typedef struct Compagnies {
    char name[50];          /**< Nom de la compagnie */
    Color color;            /**< Couleur associée (COMP) */
    int owner;              /**< ID du propriétaire */
    int value;              /**< Prix d'achat en crédits ECTS */
    int rent[2];            /**< Multiplicateurs de loyer selon le nombre de compagnies possédées (1 ou 2) */
} Compagnies;

/**
 * @struct Gares
 * @brief Structure représentant une Gare (ou lieu de transit INSA).
 */
typedef struct Gares {
    char name[50];          /**< Nom de la gare */
    Color color;            /**< Couleur associée (GAR) */
    int owner;              /**< ID du propriétaire */
    int value;              /**< Prix d'achat en crédits ECTS */
    int rent[4];            /**< Loyers selon le nombre de gares possédées (1, 2, 3 ou 4) */
} Gares;


/* ========================================================================= */
/* PROTOTYPES DES FONCTIONS                         */
/* ========================================================================= */

/**
 * @brief Alloue et initialise une nouvelle propriété standard.
 * @return Un pointeur vers la Property initialisée, ou NULL en cas d'échec d'allocation.
 */
Property * init_Property(char *name, Color color, int owner, int value, int construction_cost, int rent_empty, int rent_maison[3], int rent_logement[3], int rent_parc[3], int level, logement type, int indice);

/**
 * @brief Alloue et initialise une nouvelle compagnie.
 * @return Un pointeur vers la structure Compagnies initialisée.
 */
Compagnies * init_Compagnies(char *name, Color color, int owner, int value, int rent[2]);

/**
 * @brief Alloue et initialise une nouvelle gare.
 * @return Un pointeur vers la structure Gares initialisée.
 */
Gares * init_Gares(char *name, Color color ,int owner, int value, int rent[4]);

/**
 * @brief Charge les données des propriétés depuis un fichier texte.
 * @param tableau Tableau de pointeurs vers les propriétés à remplir.
 * @param nomfichier Chemin vers le fichier de configuration.
 * @return Le nombre de propriétés chargées avec succès.
 */
int chargement_propriete_loyer(Property * tableau[NB_PROPRIETE], const char* nomfichier);

/**
 * @brief Charge les données des gares depuis un fichier texte.
 */
int chargement_GARE_loyer(Gares * tableau[NB_GARE], const char* nomfichier);

/**
 * @brief Charge les données des compagnies depuis un fichier texte.
 */
int chargement_COMPAGNIE_loyer(Compagnies * tableau[NB_COMPAGNIE], const char* nomfichier);

/**
 * @brief Vérifie si un joueur possède l'intégralité des propriétés d'une couleur donnée.
 * @param tableau Le tableau complet des propriétés du jeu.
 * @param p Le joueur à tester.
 * @param couleur La couleur à vérifier.
 * @return 1 si le joueur possède toute la couleur, 0 sinon.
 */
int full_color(Property * tableau[NB_PROPRIETE], Player *p, Color couleur);

/**
 * @brief Vérifie si un joueur possède toutes les compagnies (au nombre de 2).
 * @return 1 si le joueur possède toutes les compagnies, 0 sinon.
 */
int full_Comp(Compagnies * tableau[NB_COMPAGNIE], Player *p);

/**
 * @brief Calcule le nombre de gares possédées par un joueur spécifique.
 * @return Le nombre de gares (entre 0 et NB_GARE).
 */
int nb_Gare(Gares * tableau[NB_GARE], Player *p);

/**
 * @brief Applique le transfert de crédits ECTS d'un locataire vers un propriétaire pour une Propriété.
 * @note Gère automatiquement le cas de la banqueroute si le locataire n'a plus assez de crédits.
 * * @param tableau Le tableau des propriétés (nécessaire pour vérifier les monopoles de couleur).
 * @param prop La propriété sur laquelle le joueur est tombé.
 * @param loueur Le joueur qui doit payer.
 * @param owner Le joueur qui encaisse le loyer.
 */
void must_rent_Property(Property * tableau[NB_PROPRIETE], Property * prop, Player *loueur, Player *owner);

/**
 * @brief Applique le transfert de crédits pour une Compagnie.
 * @param d Le résultat du lancer de dés (utilisé comme multiplicateur de loyer).
 */
void must_rent_Compagnies(Compagnies * tableau[NB_COMPAGNIE], Compagnies * comp, Player *loueur, Player *owner, int d);

/**
 * @brief Applique le transfert de crédits pour une Gare.
 */
void must_rent_Gares(Gares * tableau[NB_GARE], Gares * g, Player *loueur, Player *owner);

/**
 * @brief Ajoute un niveau d'aménagement (construction) sur une propriété.
 * @pre Le joueur doit posséder toute la famille de couleur.
 * * @param tableau Le plateau de jeu.
 * @param prop La propriété à améliorer.
 * @param p Le joueur effectuant l'action.
 * @param type Le type d'aménagement choisi (1: Maison, 2: Social, 3: Parc).
 * @return 1 si la construction a réussi, 0 en cas d'erreur.
 */
int add_level(Property * tableau[NB_PROPRIETE], Property * prop, Player * p, int type);

/**
 * @brief Retire un niveau d'aménagement (destruction) et rembourse le joueur.
 * @return 1 si la destruction a réussi, 0 sinon.
 */
int remove_level(Property * tableau[NB_PROPRIETE], Property * prop, Player * p);

/**
 * @brief Vérifie la règle d'uniformité avant une construction ou destruction.
 * * Assure qu'il n'y a pas un écart de plus d'un niveau d'aménagement entre 
 * les propriétés d'une même famille de couleur.
 * * @param action 1 pour vérifier une construction, -1 pour vérifier une destruction.
 * @return 1 si l'action respecte l'uniformité, 0 si elle est illégale.
 */
int check_uniformity(Property * tableau[NB_PROPRIETE], Property * prop, int action);


#endif