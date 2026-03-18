#include "alea.h"

void alea_init(Alea *a, uint32_t graine) {
    a->etat = graine;
}

uint32_t alea_u32(Alea *a) {
    uint32_t x = a->etat;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    a->etat = x;
    return x;
}

uint32_t alea_borne(Alea *a, uint32_t borne) {
    return (borne == 0) ? 0u : (alea_u32(a) % borne);
}

int alea_chance(Alea *a, double p) {
    if (p <= 0.0) return 0;
    if (p >= 1.0) return 1;
    const double x = (double)alea_u32(a) / 4294967296.0; /* [0,1) */
    return x < p;
}