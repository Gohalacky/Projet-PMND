#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "property.h"

/*récupère le nom des Couleurs avec le typedef enum Color*/
const char* color_names[] = {
    "Marron",
    "Gris",
    "Rose",
    "Orange",
    "Rouge",
    "Jaune",
    "Vert",
    "Bleu"
};

void replace_underscore_with_space(char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '_') {
            str[i] = ' ';
        }
    }
}

Property * init_Property(char *nom, 
                        Color couleur, 
                        int proprio, 
                        int valeur,
                        int construction_cout,
                        int loyer_vide,
                        int loyer_maison[3],int loyer_logement[3],int loyer_parc[3],
                        int  niveau, 
                        logement type_logement,
                        int note){
    
    /*Property * p = (Property *)malloc(sizeof(Property)); ça fonctionne mieux avec le calloc*/
    Property * p = (Property *)calloc(1, sizeof(Property));
    
    if(p==NULL){
        printf("ERROR init_Property, erreur dans l'allocation de la mémoire \n");
        return NULL;
    }
    
    strncpy(p->name, nom, 49);
    p->name[49] = '\0'; /*on s'assure que la chaine de caractère est fini*/
    p->color = couleur;
    p->owner = proprio;
    p->value = valeur;
    p->construction_cost = construction_cout;
    p->rent_empty = loyer_vide;
    p->rent_full_color = 2 * loyer_vide ;
    p->renovation_cost = construction_cout;

    for (int i = 0; i < 3; i++) {
        p->rent_maison[i] = loyer_maison[i];
        p->rent_logement[i] = loyer_logement[i]; /*on copie les tableaux des valeurs*/
        p->rent_parc[i] = loyer_parc[i];
    }

    p->level = niveau;
    p->type = type_logement;
    p->indice = note;

    return p;

}

Compagnies * init_Compagnies(char *nom, Color couleur, int proprio, int valeur, int loyer[2]){
    Compagnies * g = (Compagnies *)calloc(1,sizeof(Compagnies));
    if(g==NULL){
        printf("ERROR init_Compagnies, erreur dans l'allocation de la mémoire \n");
        return NULL;
    }
    strncpy(g->name, nom, 49);
    g->name[49] = '\0'; /*on s'assure que la chaine de caractère est fini*/
    g->color = couleur;
    g->owner = proprio;
    g->value = valeur;
    for(int i = 0; i < 2; i++){
        g->rent[i]=loyer[i];
    }
    return g;
}
Gares * init_Gares(char *nom, Color couleur ,int proprio, int valeur, int loyer[4]){
    Gares * g = (Gares *)calloc(1,sizeof(Gares));
    if(g==NULL){
        printf("ERROR init_Gares, erreur dans l'allocation de la mémoire \n");
        return NULL;
    }
    strncpy(g->name, nom, 49);
    g->name[49] = '\0'; /*on s'assure que la chaine de caractère est fini*/
    g->color = couleur;
    g->owner = proprio;
    g->value = valeur;
    for(int i = 0; i < 4; i++){
        g->rent[i]=loyer[i];
    }
    return g;
}


int chargement_propriete_loyer(Property * plateau[NB_PROPRIETE], const char* nomfichier){
    FILE* file = fopen(nomfichier,"r");
    if(file == NULL){
        printf("ERROR chargement_propriete_loyer, impossible d'ouvrir le fichier");
        return 0;
    }
    int color_val; // Pour lire le chiffre de la couleur (0, 1, 2...)
    char nom_str[50];
    int val, c_const, c_reno, l_vide, l_full;
    int l_m[3], l_s[3], l_p[3];

    int i = 0;
    while (i < NB_PROPRIETE && 
           fscanf(file, " %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
                  &color_val, nom_str, &val, &c_const, &c_reno, &l_vide, &l_full,
                  &l_m[0], &l_m[1], &l_m[2], 
                  &l_s[0], &l_s[1], &l_s[2], 
                  &l_p[0], &l_p[1], &l_p[2]) == 16){
        replace_underscore_with_space(nom_str);
        plateau[i] = init_Property(nom_str, (Color)color_val, 0, val, c_const, l_vide, l_m, l_s, l_p, 0, Vide, 0);
        i++ ;
    }
    if (i < NB_PROPRIETE) {
    int c = fgetc(file);
    printf("La lecture s'est arretee. Prochain caractere dans le fichier : '%c' (code ASCII : %d)\n", c, c);
    }
    fclose(file);
    return i;
    
}

/*Vérifie si un joueur possède ou non la couleur complète*/
int full_color(Property * tableau[NB_PROPRIETE], Player *p, Color couleur ){
    int i;
    for(i=0; i<NB_PROPRIETE; i++){
        if(tableau[i]->color == couleur){
            if(tableau[i]->owner != p->number){
                printf("%s possède ne possède pas toute la couleur %s \n",p->name,color_names[couleur]);
                return 0 ; /*ne possède pas toute la couleur*/
            }
        }
    }
    printf("%s possède toute la couleur %s \n",p->name,color_names[couleur]);
    return 1 ; /*possède l'entiereté de la couleur*/
}

void must_rent_Property(Property * tableau[NB_PROPRIETE], Property * prop, Player *p, Player * ownr){
    int dut;
    int banqueroute;
    if(full_color(tableau,ownr,prop->color)){ /*on vérifie si un joueur à toute la couleur avant de vérifié si il y a des maisons dessus*/
        switch(prop->type){
            case 0:
                dut = prop->rent_full_color;
                break;
            case 1:
                dut = prop->rent_maison[(prop->level)-1];
                break;
            case 2:
                dut = prop->rent_logement[(prop->level)-1];
                break;
            case 3:
                dut = prop->rent_parc[(prop->level)-1];
                break;
        }
    }
    else{
        dut = prop->rent_empty;
    }
    player_add_credits(ownr,dut);
    banqueroute = player_remove_credits(p,dut);
    if(banqueroute == 0){
        printf("BANQUEROUTE de %s \n", p->name); /*il faudra réalisé le protocole de banqueroute*/
    }
}

int add_level(Property * tableau[NB_PROPRIETE], Property * prop,Player * p , int type_choisi){
    if(prop->level < 3 && full_color(tableau,p,prop->color)){
        if(prop->level == 0){
            prop->type = type_choisi;
        }
    prop->level++;
    player_remove_credits(p,prop->construction_cost);
    return 1 ;
    }
    else{
        printf("Impossible de rajouté un niveau de propriété\n");
        return 0;
    }
}

int check_uniformity(Property * tableau[NB_PROPRIETE], Property * prop, int action) {
    if (prop == NULL) return 0;

    int min_lvl = 3;
    int max_lvl = 0;

    // On scanne le groupe de couleur
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau[i] != NULL && tableau[i]->color == prop->color) {
            if (tableau[i]->level < min_lvl) min_lvl = tableau[i]->level;
            if (tableau[i]->level > max_lvl) max_lvl = tableau[i]->level;
        }
    }

    //  Logique de vérification
    if (action == 1) { // CONSTRUCTION
        // On ne peut construire que si on n'est pas déjà au dessus du min
        if (prop->level > min_lvl) {
            printf("Construction non uniforme ! Égalisez les niveaux d'abord.\n");
            return 0;
        }
    } 
    else if (action == -1) { // DESTRUCTION
        // On ne peut détruire que si on n'est pas déjà en dessous du max
        if (prop->level < max_lvl) {
            printf("Destruction non uniforme ! Réduisez les autres d'abord.\n");
            return 0;
        }
    }

    return 1;
}

int remove_level(Property * tableau[NB_PROPRIETE], Property * prop, Player * p) {
    //  Vérifications de base
    if (prop->level > 0 && prop->owner == p->number) {
        
        // Vérification de l'uniformité (action = -1)
        if (!check_uniformity(tableau, prop, -1)) {
            return 0;
        }

        prop->level--;
        
        // Si on retombe à 0, la case redevient "Vide"
        if (prop->level == 0) {
            prop->type = 0; // Type Vide
        }

        // Remboursement ( 100% du prix de construction)
        int remboursement = prop->construction_cost ;
        player_add_credits(p, remboursement);
        
        return 1;
    }
    
    printf("Impossible de détruire un niveau ici.\n");
    return 0;
}