#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdlib.h>

#include "../src/Model/evenements.h"
#include "../src/Model/cartes.h"

#include "../src/Model/player.h"


/* ========================================================================= */
/* TESTS DU MODULE CARTES                                                    */
/* ========================================================================= */

/**
 * Vérifie que la pioche Chance est bien initialisée :
 * - bon nombre de cartes
 * - sommet à 0
 * - toutes les cartes sont dans le paquet
 * - aucune carte n'appartient à un joueur
 */
static void test_init_pioche_chance(void **state) {
    (void) state;

    PiocheChance pioche;
    init_pioche_chance(&pioche);

    assert_int_equal(pioche.nb_cartes, NB_CARTES_CHANCE);
    assert_int_equal(pioche.sommet, 0);

    for (int i = 0; i < pioche.nb_cartes; i++) {
        assert_int_equal(pioche.cartes[i].etat, ETAT_CARTE_DANS_PAQUET);
        assert_int_equal(pioche.cartes[i].owner_number, 0);
    }
}

/**
 * Vérifie que la pioche Communauté est bien initialisée.
 */
static void test_init_pioche_communaute(void **state) {
    (void) state;

    PiocheCommunaute pioche;
    init_pioche_communaute(&pioche);

    assert_int_equal(pioche.nb_cartes, NB_CARTES_COMMUNAUTE);
    assert_int_equal(pioche.sommet, 0);

    for (int i = 0; i < pioche.nb_cartes; i++) {
        assert_int_equal(pioche.cartes[i].etat, ETAT_CARTE_DANS_PAQUET);
        assert_int_equal(pioche.cartes[i].owner_number, 0);
    }
}

/**
 * Vérifie que la carte "Sortie de prison" est bien présente dans le paquet Chance.
 */
static void test_presence_carte_sortie_prison(void **state) {
    (void) state;

    PiocheChance pioche;
    init_pioche_chance(&pioche);

    int trouve = 0;
    for (int i = 0; i < pioche.nb_cartes; i++) {
        if (pioche.cartes[i].effet == CARTE_SORTIE_PRISON) {
            assert_string_equal(pioche.cartes[i].titre, "Sortie de prison");
            trouve = 1;
            break;
        }
    }

    assert_int_equal(trouve, 1);
}

/**
 * Vérifie qu'une carte Chance peut être piochée.
 */
static void test_piocher_carte_chance(void **state) {
    (void) state;

    PiocheChance pioche;
    init_pioche_chance(&pioche);

    Carte *c = piocher_carte_chance(&pioche, 1);

    assert_non_null(c);
}

/**
 * Vérifie qu'une carte Communauté peut être piochée.
 */
static void test_piocher_carte_communaute(void **state) {
    (void) state;

    PiocheCommunaute pioche;
    init_pioche_communaute(&pioche);

    Carte *c = piocher_carte_communaute(&pioche, 1);

    assert_non_null(c);
}

/**
 * Vérifie qu'une carte Sortie de prison est placée en main du joueur
 * lorsqu'on l'attribue explicitement.
 */
static void test_donner_carte_a_joueur(void **state) {
    (void) state;

    PiocheChance pioche;
    init_pioche_chance(&pioche);

    Carte *carte = NULL;
    for (int i = 0; i < pioche.nb_cartes; i++) {
        if (pioche.cartes[i].effet == CARTE_SORTIE_PRISON) {
            carte = &pioche.cartes[i];
            break;
        }
    }

    assert_non_null(carte);

    donner_carte_a_joueur(carte, 2);

    assert_int_equal(carte->etat, ETAT_CARTE_EN_MAIN);
    assert_int_equal(carte->owner_number, 2);
}

/**
 * Vérifie la fonction est_carte_sortie_prison.
 */
static void test_est_carte_sortie_prison(void **state) {
    (void) state;

    PiocheChance pioche;
    init_pioche_chance(&pioche);

    Carte *sortie = NULL;
    Carte *autre = NULL;

    for (int i = 0; i < pioche.nb_cartes; i++) {
        if (pioche.cartes[i].effet == CARTE_SORTIE_PRISON) {
            sortie = &pioche.cartes[i];
        } else if (autre == NULL) {
            autre = &pioche.cartes[i];
        }
    }

    assert_non_null(sortie);
    assert_non_null(autre);

    assert_int_equal(est_carte_sortie_prison(sortie), 1);
    assert_int_equal(est_carte_sortie_prison(autre), 0);
}

/**
 * Vérifie qu'une carte utilisée change bien d'état.
 */
static void test_utiliser_carte(void **state) {
    (void) state;

    PiocheChance pioche;
    init_pioche_chance(&pioche);

    Carte *carte = &pioche.cartes[0];
    donner_carte_a_joueur(carte, 1);
    utiliser_carte(carte);

    assert_int_equal(carte->etat, ETAT_CARTE_UTILISEE);
    assert_int_equal(carte->owner_number, 0);
}

/**
 * Vérifie qu'une carte peut être remise dans le paquet.
 */
static void test_remettre_carte_dans_paquet(void **state) {
    (void) state;

    PiocheChance pioche;
    init_pioche_chance(&pioche);

    Carte *carte = &pioche.cartes[0];
    donner_carte_a_joueur(carte, 1);
    utiliser_carte(carte);
    remettre_carte_dans_paquet(carte);

    assert_int_equal(carte->etat, ETAT_CARTE_DANS_PAQUET);
    assert_int_equal(carte->owner_number, 0);
}

/**
 * Vérifie qu'un joueur est bien détecté comme possédant une sortie de prison.
 */
static void test_joueur_possede_sortie_prison(void **state) {
    (void) state;

    PiocheChance pioche;
    init_pioche_chance(&pioche);

    Carte *sortie = NULL;
    for (int i = 0; i < pioche.nb_cartes; i++) {
        if (pioche.cartes[i].effet == CARTE_SORTIE_PRISON) {
            sortie = &pioche.cartes[i];
            break;
        }
    }

    assert_non_null(sortie);

    donner_carte_a_joueur(sortie, 3);

    assert_int_equal(joueur_possede_sortie_prison(pioche.cartes, pioche.nb_cartes, 3), 1);
    assert_int_equal(joueur_possede_sortie_prison(pioche.cartes, pioche.nb_cartes, 1), 0);
}

/**
 * Vérifie le calcul de taxe écologique.
 */
static void test_calcul_taxe_ecologique(void **state) {
    (void) state;

    assert_int_equal(calcul_taxe_ecologique(20, GRADE_A), 10);
    assert_int_equal(calcul_taxe_ecologique(20, GRADE_B), 16);
    assert_int_equal(calcul_taxe_ecologique(20, GRADE_C), 20);
    assert_int_equal(calcul_taxe_ecologique(20, GRADE_D), 24);
    assert_int_equal(calcul_taxe_ecologique(20, GRADE_E), 30);
    assert_int_equal(calcul_taxe_ecologique(20, GRADE_F), 40);
}

/* ========================================================================= */
/* TESTS DU MODULE EVENEMENTS                                                */
/* ========================================================================= */

/**
 * Vérifie l'initialisation de l'état global des événements.
 */
static void test_evenements_init(void **state) {
    (void) state;

    EtatEvenementGlobal evt;
    evenements_init(&evt);

    assert_int_equal(evt.id, EVT_AUCUN);
    assert_int_equal(evt.tours_restants, 0);
    assert_int_equal(evt.zone_debut, -1);
    assert_int_equal(evt.zone_longueur, 0);
}

/**
 * Vérifie qu'on récupère correctement une définition d'événement.
 */
static void test_evenements_def(void **state) {
    (void) state;

    const DefEvenement *def = evenements_def(EVT_CONFINEMENT);

    assert_non_null(def);
    assert_string_equal(def->nom, "Confinement");
    assert_true(def->mult_loyers > 1.0);
    assert_int_equal(def->autorise_arret_chez_soi, 1);
}

/**
 * Vérifie la fonction de bonus de risque à partir de l'indice écologique global.
 */
static void test_evenements_bonus_risque_depuis_eco(void **state) {
    (void) state;

    assert_true(evenements_bonus_risque_depuis_eco(GRADE_A) < evenements_bonus_risque_depuis_eco(GRADE_F));
    assert_true(evenements_bonus_risque_depuis_eco(GRADE_C) > evenements_bonus_risque_depuis_eco(GRADE_B));
}

/**
 * Vérifie qu'un événement peut se déclencher si la probabilité est forcée à 1.
 */
static void test_evenements_tenter_demarrage_force(void **state) {
    (void) state;

    EtatEvenementGlobal evt;
    evenements_init(&evt);

    int declenche = evenements_tenter_demarrage(&evt, 1.0, 0.0, 40);

    assert_int_equal(declenche, 1);
    assert_int_not_equal(evt.id, EVT_AUCUN);
    assert_true(evt.tours_restants > 0);
}

/**
 * Vérifie qu'on ne déclenche pas un nouvel événement si un autre est déjà actif.
 */
static void test_evenements_pas_de_nouveau_si_actif(void **state) {
    (void) state;

    EtatEvenementGlobal evt;
    evt.id = EVT_INFLATION;
    evt.tours_restants = 2;
    evt.zone_debut = -1;
    evt.zone_longueur = 0;

    int declenche = evenements_tenter_demarrage(&evt, 1.0, 0.0, 40);

    assert_int_equal(declenche, 0);
    assert_int_equal(evt.id, EVT_INFLATION);
    assert_int_equal(evt.tours_restants, 2);
}

/**
 * Vérifie le décrément du nombre de tours et la fin d'un événement.
 */
static void test_evenements_tick(void **state) {
    (void) state;

    EtatEvenementGlobal evt;
    evt.id = EVT_CANICULE;
    evt.tours_restants = 2;
    evt.zone_debut = -1;
    evt.zone_longueur = 0;

    assert_int_equal(evenements_tick(&evt), 0);
    assert_int_equal(evt.tours_restants, 1);
    assert_int_equal(evt.id, EVT_CANICULE);

    assert_int_equal(evenements_tick(&evt), 1);
    assert_int_equal(evt.id, EVT_AUCUN);
    assert_int_equal(evt.tours_restants, 0);
}

/**
 * Vérifie les accesseurs sur un événement Confinement.
 */
static void test_accesseurs_confinement(void **state) {
    (void) state;

    EtatEvenementGlobal evt;
    evt.id = EVT_CONFINEMENT;
    evt.tours_restants = 2;
    evt.zone_debut = -1;
    evt.zone_longueur = 0;

    assert_int_equal(evenements_est_actif(&evt), 1);
    assert_true(evenements_mult_loyers(&evt) == 2.0);
    assert_true(evenements_mult_cout_construction(&evt) == 1.0);
    assert_int_equal(evenements_autorise_arret_chez_soi(&evt), 1);
    assert_int_equal(evenements_interdit_construire(&evt), 0);
}

/**
 * Vérifie les accesseurs sur un événement Mouvement étudiant.
 */
static void test_accesseurs_mouvement_etudiant(void **state) {
    (void) state;

    EtatEvenementGlobal evt;
    evt.id = EVT_MOUVEMENT_ETUDIANT;
    evt.tours_restants = 2;
    evt.zone_debut = -1;
    evt.zone_longueur = 0;

    assert_int_equal(evenements_interdit_construire(&evt), 1);
    assert_true(evenements_mult_loyers(&evt) < 1.0);
}

/**
 * Vérifie qu'une zone bloquée est correctement reconnue.
 */
static void test_evenements_case_est_bloquee(void **state) {
    (void) state;

    EtatEvenementGlobal evt;
    evt.id = EVT_INONDATIONS;
    evt.tours_restants = 2;
    evt.zone_debut = 10;
    evt.zone_longueur = 3;

    assert_int_equal(evenements_zone_bloquee(&evt), 1);
    assert_int_equal(evenements_case_est_bloquee(&evt, 10), 1);
    assert_int_equal(evenements_case_est_bloquee(&evt, 11), 1);
    assert_int_equal(evenements_case_est_bloquee(&evt, 12), 1);
    assert_int_equal(evenements_case_est_bloquee(&evt, 13), 0);
    assert_int_equal(evenements_case_est_bloquee(&evt, 9), 0);
}

/**
 * Vérifie qu'en l'absence d'événement actif, les valeurs renvoyées sont neutres.
 */
static void test_accesseurs_aucun_evenement(void **state) {
    (void) state;

    EtatEvenementGlobal evt;
    evenements_init(&evt);

    assert_int_equal(evenements_est_actif(&evt), 0);
    assert_true(evenements_mult_loyers(&evt) == 1.0);
    assert_true(evenements_mult_cout_construction(&evt) == 1.0);
    assert_true(evenements_mult_gains_cartes(&evt) == 1.0);
    assert_int_equal(evenements_interdit_construire(&evt), 0);
    assert_int_equal(evenements_autorise_arret_chez_soi(&evt), 0);
    assert_int_equal(evenements_zone_bloquee(&evt), 0);
}

/* ========================================================================= */
/* MAIN                                                                      */
/* ========================================================================= */

int main(void) {
    const struct CMUnitTest tests[] = {
        /* ---------------------------- CARTES ----------------------------- */
        cmocka_unit_test(test_init_pioche_chance),
        cmocka_unit_test(test_init_pioche_communaute),
        cmocka_unit_test(test_presence_carte_sortie_prison),
        cmocka_unit_test(test_piocher_carte_chance),
        cmocka_unit_test(test_piocher_carte_communaute),
        cmocka_unit_test(test_donner_carte_a_joueur),
        cmocka_unit_test(test_est_carte_sortie_prison),
        cmocka_unit_test(test_utiliser_carte),
        cmocka_unit_test(test_remettre_carte_dans_paquet),
        cmocka_unit_test(test_joueur_possede_sortie_prison),
        cmocka_unit_test(test_calcul_taxe_ecologique),

        /* -------------------------- EVENEMENTS --------------------------- */
        cmocka_unit_test(test_evenements_init),
        cmocka_unit_test(test_evenements_def),
        cmocka_unit_test(test_evenements_bonus_risque_depuis_eco),
        cmocka_unit_test(test_evenements_tenter_demarrage_force),
        cmocka_unit_test(test_evenements_pas_de_nouveau_si_actif),
        cmocka_unit_test(test_evenements_tick),
        cmocka_unit_test(test_accesseurs_confinement),
        cmocka_unit_test(test_accesseurs_mouvement_etudiant),
        cmocka_unit_test(test_evenements_case_est_bloquee),
        cmocka_unit_test(test_accesseurs_aucun_evenement)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}