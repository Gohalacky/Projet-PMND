#ifndef ECO_H
#define ECO_H

typedef enum {
    NOTE_A = 0,
    NOTE_B,
    NOTE_C,
    NOTE_D,
    NOTE_E,
    NOTE_F,
    NOTE_ECO_NB
} NoteEco;

/* Convertit 'A'..'F' en NoteEco. 0 si OK. */
int eco_depuis_char(char c, NoteEco *out);

/* Convertit NoteEco en 'A'..'F'. */
char eco_vers_char(NoteEco n);

/* Coefficient de taxe selon la note :
   A:0.5 B:0.8 C:1.0 D:1.2 E:1.5 F:2.0 */
double eco_coeff_taxe(NoteEco n);

/* Montant de taxe = base * coeff, arrondi à l'entier le plus proche (0.5 arrondi au-dessus). */
int eco_montant_taxe(int base, NoteEco n);

#endif