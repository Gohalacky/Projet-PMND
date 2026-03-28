#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <stdlib.h>
#include "property.h"
#include "player.h"
#include "tool.h"


static void test_init_Property_basic(void **state) {
    (void) state; /* Variable inutilisée */

    /* Données de test*/
    int rm[3] = {15, 45, 135};
    int rl[3] = {10, 30, 90};
    int rp[3] = {3, 8, 23};


    /* On appelle la fonction avec les arguments*/
    Property *p = init_Property("A", MARRON, 0, 60, 50, 2, rm, rl, rp, 0, 0, 10);

    
    /* Vérifier que l'allocation mémoire a réussi */
    assert_non_null(p);

    /* Vérifie les chaînes de caractères et les entiers  */
    assert_string_equal(p->name, "A");
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

static void test_init_COMPAGNIE_basic(void **state) {
    (void) state; /* Variable inutilisée */

    /* Données de test*/
    int rm[2] = {4, 10};

    /* On appelle la fonction avec les arguments*/
    Compagnies *p = init_Compagnies("A", COMP, 0, 150, rm);

    
    /* Vérifier que l'allocation mémoire a réussi */
    assert_non_null(p);

    /* Vérifie les chaînes de caractères et les entiers  */
    assert_string_equal(p->name, "A");
    assert_int_equal(p->color, COMP);
    assert_int_equal(p->value, 150);

    /* Vérifie que les tableaux de loyers ont été bien copiés par exemple le loyer de la maison 3 (index 2)*/
    assert_int_equal(p->rent[1], 10);
    assert_int_equal(p->rent[0], 4);

    /* libération de la mémoire*/
    free(p);
}

static void test_init_GARE_basic(void **state) {
    (void) state; /* Variable inutilisée */

    /* Données de test*/
    int rm[4] = {25, 50, 100, 200};

    /* On appelle la fonction avec les arguments*/
    Gares *p = init_Gares("A", GAR, 0, 200, rm);

    
    /* Vérifier que l'allocation mémoire a réussi */
    assert_non_null(p);

    /* Vérifie les chaînes de caractères et les entiers  */
    assert_string_equal(p->name, "A");
    assert_int_equal(p->color, GAR);
    assert_int_equal(p->value, 200);

    /* Vérifie que les tableaux de loyers ont été bien copiés par exemple le loyer de la maison 3 (index 2)*/
    assert_int_equal(p->rent[3], 200);
    assert_int_equal(p->rent[2], 100);
    assert_int_equal(p->rent[1], 50);
    assert_int_equal(p->rent[0], 25);

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

static void test_chargement_GARE(void **state) {
    (void) state;
    Gares *plateau[NB_GARE] = {NULL};
    
    int nb = chargement_GARE_loyer(plateau, "gares.txt");

    /* : On attend exactement 4 gares*/
    assert_int_equal(nb, 4);

    /* On vérifie les données de la première propriété (A)*/
    assert_non_null(plateau[0]);
    assert_string_equal(plateau[0]->name, "Beaulieu-Université");
    assert_int_equal(plateau[0]->color, GAR);
    assert_int_equal(plateau[0]->value, 200);

    /*On vérifie une donnée au milieu (par exemple C, index 2)*/
    assert_non_null(plateau[2]);
    assert_string_equal(plateau[2]->name, "Gare de Rennes");
    assert_int_equal(plateau[2]->color, GAR); /* Index 1 dans enum*/
    assert_int_equal(plateau[2]->rent[2], 100);
    assert_int_equal(plateau[3]->rent[3], 200);
    /*Nettoyage de la mémoire après le test*/
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

static void test_chargement_COMPAGNIE(void **state) {
    (void) state;
    Compagnies *plateau[NB_COMPAGNIE] = {NULL};
    
    int nb = chargement_COMPAGNIE_loyer(plateau, "compagnie.txt");

    /* : On attend exactement 4 gares*/
    assert_int_equal(nb, 2);

    /* On vérifie les données de la première propriété (A)*/
    assert_non_null(plateau[0]);
    assert_string_equal(plateau[0]->name, "Foy");
    assert_int_equal(plateau[0]->color, COMP);
    assert_int_equal(plateau[0]->value, 150);
    assert_non_null(plateau[1]);
    assert_string_equal(plateau[1]->name, "K'Fet");
    assert_int_equal(plateau[1]->color, COMP);
    assert_int_equal(plateau[1]->rent[0], 4);
    assert_int_equal(plateau[1]->rent[1], 10);

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

static void test_player_addremove_comp(void **state){
    (void) state;
    Player *p = init_player("Owner", 1);
    // on simule une compagnie
    int loyer[2] = {4,10};
    Compagnies * c = init_Compagnies("test comp", COMP, 0, 150, loyer);
    player_add_comp(p,c);
    assert_int_equal(c->owner, p->number);
    
    player_remove_comp(p, c);
    assert_int_equal(c->owner, 0);

    free(p);
    free(c);
}

static void test_player_addremove_gar(void **state){
    (void) state;
    Player *p = init_player("Owner", 1);
    // on simule une gare
    int loyer[4] = {25,50,100,200};
    Gares * g = init_Gares("test gare", COMP, 0, 200, loyer);
    player_add_gar(p,g);
    assert_int_equal(g->owner, p->number);
    
    player_remove_gar(p, g);
    assert_int_equal(g->owner, 0);

    free(p);
    free(g);
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

static void test_full_comp(void **state){
    (void) state;
    Player *p1 = init_player("Testfullcolor1\n", 1);
    Compagnies * tableau[NB_COMPAGNIE] = {NULL};
    int nb = chargement_COMPAGNIE_loyer(tableau, "compagnie.txt");
    player_add_comp(p1,tableau[0]);
    assert_int_equal(full_Comp(tableau, p1),0);
    player_add_comp(p1,tableau[1]);
    assert_int_equal(full_Comp(tableau, p1),1);
    for (int i = 0; i < nb; i++) {
        if (tableau[i] != NULL){
            free(tableau[i]);
        }
    }
    free(p1);
}

static void test_nb_Gare(void **state){
    (void) state;
    Player *p1 = init_player("Testnbg\n", 1);
    Gares * tableau[NB_GARE] = {NULL};
    int nb = chargement_GARE_loyer(tableau, "gares.txt");
    assert_int_equal(nb_Gare(tableau, p1),0);   // on test pour si la personne possède de 0 à 4 gares
    player_add_gar(p1,tableau[0]);
    assert_int_equal(nb_Gare(tableau, p1),1);
    player_add_gar(p1,tableau[1]);
    assert_int_equal(nb_Gare(tableau, p1),2);
    player_add_gar(p1,tableau[2]);
    assert_int_equal(nb_Gare(tableau, p1),3);
    player_add_gar(p1,tableau[3]);
    assert_int_equal(nb_Gare(tableau, p1),4);
    for (int i = 0; i < nb; i++) {
        if (tableau[i] != NULL){
            free(tableau[i]);
        }
    }
    free(p1);
}

static void test_underscore_conversion(void **state) {
    (void) state;
    char nom_test[] = "Rue_de_Vaugirard";
    replace_underscore_with_space(nom_test);
    assert_string_equal(nom_test, "Rue de Vaugirard");
}

static void test_must_rent_Property(void **state){
    (void) state;
    Player * proprio_test1 = init_player("proprio_test",1);
    Player * proprio_test2 = init_player("proprio_test",1);
    Player * loca_test = init_player("loca_test",2);
    Property * plateau[NB_PROPRIETE] = {NULL}; /*création d'un tableau de toute les propriete*/
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); /*chargement des 22 propriétés classiques*/
    player_add_property(proprio_test1,plateau[8]);
    player_add_property(proprio_test1,plateau[9]);
    player_add_property(proprio_test1,plateau[10]);
    player_add_property(proprio_test2,plateau[11]);
    plateau[8]->hypo=1;
    must_rent_Property(plateau,plateau[8],loca_test,proprio_test1);
    assert_int_equal(loca_test->credit,1500);
    must_rent_Property(plateau,plateau[10],loca_test,proprio_test1);
    must_rent_Property(plateau,plateau[11],loca_test,proprio_test2);

    assert_int_equal(loca_test->credit,1450);
    assert_int_equal(proprio_test1->credit,1532);
    assert_int_equal(proprio_test2->credit,1518);
    /*à la fin du jeu on libère les propriétés qui sont chargées*/
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL){
            free(plateau[i]);
        }
    }
    free(proprio_test1);
    free(proprio_test2);
    free(loca_test);
}

static void test_must_rent_COMP(void **state){
    (void) state;
    Player * proprio_test1 = init_player("proprio_test",1);
    Player * loca_test = init_player("loca_test",2);
    Compagnies * plateau[NB_COMPAGNIE] = {NULL};
    int nb = chargement_COMPAGNIE_loyer(plateau, "compagnie.txt"); 
    
    player_add_comp(proprio_test1,plateau[0]);
    plateau[0]->hypo=1;
    must_rent_Compagnies(plateau,plateau[0],loca_test,proprio_test1,5);
    assert_int_equal(loca_test->credit,1500);
    plateau[0]->hypo=0;
    must_rent_Compagnies(plateau,plateau[0],loca_test,proprio_test1,5);
    assert_int_equal(loca_test->credit,1480);
    assert_int_equal(proprio_test1->credit,1520);
    player_add_comp(proprio_test1,plateau[1]);
    must_rent_Compagnies(plateau,plateau[0],loca_test,proprio_test1,5);
    assert_int_equal(loca_test->credit,1430);
    assert_int_equal(proprio_test1->credit,1570);

    /*à la fin du jeu on libère les propriétés qui sont chargées*/
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL){
            free(plateau[i]);
        }
    }
    free(proprio_test1);
    free(loca_test);
}

static void test_must_rent_GARE(void **state){
    (void) state;
    Player * proprio_test1 = init_player("proprio_test",1);
    Player * loca_test = init_player("loca_test",2);
    Gares * plateau[NB_GARE] = {NULL};
    int nb = chargement_GARE_loyer(plateau, "gares.txt"); 
    
    player_add_gar(proprio_test1,plateau[0]);
    plateau[0]->hypo=1;
    must_rent_Gares(plateau,plateau[0],loca_test,proprio_test1);
    assert_int_equal(loca_test->credit,1500);
    plateau[0]->hypo=0;
    must_rent_Gares(plateau,plateau[0],loca_test,proprio_test1);
    assert_int_equal(loca_test->credit,1475);
    assert_int_equal(proprio_test1->credit,1525);
    player_add_gar(proprio_test1,plateau[1]);
    player_add_gar(proprio_test1,plateau[2]);
    player_add_gar(proprio_test1,plateau[3]);
    must_rent_Gares(plateau,plateau[0],loca_test,proprio_test1);
    assert_int_equal(loca_test->credit,1275);
    assert_int_equal(proprio_test1->credit,1725);

    /*à la fin du jeu on libère les propriétés qui sont chargées*/
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL){
            free(plateau[i]);
        }
    }
    free(proprio_test1);
    free(loca_test);
}


static void test_add_level_integration(void **state) {
    (void) state;

    //  Initialisation du joueur avec assez de crédits
    Player * p_test = init_player("Bâtisseur", 1);
    p_test->credit = 2000; // On s'assure qu'il a de quoi construire

    //  Préparation du plateau 
    Property * plateau[NB_PROPRIETE] = {NULL};
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt");

    // Attribution d'un groupe de couleur complet (ex: le groupe Orange index 8, 9, 10)
    player_add_property(p_test, plateau[8]);
    player_add_property(p_test, plateau[9]);
    player_add_property(p_test, plateau[10]);

    //  On récupère les valeurs avant construction pour le calcul
    int ancien_solde = p_test->credit;
    int cout_const = plateau[8]->construction_cost;

    
    // On choisit le type 'Maison' 
    int type_voulu = 1; 
    int success = add_level(plateau, plateau[8], p_test, type_voulu);

    assert_int_equal(success, 1);                 // La fonction doit renvoyer 1
    assert_int_equal(plateau[8]->level, 1);       // Le niveau doit être passé à 1
    assert_int_equal(plateau[8]->type, type_voulu); // Le type doit être mis à jour
    assert_int_equal(p_test->credit, ancien_solde - cout_const); // L'argent doit être retiré

    // On force le niveau à 3 manuellement pour tester la sécurité
    plateau[8]->level = 3;
    int fail = add_level(plateau, plateau[8], p_test, type_voulu);
    assert_int_equal(fail, 0); // Doit renvoyer 0 car level n'est pas < 3
    assert_int_equal(plateau[8]->level, 3); // Le niveau ne doit pas avoir bougé

    //  Nettoyage mémoire
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
    free(p_test);
}

// Test de l'uniformité lors de la construction
static void test_check_uniformity_build(void **state) {
    (void) state;
    Property *plateau[NB_PROPRIETE] = {NULL};
    Property *p1;
    Property *p2;
    // On crée 2 propriétés de la même couleur (ex: MARRON)
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); /*chargement des 22 propriétés classiques*/
    p1 = plateau[0] ; 
    p2 = plateau[1] ;

    // Cas 1 : P1 et P2 sont à 0. On peut construire sur P1 (lvl 0 -> 1)
    assert_int_equal(check_uniformity(plateau, p1, 1), 1);

    // Cas 2 : P1 passe à 1, P2 reste à 0.
    p1->level = 1;
    // On ne peut pas construire un 2ème niveau sur P1 car P2 est à 0 (écart > 1)
    assert_int_equal(check_uniformity(plateau, p1, 1), 0);
    // Mais on peut construire sur P2 pour rattraper P1
    assert_int_equal(check_uniformity(plateau, p2, 1), 1);
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

// Test de l'uniformité lors de la destruction (vente)
static void test_check_uniformity_destroy(void **state) {
    (void) state;
    Property *plateau[NB_PROPRIETE] = {NULL};
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); /*chargement des 22 propriétés classiques*/
    Property *p1;
    Property *p2;
    p1=plateau[0];
    p2=plateau[1];
    p1->type=1;
    p2->type=1;
    p1->level=1;
    p2->level=1;
    // Cas 1 : P1 et P2 sont à 1. On peut détruire sur P1 (lvl 1 -> 0)
    assert_int_equal(check_uniformity(plateau, p1, -1), 1);

    // Cas 2 : P1 est descendu à 0, P2 est resté à 1.
    p1->level = 0;
    // On ne peut pas détruire P1 (déjà à 0), mais surtout on peut détruire P2
    assert_int_equal(check_uniformity(plateau, p2, -1), 1);
    
    // Si on avait p1=2 et p2=1, on ne pourrait pas détruire p2 (car p1 est trop haut)
    p1->level = 2; p2->level = 1;
    assert_int_equal(check_uniformity(plateau, p2, -1), 0);
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

// Test complet de la fonction remove_level
static void test_remove_level_integration(void **state) {
    (void) state;
    Player *p = init_player("Test", 1);
    p->credit = 1000;
    Property *plateau[NB_PROPRIETE] = {NULL};
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); /*chargement des 22 propriétés classiques*/
    // Propriété avec une maison (lvl 1), coût construction 200
    Property *prop ;
    prop =plateau[0] ;
    prop->construction_cost = 200;
    prop->type=1;
    prop->level=1;
    prop->owner = p->number;
    // On détruit le niveau
    int success = remove_level(plateau, prop, p);

    assert_int_equal(success, 1);
    assert_int_equal(prop->level, 0);
    assert_int_equal(prop->type, 0); // Doit être revenu à Vide
    // Crédits : 1000 + (200 ) = 1200
    assert_int_equal(p->credit, 1200);

    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
    free(p);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init_Property_basic),
        cmocka_unit_test(test_init_GARE_basic),
        cmocka_unit_test(test_init_COMPAGNIE_basic),
        cmocka_unit_test(test_chargement_complet),
        cmocka_unit_test(test_chargement_GARE),
        cmocka_unit_test(test_chargement_COMPAGNIE),
        cmocka_unit_test(test_init_player_values),
        cmocka_unit_test(test_calcul_moyenne_eco_index_player),
        cmocka_unit_test(test_player_move),
        cmocka_unit_test(test_player_credits_and_faillite),
        cmocka_unit_test(test_player_acquisition_propriete),
        cmocka_unit_test(test_player_addremove_comp),
        cmocka_unit_test(test_player_addremove_gar),
        cmocka_unit_test(test_full_color),
        cmocka_unit_test(test_full_comp),
        cmocka_unit_test(test_nb_Gare),
        cmocka_unit_test(test_underscore_conversion),
        cmocka_unit_test(test_must_rent_Property),
        cmocka_unit_test(test_must_rent_COMP),
        cmocka_unit_test(test_must_rent_GARE),
        cmocka_unit_test(test_add_level_integration),
        cmocka_unit_test(test_check_uniformity_build),
        cmocka_unit_test(test_check_uniformity_destroy),
        cmocka_unit_test(test_remove_level_integration)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}