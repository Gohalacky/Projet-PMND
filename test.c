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

    /* Données de test*/
    int rm[3] = {15, 45, 135};
    int rl[3] = {10, 30, 90};
    int rp[3] = {3, 8, 23};


    /* On appelle la fonction avec les arguments*/
    Property *p = init_Property("Boulevard de Belleville", MARRON, 0, 60, 50, 2, rm, rl, rp, 0, 0, 10);

    
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
    assert_string_equal(plateau[0]->name, "LongsChamps");
    assert_int_equal(plateau[0]->color, MARRON);
    assert_int_equal(plateau[0]->value, 60);

    /*On vérifie une donnée au milieu (par exemple C, index 2)*/
    assert_non_null(plateau[2]);
    assert_string_equal(plateau[2]->name, "Jeanne d'Arc");
    assert_int_equal(plateau[2]->color, GRIS); /* Index 1 dans enum*/
    assert_int_equal(plateau[2]->rent_empty, 6);

    /*Nettoyage de la mémoire après le test*/
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

// test l'initialisation de player
static void test_init_player_values(void **state) {
    (void) state;


    Player *p = init_player("TestPlayer", 1);


    assert_non_null(p);
    assert_string_equal(p->name, "TestPlayer");
    assert_int_equal(p->number, 1);
    assert_int_equal(p->credit, 1500);
    assert_int_equal(p->position, 0);
    assert_int_equal(p->eco_index, GRADE_A);

    // Nettoyage
    free(p);
}




static void test_calcul_moyenne_eco_index_player(void **state) {
    (void) state;
    Player *p = init_player("Ecolo", 1);
    
    // Création plateau de 2 propriétés
    Property board[2];
    // Propriété 1 : Note B
    // Propriété 2 : Note D
    // Moyenne attendue : (2+4)/2 = 3 -> Grade C
    
    board[0].owner = 1; 
    board[0].indice = GRADE_B;
    board[1].owner = 1; 
    board[1].indice = GRADE_D;
    
    calcul_moyenne_eco_index_player(p, board, 2);
    
    assert_int_equal(p->eco_index, GRADE_C);
    
    free(p);
}

static void test_player_move(void **state) {
    (void) state;
    Player *p = init_player("TestMove\n", 1);
    p->position = 35;
    p->credit = 1000;

    player_move(p, 10); // 35 + 10 = 45 -> 45 % 40 = 5

    assert_int_equal(p->position, 5);
    free(p);
}

/* Test de la gestion des crédits et faillite */
static void test_player_credits_and_faillite(void **state) {
    (void) state;
    Player *p = init_player("Test player crédit \n", 1);
    p->credit = 100;

    player_add_credits(p, 50);
    assert_int_equal(p->credit, 150);

    int success = player_remove_credits(p, 200);
    assert_int_equal(success, 0); // Doit retourner 0 (faillite)
    assert_int_equal(p->credit, 0);

    free(p);
}

/* Test de l'acquisition d'une propriété */
static void test_player_acquisition_propriete(void **state) {
    (void) state;
    Player *p = init_player("Owner", 1);
    // On simule une propriété (tableaux de loyers vides pour le test)
    int rm[3]={0}, rl[3]={0}, rp[3]={0};
    Property *prop = init_Property("Test Prop", MARRON, -1, 60, 50, 2, rm, rl, rp, 0, 0, 10);

    player_add_property(p, prop);
    assert_int_equal(prop->owner, p->number);

    player_remove_property(p, prop);
    assert_int_equal(prop->owner, 0);

    free(p);
    free(prop);
}

static void test_full_color(void **state) {
    (void) state;
    Player *p1 = init_player("Testfullcolor1\n", 1);
    Player *p2 = init_player("Testfullcolor2\n", 2);
    Property * plateau[NB_PROPRIETE] = {NULL}; /*création d'un tableau de toute les propriete*/
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); /*chargement des 22 propriétés classiques*/
    Property *prop1;
    Property *prop2;
    Property *prop3;
    prop1=plateau[0];
    prop2=plateau[1];
    prop3=plateau[2];
    player_add_property(p1, prop1);
    player_add_property(p1, prop2);
    player_add_property(p2, prop3);

    assert_int_equal(full_color(plateau,p1,MARRON),1);
    assert_int_equal(full_color(plateau,p2,GRIS),0);
    /*à la fin du jeu on libère les propriétés qui sont chargées*/
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL){
            free(plateau[i]);
        }
    }
    free(p1);
    free(p2);
}

static void test_underscore_conversion(void **state) {
    (void) state;
    char nom_test[] = "Rue_de_Vaugirard";
    replace_underscore_with_space(nom_test);
    assert_string_equal(nom_test, "Rue de Vaugirard");
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init_Property_basic),
        cmocka_unit_test(test_chargement_complet),
        cmocka_unit_test(test_init_player_values),
        cmocka_unit_test(test_calcul_moyenne_eco_index_player),
        cmocka_unit_test(test_player_move),
        cmocka_unit_test(test_player_credits_and_faillite),
        cmocka_unit_test(test_player_acquisition_propriete),
        cmocka_unit_test(test_full_color),
        cmocka_unit_test(test_underscore_conversion)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}