#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "property.h"

Property * init_Property(char *nom, Color couleur, int proprio, int valeur,int construction_cout,int loyer_vide,int loyer_maison[3],int loyer_logement[3],int loyer_parc[3],int  niveau, int type_logement,int note){
    
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