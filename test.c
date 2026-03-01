#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <stdlib.h>
#include "property.h"
#include "player.h"


static void test_init_Property_basic(void **state) {
    (void) state; /* Variable inutilisée */

    /* Données de test (tirées de la premiere ligne*/
    int rm[3] = {15, 45, 135};
    int rl[3] = {10, 30, 90};
    int rp[3] = {3, 8, 23};

    /* On appelle la fonction avec les arguments*/
    Property *p = init_Property("Boulevard de Belleville", MARRON, 0, 60, 50, 2, rm, rl, rp, 0, Vide, 10);

    
    /* Vérifier que l'allocation mémoire a réussi */
    assert_non_null(p);

    /* Vérifie les chaînes de caractères et les entiers  */
    assert_string_equal(p->name, "Boulevard de Belleville");
    assert_int_equal(p->color, MARRON);
    assert_int_equal(p->value, 60);
    assert_int_equal(p->construction_cost, 50);
    assert_int_equal(p->rent_empty, 2);

    /* Vérifie que les tableaux de loyers ont été bien copiés par exemple le loyer de la maison 3 (index 2)*/
    assert_int_equal(p->rent_maison[2], 135);
    assert_int_equal(p->rent_logement[1], 30);
    assert_int_equal(p->rent_parc[0], 3);

    /*  Vérifier les états par défaut */
    assert_int_equal(p->level, 0);
    assert_int_equal(p->indice, 10);

    /* libération de la mémoire*/
    free(p);
}

static void test_chargement_complet(void **state) {
    (void) state;
    Property *plateau[NB_PROPRIETE] = {NULL};
    
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt");

    /* : On attend exactement 22 propriétés*/
    assert_int_equal(nb, 22);

    /* On vérifie les données de la première propriété (A)*/
    assert_non_null(plateau[0]);
    assert_string_equal(plateau[0]->name, "A");
    assert_int_equal(plateau[0]->color, MARRON);
    assert_int_equal(plateau[0]->value, 60);

    /*On vérifie une donnée au milieu (par exemple C, index 2)*/
    assert_non_null(plateau[2]);
    assert_string_equal(plateau[2]->name, "C");
    assert_int_equal(plateau[2]->color, GRIS); /* Index 1 dans enum*/
    assert_int_equal(plateau[2]->rent_empty, 6);

    /*Nettoyage de la mémoire après le test*/
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

// test l'initialisation de player
static void test_init_player_values(void **state) {
    (void) state; /* inutilisé */

    // 1. Action
    Player *p = init_player("TestPlayer", 1);

    // 2. Assertions (Vérifications)
    assert_non_null(p);
    assert_string_equal(p->name, "TestPlayer");
    assert_int_equal(p->number, 1);
    assert_int_equal(p->credit, 1500);
    assert_int_equal(p->position, 0);
    assert_int_equal(p->eco_index, GRADE_A);

    // 3. Nettoyage (Crucial pour Valgrind)
    free(p);
}




int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init_Property_basic),
        cmocka_unit_test(test_chargement_complet),
        cmocka_unit_test(test_init_player_values),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}