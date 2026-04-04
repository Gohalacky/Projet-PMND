#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "eco.h"

static double coeffs[NOTE_ECO_NB] = {0.5, 0.8, 1.0, 1.2, 1.5, 2.0};

int eco_depuis_char(char c, NoteEco *out) {
    if (!out) return 1;
    if (c < 'A' || c > 'F') return 2;
    *out = (NoteEco)(c - 'A');
    return 0;
}

char eco_vers_char(NoteEco n) {
    if (n < NOTE_A || n > NOTE_F) return '?';
    return (char)('A' + (int)n);
}

double eco_coeff_taxe(NoteEco n) {
    if (n < NOTE_A || n > NOTE_F) return 1.0;
    return coeffs[(int)n];
}

int eco_montant_taxe(int base, NoteEco n) {
    double x = (double)base * eco_coeff_taxe(n);
    int i = (int)x;
    double frac = x - (double)i;
    if (frac > 0.5) return i + 1;
    if (frac < 0.5) return i;
    return i + 1;
}