#include <stdio.h>
#include "modele/alea.h"
#include "modele/cartes.h"
#include "modele/evenements.h"
#include "modele/eco.h"

int main(void) {
    Alea a;
    alea_init(&a, 12345);

    Paquet paquets[PAQUET_NB];
    int ordre_chance[32];
    int ordre_commu[32];
    cartes_init_paquets(paquets, ordre_chance, 32, ordre_commu, 32, &a);

    EtatEvenementGlobal evt;
    evenements_init(&evt);

    NoteEco noteA = NOTE_A;
    NoteEco noteF = NOTE_F;

    int round = 1;
    for (round; round <= 6; ++round) {
        printf("\n=== ROUND %d (check evenement 1 fois) ===\n", round);

        /* Exemple bonus_risque : +0.05 */
        evenements_tenter_demarrage(&evt, &a, 0.08, 0.05, 40);

        if (evt.id != EVT_AUCUN) {
            const DefEvenement *d = evenements_def(evt.id);
            printf("Evenement ACTIF: %s (%d tours restants)\n", d->nom, evt.tours_restants);
            if (d->utilise_zone_bloquee) {
                printf("  Zone bloquee: debut=%d, longueur=%d\n", evt.zone_debut, evt.zone_longueur);
            }
        } else {
            printf("Aucun evenement.\n");
        }

        const Carte *c1 = cartes_piocher(&paquets[PAQUET_CHANCE], &a);
        const Carte *c2 = cartes_piocher(&paquets[PAQUET_COMMUNAUTE], &a);

        if (c1) printf("Chance: %s : %s\n", c1->titre, c1->description);
        if (c2) {
            printf("Communaute: %s : %s\n", c2->titre, c2->description);
            if (c2->effet == EFFET_TAXE_ECO) {
                printf("  Taxe eco (A)=%d ECTS | (F)=%d ECTS\n",
                       cartes_taxe_eco(c2, noteA),
                       cartes_taxe_eco(c2, noteF));
            }
        }

        evenements_tick(&evt);
    }

    return 0;
}