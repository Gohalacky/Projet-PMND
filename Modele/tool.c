#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tool.h"

void replace_underscore_with_space(char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '_') {
            str[i] = ' ';
        }
    }
}

int engine_roll_dice(int admin_mode, int *compteur_doubles) {
    if (admin_mode) {
        int choix = 0;
        while (choix < 1 || choix > 40) {
            printf("\n[MODE ADMIN] Entrez le nombre de cases à avancer (1-40) : ");
            if (scanf("%d", &choix) != 1) {
                while(getchar() != '\n'); // Nettoyer si erreur
            }
        }
        *compteur_doubles = 0; // En mode admin, on réinitialise les doubles
        return choix;
    }
    
    // Mode normal
    int de1 = rand() % 6 + 1;
    int de2 = rand() % 6 + 1;
    
    // Si c'est un double, on incrémente la valeur pointée. 
    // Sinon, on la remet à zéro.
    if (de1 == de2) {
        (*compteur_doubles)++; // Attention aux parenthèses, elles sont obligatoires 
        printf("Double de %d !\n",de1);
    } else {
        *compteur_doubles = 0;
        printf("%d + %d = %d\n",de1,de2,de1+de2);
    }
    
    return de1 + de2;
}