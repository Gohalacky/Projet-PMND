#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hypo.h"

int hypotheque_PROP(Property * prop, Player * p){
    //  Vérifications de sécurité de base
    if (prop == NULL || p == NULL) return 0;
    if (prop->owner != p->number) {
        printf("Erreur : %s n'appartient pas à %s.\n", prop->name, p->name);
        return 0; 
    }
    if (prop->hypo) {
        printf("Erreur : %s est déjà hypothéquée.\n", prop->name);
        return 0;
    }

    //  Vérification stricte : AUCUNE construction sur la case
    if(prop->level !=0){
        printf("Impossible d'hypothéquer %s, il reste des constructions sur la case \n", prop->name);
        return 0;
    }
    //  Application de l'hypothèque
    prop->hypo = 1;
    int gain = prop->value / 2; // L'hypothèque rapporte la moitié de la valeur d'achat
    player_add_credits(p,gain);
    
    printf("%s a hypothéqué %s et récupère %d ECTS.\n", p->name, prop->name, gain);
    return 1;
}


int hypotheque_GARE(Gares * gare, Player * p) {
    if (gare == NULL || p == NULL) return 0;
    if (gare->owner != p->number || gare->hypo) return 0;

    gare->hypo = 1;
    int gain = (gare->value / 2);
    player_add_credits(p,gain);

    printf("%s a hypothéqué la gare %s.\n", p->name, gare->name);
    return 1;
}


int hypotheque_COMP(Compagnies * comp, Player * p) {
    if (comp == NULL || p == NULL) return 0;
    if (comp->owner != p->number || comp->hypo) return 0;

    comp->hypo = 1;
    int gain= (comp->value / 2); 
    player_add_credits(p,gain);
    
    printf("%s a hypothéqué la compagnie %s.\n", p->name, comp->name);
    return 1;
}

int rachathypo_PROP(Property * prop, Player * p){
    if (prop == NULL || p == NULL) return 0;
    if (prop->owner != p->number) {
        printf("Erreur : %s n'appartient pas à %s.\n", prop->name, p->name);
        return 0; 
    }
    if (prop->hypo == 0) {
        printf("Erreur : %s est n'est pas déjà hypothéquée.\n", prop->name);
        return 0;
    }
    int du;
    int valeur_hypo = prop->value / 2;
    du = valeur_hypo + (valeur_hypo * 0.1);
    if(du > p->credit){
        printf("Action impossible, %s n'a pas assez de crédits. \n",p->name);
        return 0;
    }
    player_remove_credits(p,du);
    prop->hypo = 0;
    printf("%s à racheté l'hypothèque de %s \n",p->name,prop->name);
    return 1;
}

int rachathypo_COMP(Compagnies * prop, Player * p){
    if (prop == NULL || p == NULL) return 0;
    if (prop->owner != p->number) {
        printf("Erreur : %s n'appartient pas à %s.\n", prop->name, p->name);
        return 0; 
    }
    if (prop->hypo == 0) {
        printf("Erreur : %s est n'est pas déjà hypothéquée.\n", prop->name);
        return 0;
    }
    int du;
    int valeur_hypo = prop->value / 2;
    du = valeur_hypo + (valeur_hypo * 0.1);
    if(du > p->credit){
        printf("Action impossible, %s n'a pas assez de crédits. \n",p->name);
        return 0;
    }
    player_remove_credits(p,du);
    prop->hypo = 0;
    printf("%s à racheté l'hypothèque de %s \n",p->name,prop->name);
    return 1;
}

int rachathypo_GARE(Gares * prop, Player * p){
    if (prop == NULL || p == NULL) return 0;
    if (prop->owner != p->number) {
        printf("Erreur : %s n'appartient pas à %s.\n", prop->name, p->name);
        return 0; 
    }
    if (prop->hypo == 0) {
        printf("Erreur : %s est n'est pas déjà hypothéquée.\n", prop->name);
        return 0;
    }
    int du;
    int valeur_hypo = prop->value / 2;
    du = valeur_hypo + (valeur_hypo * 0.1);
    if(du > p->credit){
        printf("Action, impossible %s n'a pas assez de crédits. \n",p->name);
        return 0;
    }
    player_remove_credits(p,du);
    prop->hypo = 0;
    printf("%s à racheté l'hypothèque de %s \n",p->name,prop->name);
    return 1;
}