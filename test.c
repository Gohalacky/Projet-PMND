#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <stdlib.h>
#include "property.h"
#include "player.h"


// test l'initialisation de propiété
static void test_init_Property_basic(void **state) {
    (void) state; /* Variable inutilisée */

    /* Données de test (tirées de ta ligne Marron dans l'image Excel) */
    int rm[3] = {15, 45, 135};
    int rl[3] = {10, 30, 90};
    int rp[3] = {3, 8, 23};

    /* On appelle ta fonction avec tes arguments en français */
    Property *p = init_Property("Boulevard de Belleville", MARRON, 0, 60, 50, 2, rm, rl, rp, 0, 0, 10);

    
    /* Vérifier que l'allocation mémoire a réussi */
    assert_non_null(p);

    /* Vérifie les chaînes de caractères et les entiers  */
    assert_string_equal(p->name, "Boulevard de Belleville");
    assert_int_equal(p->color, MARRON);
    assert_int_equal(p->value, 60);
    assert_int_equal(p->construction_cost, 50);
    assert_int_equal(p->rent_empty, 2);

    /* Vérifie que les tableaux de loyers ont été bien copiés */
    /* On vérifie par exemple le loyer de la maison 3 (index 2) */
    assert_int_equal(p->rent_maison[2], 135);
    assert_int_equal(p->rent_logement[1], 30);
    assert_int_equal(p->rent_parc[0], 3);

    /*  Vérifier les états par défaut */
    assert_int_equal(p->level, 0);
    assert_int_equal(p->indice, 10);

    /* libération de la mémoire*/
    free(p);
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
        cmocka_unit_test(test_init_player_values),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}