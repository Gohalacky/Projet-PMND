#ifndef PROPERTY_H
#define PROPERTY_H
#define NB_PROPRIETE 22
#define NB_COMPAGNIE 2
#define NB_GARE 4
#define NB_ACCISITION 28
#include "player.h"

typedef enum Color{
    MARRON = 0, 
    GRIS = 1, 
    ROSE = 2, 
    ORANGE = 3, 
    ROUGE = 4,
    JAUNE = 5, 
    VERT = 6, 
    BLEU = 7,
    COMP = 8,
    GAR = 9,
} Color;

extern const char* color_names[];

typedef enum logement{
    Vide = 0 ,
    Maison = 1 , 
    Social = 2 , 
    Parc = 3
} logement ;

typedef struct Property{
    char name[50];          /*Nome de la propriété*/
    Color color;            /*Couleur de la propriété*/   
    int owner;              /*Qui est le propriétaire 0 si non possédé et 1 pour joueur 1 et 2 pour joueur 2 et ainsi de suite*/ 
    int value;              /*Valeur pour acheter la propriété*/
    int construction_cost;  /*Cout pour construire un logement*/
    int renovation_cost;    /*cout pour faire une rénovation*/
    int rent_empty;         /*cout du loyer de base*/
    int rent_full_color;    /*cout du loyer si toute la couleur est possédé par le même propriétaire*/
    int rent_maison[3];     /*liste de la valeur des loyer pour 1,2 et 3 maison*/
    int rent_logement[3];   /*liste de la valeur des loyer pour 1,2 et 3 logement sociaux*/
    int rent_parc[3];       /*liste de la valeur des loyer pour 1,2 et 3 parc*/
    int level;              /*indique le nombre de maison, de logement sociaux ou de parc sur la propriété*/
    logement type;          /*indique le type de logement construit sur la case 0 si rien, 1 pour maison , 2 pour logement sociaux, 3 pour parcs*/        
    int indice;             /*indique l'indice écologique de la propriété*/
} Property ;

typedef struct Compagnies{
    char name[50];
    Color color;
    int owner;
    int value;
    int rent[2];
} Compagnies ;

typedef struct Gares{
    char name[50];
    Color color;
    int owner;
    int value;
    int rent[4];
} Gares ;



Property  * init_Property(char *name, 
            Color color, 
            int owner, 
            int value, int construction_cost, 
            int rent_empty,int rent_maison[3],int rent_logement[3],int rent_parc[3], 
            int level, logement type, int indice);
Compagnies * init_Compagnies(char *name, Color color, int owner, int value, int rent[2]);
Gares * init_Gares(char *name, Color color ,int owner, int value, int rent[4]);
int chargement_propriete_loyer(Property * tableau[NB_PROPRIETE], const char* nomfichier);
int full_color(Property * tableau[NB_PROPRIETE], Player *p, Color couleur);
void must_rent_Property(Property * tableau[NB_PROPRIETE], Property * prop, Player *p, Player *o);
void must_rent_Compagnies(Compagnies * tableau[NB_COMPAGNIE], Compagnies * comp, Player *p, Player *o);
void must_rent_Gares(Gares * tableau[NB_GARE], Gares * g, Player *p, Player *o);
void replace_underscore_with_space(char *str);
int add_level(Property * tableau[NB_PROPRIETE], Property * prop,Player * p, int type);
int remove_level(Property * tableau[NB_PROPRIETE], Property * prop,Player * p );
int check_uniformity(Property * tableau[NB_PROPRIETE], Property * prop, int action);

#endif