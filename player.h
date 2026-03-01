#ifndef PLAYER_H
#define PLAYER_H
#include "property.h"

typedef enum {
    GRADE_A = 1,
    GRADE_B = 2,
    GRADE_C = 3,
    GRADE_D = 4,
    GRADE_E = 5,
    GRADE_F = 6
} EcoGrade;

typedef struct{
    char name[20];  /*Nom du joueur*/
    int number;     /*numéro du joueur*/
    int credit;     /*nombre de crédit du joueur*/
    int position;   /*Index de la case sur laquelle se trouve le joueur (0 à 39)*/
    EcoGrade eco_index;  /* Indice écologique personnel*/
}Player;

Player* init_player(const char* name, int number);
void calcul_moyenne_eco_index_player(Player *p, Property *board, int board_size);
void player_move(Player *p, int steps);
void player_add_credits(Player *p, int montant);
int player_remove_credits(Player *p, int montant);


#endif