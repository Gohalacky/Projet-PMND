#ifndef PROPERTY_H
#define PROPERTY_H

typedef enum {
    MARRON, BLEU_CIEL, ROSE, ORANGE, ROUGE, JAUNE, VERT, BLEU_FONCE
} Color;

typedef struct{
    char name[50];          /*Nome de la propriété*/
    Color color;            /*Couleur de la propriété*/   
    int owner;              /*Qui est le propriétaire 0 si non possédé et 1 pour joueur 1 et 2 pour joueur 2 et ainsi de suite*/ 
    int value;              /*Valeur pour acheter la propriété*/
    int construction_cost;  /*Cout pour construire un logement*/
    int renocation_cost;    /*cout pour faire une rénovation*/
    int rent_empty;         /*cout du loyer de base*/
    int rent_full_color;    /*cout du loyer si toute la couleur est possédé par le même propriétaire*/
    int rent_maison[3];     /*liste de la valeur des loyer pour 1,2 et 3 maison*/
    int rent_logement[3];   /*liste de la valeur des loyer pour 1,2 et 3 logement sociaux*/
    int rent_parc[3];       /*liste de la valeur des loyer pour 1,2 et 3 parc*/
    int level;              /*indique le nombre de maison, de logement sociaux ou de parc sur la propriété*/
    int type;               /*indique le type de logement construit sur la case 0 si rien, 1 pour maison , 2 pour logement sociaux, 3 pour parcs*/        
    int indice;             /*indique l'indice écologique de la propriété*/
} Property ;


#endif