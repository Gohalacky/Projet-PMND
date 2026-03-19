#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "property.h"


Property * init_Property(char *nom, Color couleur, int proprio, int valeur,int construction_cout,int loyer_vide,int loyer_maison[3],int loyer_logement[3],int loyer_parc[3],int  niveau, logement type_logement,int note){
    
    Property * p = (Property *)malloc(sizeof(Property));
    
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
