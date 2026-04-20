#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <string.h>
#include <stdlib.h>
#include "property.h"
#include "player.h"
#include "tool.h"
#include "hypo.h"
#include "trade.h"
#include "prison.h"
#include "voe.h"

/*
printf("==========================================================\n");
printf("              TESTS DE property                \n");
printf("==========================================================\n");
*/
static void test_init_Property_basic(void **state) {
    (void) state; /* Variable inutilisée */
    printf("==========================================================\n");
    printf("              TESTS DE property                \n");
    printf("==========================================================\n");
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
/*
printf("==========================================================\n");
printf("              TESTS DE player                 \n");
printf("==========================================================\n");
*/
// test l'initialisation de player
static void test_init_player_values(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE player                 \n");
    printf("==========================================================\n");

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



/*
printf("==========================================================\n");
printf("              TESTS DE tool                \n");
printf("==========================================================\n");
*/
static void test_underscore_conversion(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE tool                \n");
    printf("==========================================================\n");
    char nom_test[] = "Rue_de_Vaugirard";
    replace_underscore_with_space(nom_test);
    assert_string_equal(nom_test, "Rue de Vaugirard");
}
/* TEST SUPPRIMÉ CAR FONCTION DÉPLACÉ DANS LE CONTROLLEUR
// --- MOCK DE RAND() ---
// Cette fonction va remplacer le vrai rand() pendant les tests
int __wrap_rand(void) {
    // mock_type(int) récupère la valeur qu'on a préparée avec will_return()
    return mock_type(int); 
}

// --- TESTS ---

// Test 1 : Lancer normal, pas de double
static void test_normal_mode_no_double(void **state) {
    (void) state; // Paramètre non utilisé
    int compteur = 0;

    // On prépare les valeurs que rand() va renvoyer.
    // Attention : la fonction fait "rand() % 6 + 1". 
    // Si rand() renvoie 2, le dé vaudra 3 (2 % 6 + 1).
    // Si rand() renvoie 4, le dé vaudra 5 (4 % 6 + 1).
    will_return(__wrap_rand, 2); 
    will_return(__wrap_rand, 4); 

    int resultat = engine_roll_dice(0, &compteur);

    assert_int_equal(resultat, 8); // 3 + 5 = 8
    assert_int_equal(compteur, 0); // Pas de double, le compteur reste à 0
}

// Test 2 : Lancer avec un double
static void test_normal_mode_double(void **state) {
    (void) state;
    int compteur = 0;

    // rand() renvoie 3 et 3 -> dé 1 = 4, dé 2 = 4
    will_return(__wrap_rand, 3);
    will_return(__wrap_rand, 3);

    int resultat = engine_roll_dice(0, &compteur);

    assert_int_equal(resultat, 8); // 4 + 4 = 8
    assert_int_equal(compteur, 1); // C'est un double, le compteur passe à 1
}

// Test 3 : Chaîne de doubles et remise à zéro
static void test_normal_mode_multiple_doubles_and_reset(void **state) {
    (void) state;
    int compteur = 0;

    // 1er lancer : Double (dé = 1, dé = 1 -> rand() doit renvoyer 0)
    will_return(__wrap_rand, 0);
    will_return(__wrap_rand, 0);
    engine_roll_dice(0, &compteur);
    assert_int_equal(compteur, 1);

    // 2ème lancer : Double (dé = 6, dé = 6 -> rand() doit renvoyer 5)
    will_return(__wrap_rand, 5);
    will_return(__wrap_rand, 5);
    engine_roll_dice(0, &compteur);
    assert_int_equal(compteur, 2); // Le compteur s'accumule !

    // 3ème lancer : Raté, pas de double (dé = 1, dé = 2)
    will_return(__wrap_rand, 0);
    will_return(__wrap_rand, 1);
    engine_roll_dice(0, &compteur);
    
    // Le compteur doit être retombé à zéro
    assert_int_equal(compteur, 0); 
}
*/

/*
printf("==========================================================\n");
printf("              TESTS DE hypo                \n");
printf("==========================================================\n");
*/
static void test_hypotheque_et_rachat_PROP(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE hypo                \n");
    printf("==========================================================\n");
    // Initialisation
    Player *p1 = init_player("Joueur1", 1);
    Player *p2 = init_player("Joueur2", 2);
    
    Property *prop = calloc(1, sizeof(Property));
    strcpy(prop->name, "Rue de la Soif");
    prop->owner = 1;      // Appartient à p1
    prop->value = 200;    // Hypothèque = 100, Rachat = 220 (selon ta formule)
    prop->hypo = 0;
    prop->level = 0;

    // 1. Tests d'échec de mise en hypothèque
    assert_int_equal(hypotheque_PROP(prop, p2), 0); // Pas le bon propriétaire
    
    prop->level = 1; // On simule une maison construite
    assert_int_equal(hypotheque_PROP(prop, p1), 0); // Échec : il y a une construction
    prop->level = 0; // On retire la maison

    // 2. Test de succès de mise en hypothèque
    p1->credit = 1000;
    assert_int_equal(hypotheque_PROP(prop, p1), 1); // Succès
    assert_int_equal(prop->hypo, 1);
    assert_int_equal(p1->credit, 1100); // 1000 + (200/2)

    // 3. Test d'échec (déjà hypothéquée)
    assert_int_equal(hypotheque_PROP(prop, p1), 0); 

    // 4. Tests de rachat (Levée d'hypothèque)
    assert_int_equal(rachathypo_PROP(prop, p2), 0); // Pas le bon propriétaire
    
    p1->credit = 100; // Pas assez d'argent (coût = 200 + 10% = 220)
    assert_int_equal(rachathypo_PROP(prop, p1), 0);
    assert_int_equal(prop->hypo, 1); // Reste hypothéquée

    p1->credit = 1000;
    assert_int_equal(rachathypo_PROP(prop, p1), 1); // Succès
    assert_int_equal(prop->hypo, 0); // N'est plus hypothéquée
    assert_int_equal(p1->credit, 890); // 1000 - 110

    // Nettoyage
    free(prop);
    free(p1);
    free(p2);
}

static void test_hypotheque_et_rachat_GARE(void **state) {
    (void) state;

    Player *p1 = init_player("Joueur1", 1);
    Gares *gare = calloc(1, sizeof(Gares));
    strcpy(gare->name, "Gare du Nord");
    gare->owner = 1;
    gare->value = 200;
    gare->hypo = 0;

    p1->credit = 1000;

    // Succès hypothèque
    assert_int_equal(hypotheque_GARE(gare, p1), 1);
    assert_int_equal(gare->hypo, 1);
    assert_int_equal(p1->credit, 1100);

    // Succès rachat
    assert_int_equal(rachathypo_GARE(gare, p1), 1);
    assert_int_equal(gare->hypo, 0);
    assert_int_equal(p1->credit, 990); // 1100 - 110

    free(gare);
    free(p1);
}

static void test_hypotheque_et_rachat_COMP(void **state) {
    (void) state;

    Player *p1 = init_player("Joueur1", 1);
    Compagnies *comp = calloc(1, sizeof(Compagnies));
    strcpy(comp->name, "Compagnie des Eaux");
    comp->owner = 1;
    comp->value = 150; // Hypothèque = 75, Rachat = 150 + 15 = 165
    comp->hypo = 0;

    p1->credit = 1000;

    // Succès hypothèque
    assert_int_equal(hypotheque_COMP(comp, p1), 1);
    assert_int_equal(comp->hypo, 1);
    assert_int_equal(p1->credit, 1075); // 1000 + 75

    // Succès rachat
    assert_int_equal(rachathypo_COMP(comp, p1), 1);
    assert_int_equal(comp->hypo, 0);
    assert_int_equal(p1->credit, 993); // 1075 - 82

    free(comp);
    free(p1);
}
/*
printf("==========================================================\n");
printf("              TESTS DE trade                \n");
printf("==========================================================\n");
*/
static void test_init_empty_offer(void **state) {
    (void) state; // Variable inutilisée
    printf("==========================================================\n");
    printf("              TESTS DE trade                \n");
    printf("==========================================================\n");
    // 1. Initialisation d'un faux joueur pour le test
    Player *p1 = init_player("Négociateur", 1);
    assert_non_null(p1);

    // 2. Appel de la fonction à tester
    TradeOffer *offer = init_empty_offer(p1);

    // 3. Vérifications de base
    assert_non_null(offer); // On vérifie que le calloc a bien fonctionné
    assert_ptr_equal(offer->proposant, p1); // Vérifie que c'est le bon pointeur de joueur

    // 4. Vérification que "calloc" a bien tout mis à 0 et NULL
    assert_int_equal(offer->ects, 0);
    assert_int_equal(offer->nb_props, 0);
    assert_int_equal(offer->nb_gares, 0);
    assert_int_equal(offer->nb_comps, 0);
    assert_int_equal(offer->nb_cartes, 0);

    // On vérifie quelques cases des tableaux au hasard pour s'assurer du NULL
    assert_null(offer->props[0]); 
    assert_null(offer->gares[0]);
    assert_null(offer->comps[0]);
    assert_null(offer->car[0]);

    // 5. Nettoyage de la mémoire (Très important pour Valgrind)
    free(offer);
    free(p1);
}

static void test_is_trade_valid(void **state) {
    (void) state;

    // 1. Initialisation des joueurs (p1 et p2)
    Player *p1 = init_player("Alice", 1);
    Player *p2 = init_player("Bob", 2);
    p1->credit = 1000;
    p2->credit = 1000;

    // 2. Initialisation des offres vides
    TradeOffer *off1 = init_empty_offer(p1);
    TradeOffer *off2 = init_empty_offer(p2);

    // --- TEST A : Offres NULL ---
    assert_int_equal(is_trade_valid(NULL, off2), 0);
    assert_int_equal(is_trade_valid(off1, NULL), 0);

    // --- TEST B : Manque de crédits ---
    off1->ects = 1500; // Alice propose plus que ce qu'elle a (1000)
    assert_int_equal(is_trade_valid(off1, off2), 0);
    off1->ects = 0; // On remet à zéro pour la suite

    // --- TEST C : Propriété non possédée ---
    Property *prop1 = calloc(1, sizeof(Property));
    strcpy(prop1->name, "Rue de la Soif");
    prop1->owner = 2; // Appartient à Bob (Joueur 2)
    prop1->level = 0;

    // Alice essaie de mettre la propriété de Bob dans son offre (Triche)
    off1->props[0] = prop1;
    off1->nb_props = 1;
    assert_int_equal(is_trade_valid(off1, off2), 0);

    // --- TEST D : Propriété avec construction ---
    prop1->owner = 1; // La propriété appartient bien à Alice maintenant
    prop1->level = 1; // Mais elle contient un bâtiment !
    assert_int_equal(is_trade_valid(off1, off2), 0);

    // --- TEST E : Échange valide ---
    prop1->level = 0; // On retire le bâtiment, le terrain est nu
    off1->ects = 100; // Alice donne la propriété + 100 ECTS
    off2->ects = 50;  // Bob donne juste 50 ECTS

    assert_int_equal(is_trade_valid(off1, off2), 1); // Cette fois, ça doit passer !

    // 3. Nettoyage de la mémoire
    free(prop1);
    free(off1);
    free(off2);
    free(p1);
    free(p2);
}

static void test_trade_modifiers(void **state) {
    (void) state; // Variable inutilisée

    // 1. --- INITIALISATION ---
    Player *p1 = init_player("Négociateur", 1);
    TradeOffer *offer = init_empty_offer(p1);

    // On crée quelques fausses cartes pour les tests
    Property prop1 = {.owner = 1}; strcpy(prop1.name, "Rue 1");
    Property prop2 = {.owner = 1}; strcpy(prop2.name, "Rue 2");
    Property prop3 = {.owner = 1}; strcpy(prop3.name, "Rue 3");
    
    Gares gare1 = {.owner = 1}; strcpy(gare1.name, "Gare 1");
    Compagnies comp1 = {.owner = 1}; strcpy(comp1.name, "Compagnie 1");

    // 2. --- TESTS DES CRÉDITS ---
    trade_set_credits(offer, 500);
    assert_int_equal(offer->ects, 500);

    // 3. --- TESTS D'AJOUT ET DOUBLONS (PROPRIÉTÉS) ---
    // Ajout normal
    assert_int_equal(trade_add_property(offer, &prop1), 1); // Succès (retourne 1)
    assert_int_equal(offer->nb_props, 1);
    assert_ptr_equal(offer->props[0], &prop1);

    // Tentative d'ajout en doublon (doit échouer)
    assert_int_equal(trade_add_property(offer, &prop1), 0); // Échec (retourne 0)
    assert_int_equal(offer->nb_props, 1); // Le compteur n'a pas bougé

    // On ajoute prop2 et prop3 pour tester le retrait ensuite
    trade_add_property(offer, &prop2);
    trade_add_property(offer, &prop3);
    assert_int_equal(offer->nb_props, 3);
    // Le tableau est maintenant : [prop1, prop2, prop3, NULL, ...]

    // 4. --- TESTS DE RETRAIT ET DÉCALAGE (PROPRIÉTÉS) ---
    // On retire la carte du milieu (prop2) pour vérifier le décalage
    assert_int_equal(trade_remove_property(offer, &prop2), 1); // Succès
    
    // Vérifications cruciales :
    assert_int_equal(offer->nb_props, 2); // Le compteur a bien diminué
    assert_ptr_equal(offer->props[0], &prop1); // Prop1 n'a pas bougé
    assert_ptr_equal(offer->props[1], &prop3); // Prop3 a bien glissé vers la gauche !
    assert_null(offer->props[2]); // L'ancienne place de Prop3 est bien nettoyée à NULL

    // Tentative de retirer une carte qui n'est plus là
    assert_int_equal(trade_remove_property(offer, &prop2), 0); // Échec

    // 5. --- TESTS BREF SUR GARES ET COMPAGNIES ---
    // Gares
    assert_int_equal(trade_add_gare(offer, &gare1), 1);
    assert_int_equal(offer->nb_gares, 1);
    assert_int_equal(trade_remove_gare(offer, &gare1), 1);
    assert_int_equal(offer->nb_gares, 0);
    assert_null(offer->gares[0]);

    // Compagnies
    assert_int_equal(trade_add_compagnie(offer, &comp1), 1);
    assert_int_equal(offer->nb_comps, 1);
    assert_int_equal(trade_remove_compagnie(offer, &comp1), 1);
    assert_int_equal(offer->nb_comps, 0);
    assert_null(offer->comps[0]);

    // 6. --- NETTOYAGE ---
    free(offer);
    free(p1);
}

static void test_execute_trade_full(void **state) {
    (void) state;

    // 1. --- PRÉPARATION DES JOUEURS ---
    Player *p1 = init_player("Alice", 1);
    Player *p2 = init_player("Bob", 2);
    p1->credit = 1000;
    p2->credit = 1000;

    // 2. --- PRÉPARATION DES BIENS ---
    // Une propriété pour Alice
    Property *prop_alice = calloc(1, sizeof(Property));
    strcpy(prop_alice->name, "Rue de la Paix");
    prop_alice->owner = 1; 
    prop_alice->level = 0; // Important pour que l'échange soit valide

    // Une gare pour Bob
    Gares *gare_bob = calloc(1, sizeof(Gares));
    strcpy(gare_bob->name, "Gare de Lyon");
    gare_bob->owner = 2;
    gare_bob->hypo = 0;

    // 3. --- PRÉPARATION DES OFFRES ---
    TradeOffer *off1 = init_empty_offer(p1);
    TradeOffer *off2 = init_empty_offer(p2);

    // Alice donne : Rue de la Paix + 200 ECTS
    off1->ects = 200;
    trade_add_property(off1, prop_alice);

    // Bob donne : Sa Gare + 0 ECTS
    off2->ects = 0;
    trade_add_gare(off2, gare_bob);

    // 4. --- EXÉCUTION ---
    // On vérifie d'abord que c'est valide (pour être sûr)
    assert_int_equal(is_trade_valid(off1, off2), 1);
    
    // On exécute l'échange
    execute_trade(off1, off2);

    // 5. --- VÉRIFICATIONS (ASSERTIONS) ---
    
    // A. Vérification de l'argent
    // Alice : 1000 - 200 = 800
    assert_int_equal(p1->credit, 800);
    // Bob : 1000 + 200 = 1200
    assert_int_equal(p2->credit, 1200);

    // B. Vérification des Propriétés (Le changement de propriétaire)
    // La rue d'Alice doit maintenant appartenir à Bob (numéro 2)
    assert_int_equal(prop_alice->owner, 2);
    
    // La gare de Bob doit maintenant appartenir à Alice (numéro 1)
    assert_int_equal(gare_bob->owner, 1);

    // C. Vérification de la sécurité (l'offre doit être consommée ou nettoyée )
    // Ici, on vérifie juste que les pointeurs sont toujours là si on n'a pas fait de free
    assert_ptr_equal(off1->props[0], prop_alice);

    // 6. --- NETTOYAGE FINAL ---
    free(off1);
    free(off2);
    free(prop_alice);
    free(gare_bob);
    free(p1);
    free(p2);
}

/*
printf("==========================================================\n");
printf("              TESTS DE prison              \n");
printf("==========================================================\n");
*/

static void test_init_prison(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE prison              \n");
    printf("==========================================================\n");
    Prison *jail = init_prison();
    assert_non_null(jail);
    assert_int_equal(jail->nb_j, 0);
    free(jail);
}

static void test_go_to_jail_and_limit(void **state) {
    (void) state;
    Prison *jail = init_prison();
    Player * p1 = init_player("p1", 1); // Création de 5 joueurs factices
    Player * p2 = init_player("p2", 2);
    Player * p3 = init_player("p3", 3);
    Player * p4 = init_player("p4", 4);
    Player * p5 = init_player("p5", 5);

    // On ajoute 4 joueurs (la limite NB_J_MAX)
    go_to_jail(p1, jail);
    go_to_jail(p2, jail);
    go_to_jail(p3, jail);
    go_to_jail(p4, jail);
    
    assert_int_equal(jail->nb_j, 4);
    assert_int_equal(is_in_jail(p4, jail), 1);

    // On essaie d'en ajouter un 5ème (ne doit pas planter et ne doit pas s'ajouter)
    go_to_jail(p5, jail);
    assert_int_equal(jail->nb_j, 4); 
    assert_int_equal(is_in_jail(p5, jail), 0);
}

static void test_get_out_of_jail(void **state) {
    (void) state;
    Prison *jail = init_prison();
    Player * p1 = init_player("p1", 1);
    Player * p2 = init_player("p2", 2);
    Player * p3 = init_player("p3", 3);
    
    go_to_jail(p1, jail);
    go_to_jail(p2, jail);
    go_to_jail(p3, jail);
    
    // p2 sort de prison (il est au milieu de la liste, teste bien le décalage)
    get_out_of_jail(p2, jail);
    
    assert_int_equal(jail->nb_j, 2);
    assert_int_equal(is_in_jail(p2, jail), 0); // p2 n'y est plus
    assert_int_equal(is_in_jail(p1, jail), 1); // p1 y est toujours
    assert_int_equal(is_in_jail(p3, jail), 1); // p3 y est toujours
    
    // p3 devrait avoir été décalé à l'indice 1
    assert_ptr_equal(jail->liste[1], p3);
    free(p1);
    free(p2);
    free(p3);
    free(jail);
}

static void test_tours_in_jail(void **state) {
    (void) state;
    Prison *jail = init_prison();
    Player  * p1 = init_player("p1", 1);
    
    go_to_jail(p1, jail);
    
    assert_int_equal(istreetour_in_jail(p1, jail), 0); // 0 tour
    
    add_tour_jail(p1, jail);
    assert_int_equal(istreetour_in_jail(p1, jail), 0); // 1 tour
    
    add_tour_jail(p1, jail);
    add_tour_jail(p1, jail);
    
    // Après 3 appels à add_tour_jail, le joueur a atteint 3 tours
    assert_int_equal(istreetour_in_jail(p1, jail), 1);
    free(p1);
    free(jail);
}

static void test_pay_to_get_out(void **state) {
    (void) state;
    Prison *jail = init_prison();
    Player * p1 = init_player("p1", 1);
    
    go_to_jail(p1, jail);
    
    
    pay_to_get_out(p1, jail);
    assert_int_equal(p1->credit, 1450);
    
    // Le joueur ne doit plus être en prison
    assert_int_equal(is_in_jail(p1, jail), 0);
    assert_int_equal(jail->nb_j, 0);
    free(jail);
    free(p1);
}
/*
printf("==========================================================\n");
printf("              TESTS DE voe               \n");
printf("==========================================================\n");
*/
static void test_auction_system(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE voe               \n");
    printf("==========================================================\n");
    // 1. --- PRÉPARATION DU JEU ---
    Player *p1 = init_player("Alice (Refuse)", 1);
    Player *p2 = init_player("Bob", 2);
    Player *p3 = init_player("Charlie", 3);
    Player *p4 = init_player("David", 4);
    
    p2->credit = 1000;
    p3->credit = 100; // Charlie est pauvre pour tester la sécurité
    
    Player *tous_les_joueurs[4] = {p1, p2, p3, p4};

    Property *prop = calloc(1, sizeof(Property));
    strcpy(prop->name, "Rue de la Soif");
    prop->owner = 0; // Appartient à la banque

    // 2. --- INITIALISATION ---
    // Alice (p1) tombe dessus et refuse de l'acheter.
    AuctionState *auction = init_auction_prop(prop, p1, tous_les_joueurs, 4);

    assert_non_null(auction);
    assert_int_equal(auction->players_left, 3); // 4 joueurs - Alice = 3
    assert_int_equal(auction->current_bid, 10);
    assert_ptr_equal(auction->prop_item, prop);
    assert_null(auction->gar_item);

    // Alice essaie de parier (doit échouer car exclue)
    assert_int_equal(process_bid(auction, p1, 20), 0);

    // 3. --- BATAILLE D'ENCHÈRES ---
    // Bob propose 50 (Succès)
    assert_int_equal(process_bid(auction, p2, 50), 1);
    assert_ptr_equal(auction->highest_bidder, p2);

    // Charlie propose 20 (Échec : trop bas)
    assert_int_equal(process_bid(auction, p3, 20), 0);
    
    // Charlie propose 200 (Échec : il n'a que 100 crédits)
    assert_int_equal(process_bid(auction, p3, 200), 0);

    // 4. --- ABANDONS ---
    fold_player(auction, p3); // Charlie se couche
    fold_player(auction, p4); // David se couche
    assert_int_equal(auction->players_left, 1); // Il ne reste que Bob !

    // 5. --- FINALISATION ---
    finalize_auction(auction);

    // Vérifications : Bob doit avoir payé 50 ECTS et reçu la carte
    assert_int_equal(p2->credit, 950); // 1000 - 50
    assert_int_equal(prop->owner, 2);  // Appartient maintenant à Bob

    // 6. --- NETTOYAGE ---
    free(auction);
    free(prop);
    free(p1); free(p2); free(p3); free(p4);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        //property
        cmocka_unit_test(test_init_Property_basic),
        cmocka_unit_test(test_init_GARE_basic),
        cmocka_unit_test(test_init_COMPAGNIE_basic),
        cmocka_unit_test(test_chargement_complet),
        cmocka_unit_test(test_chargement_GARE),
        cmocka_unit_test(test_chargement_COMPAGNIE),
        cmocka_unit_test(test_must_rent_Property),
        cmocka_unit_test(test_must_rent_COMP),
        cmocka_unit_test(test_must_rent_GARE),
        cmocka_unit_test(test_add_level_integration),
        cmocka_unit_test(test_check_uniformity_build),
        cmocka_unit_test(test_check_uniformity_destroy),
        cmocka_unit_test(test_remove_level_integration),
        cmocka_unit_test(test_full_color),
        cmocka_unit_test(test_full_comp),
        cmocka_unit_test(test_nb_Gare),
        //player
        cmocka_unit_test(test_init_player_values),
        cmocka_unit_test(test_calcul_moyenne_eco_index_player),
        cmocka_unit_test(test_player_move),
        cmocka_unit_test(test_player_credits_and_faillite),
        cmocka_unit_test(test_player_acquisition_propriete),
        cmocka_unit_test(test_player_addremove_comp),
        cmocka_unit_test(test_player_addremove_gar),
        //tool
        cmocka_unit_test(test_underscore_conversion),
        //hypo
        cmocka_unit_test(test_hypotheque_et_rachat_PROP),
        cmocka_unit_test(test_hypotheque_et_rachat_GARE),
        cmocka_unit_test(test_hypotheque_et_rachat_COMP),
        //trade
        cmocka_unit_test(test_init_empty_offer),
        cmocka_unit_test(test_is_trade_valid),
        cmocka_unit_test(test_trade_modifiers),
        cmocka_unit_test(test_execute_trade_full),
        //prison
        cmocka_unit_test(test_init_prison),
        cmocka_unit_test(test_go_to_jail_and_limit),
        cmocka_unit_test(test_get_out_of_jail),
        cmocka_unit_test(test_tours_in_jail),
        cmocka_unit_test(test_pay_to_get_out),
        //voe
        cmocka_unit_test(test_auction_system)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}