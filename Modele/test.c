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
#include "evenements.h"
#include "cartes.h"

/* ========================================================================= */
/* TESTS DE PROPERTY                                */
/* ========================================================================= */

static void test_init_Property_basic(void **state) {
    (void) state; 
    printf("==========================================================\n");
    printf("              TESTS DE property                \n");
    printf("==========================================================\n");
    
    int rm[3] = {15, 45, 135};
    int rl[3] = {10, 30, 90};
    int rp[3] = {3, 8, 23};

    Property *p = init_Property("A", MARRON, 0, 60, 50, 2, rm, rl, rp, 0, 0, 10);
    assert_non_null(p);
    assert_string_equal(p->name, "A");
    assert_int_equal(p->color, MARRON);
    assert_int_equal(p->value, 60);
    assert_int_equal(p->construction_cost, 50);
    assert_int_equal(p->rent_empty, 2);
    assert_int_equal(p->rent_maison[2], 135);
    assert_int_equal(p->rent_logement[1], 30);
    assert_int_equal(p->rent_parc[0], 3);
    assert_int_equal(p->level, 0);
    assert_int_equal(p->indice, 10);

    free(p);
}

static void test_init_COMPAGNIE_basic(void **state) {
    (void) state; 
    int rm[2] = {4, 10};
    Compagnies *p = init_Compagnies("A", COMP, 0, 150, rm);
    
    assert_non_null(p);
    assert_string_equal(p->name, "A");
    assert_int_equal(p->color, COMP);
    assert_int_equal(p->value, 150);
    assert_int_equal(p->rent[1], 10);
    assert_int_equal(p->rent[0], 4);

    free(p);
}

static void test_init_GARE_basic(void **state) {
    (void) state;
    int rm[4] = {25, 50, 100, 200};
    Gares *p = init_Gares("A", GAR, 0, 200, rm);
    
    assert_non_null(p);
    assert_string_equal(p->name, "A");
    assert_int_equal(p->color, GAR);
    assert_int_equal(p->value, 200);
    assert_int_equal(p->rent[3], 200);
    assert_int_equal(p->rent[2], 100);
    assert_int_equal(p->rent[1], 50);
    assert_int_equal(p->rent[0], 25);

    free(p);
}

static void test_chargement_complet(void **state) {
    (void) state;
    Property *plateau[NB_PROPRIETE] = {NULL};
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt");

    assert_int_equal(nb, 22);
    assert_non_null(plateau[0]);
    assert_string_equal(plateau[0]->name, "LongsChamps");
    assert_int_equal(plateau[0]->color, MARRON);
    assert_int_equal(plateau[0]->value, 60);

    assert_non_null(plateau[2]);
    assert_string_equal(plateau[2]->name, "Jeanne d'Arc");
    assert_int_equal(plateau[2]->color, GRIS); 
    assert_int_equal(plateau[2]->rent_empty, 6);

    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

static void test_chargement_GARE(void **state) {
    (void) state;
    Gares *plateau[NB_GARE] = {NULL};
    int nb = chargement_GARE_loyer(plateau, "gares.txt");

    assert_int_equal(nb, 4);
    assert_non_null(plateau[0]);
    assert_string_equal(plateau[0]->name, "Beaulieu-Université");
    assert_int_equal(plateau[0]->color, GAR);
    assert_int_equal(plateau[0]->value, 200);

    assert_non_null(plateau[2]);
    assert_string_equal(plateau[2]->name, "Gare de Rennes");
    assert_int_equal(plateau[2]->color, GAR); 
    assert_int_equal(plateau[2]->rent[2], 100);
    assert_int_equal(plateau[3]->rent[3], 200);

    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

static void test_chargement_COMPAGNIE(void **state) {
    (void) state;
    Compagnies *plateau[NB_COMPAGNIE] = {NULL};
    int nb = chargement_COMPAGNIE_loyer(plateau, "compagnie.txt");

    assert_int_equal(nb, 2);
    assert_non_null(plateau[0]);
    assert_string_equal(plateau[0]->name, "Foy");
    assert_int_equal(plateau[0]->color, COMP);
    assert_int_equal(plateau[0]->value, 150);
    assert_non_null(plateau[1]);
    assert_string_equal(plateau[1]->name, "K'Fet");
    assert_int_equal(plateau[1]->color, COMP);
    assert_int_equal(plateau[1]->rent[0], 4);
    assert_int_equal(plateau[1]->rent[1], 10);

    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

static void test_must_rent_Property(void **state){
    (void) state;
    Player * proprio_test1 = init_player("proprio_test",1);
    Player * proprio_test2 = init_player("proprio_test",1);
    Player * loca_test = init_player("loca_test",2);
    Property * plateau[NB_PROPRIETE] = {NULL}; 
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); 
    
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
    
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
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

    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
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

    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
    free(proprio_test1);
    free(loca_test);
}

static void test_add_level_integration(void **state) {
    (void) state;
    Player * p_test = init_player("Bâtisseur", 1);
    p_test->credit = 2000; 

    Property * plateau[NB_PROPRIETE] = {NULL};
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt");

    player_add_property(p_test, plateau[8]);
    player_add_property(p_test, plateau[9]);
    player_add_property(p_test, plateau[10]);

    int ancien_solde = p_test->credit;
    int cout_const = plateau[8]->construction_cost;

    int type_voulu = 1; 
    int success = add_level(plateau, plateau[8], p_test, type_voulu);

    assert_int_equal(success, 1);                 
    assert_int_equal(plateau[8]->level, 1);       
    assert_int_equal(plateau[8]->type, type_voulu); 
    assert_int_equal(p_test->credit, ancien_solde - cout_const); 

    plateau[8]->level = 3;
    int fail = add_level(plateau, plateau[8], p_test, type_voulu);
    assert_int_equal(fail, 0); 
    assert_int_equal(plateau[8]->level, 3); 

    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
    free(p_test);
}

static void test_check_uniformity_build(void **state) {
    (void) state;
    Property *plateau[NB_PROPRIETE] = {NULL};
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); 
    Property *p1 = plateau[0] ; 
    Property *p2 = plateau[1] ;

    assert_int_equal(check_uniformity(plateau, p1, 1), 1);

    p1->level = 1;
    assert_int_equal(check_uniformity(plateau, p1, 1), 0);
    assert_int_equal(check_uniformity(plateau, p2, 1), 1);
    
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

static void test_check_uniformity_destroy(void **state) {
    (void) state;
    Property *plateau[NB_PROPRIETE] = {NULL};
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); 
    Property *p1 = plateau[0];
    Property *p2 = plateau[1];
    p1->type=1; p2->type=1;
    p1->level=1; p2->level=1;

    assert_int_equal(check_uniformity(plateau, p1, -1), 1);

    p1->level = 0;
    assert_int_equal(check_uniformity(plateau, p2, -1), 1);
    
    p1->level = 2; p2->level = 1;
    assert_int_equal(check_uniformity(plateau, p2, -1), 0);
    
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
}

static void test_remove_level_integration(void **state) {
    (void) state;
    Player *p = init_player("Test", 1);
    p->credit = 1000;
    Property *plateau[NB_PROPRIETE] = {NULL};
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); 

    Property *prop = plateau[0] ;
    prop->construction_cost = 200;
    prop->type=1;
    prop->level=1;
    prop->owner = p->number;
    
    int success = remove_level(plateau, prop, p);
    assert_int_equal(success, 1);
    assert_int_equal(prop->level, 0);
    assert_int_equal(prop->type, 0); 
    assert_int_equal(p->credit, 1200);

    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
    free(p);
}

static void test_full_color(void **state) {
    (void) state;
    Player *p1 = init_player("Testfullcolor1", 1);
    Player *p2 = init_player("Testfullcolor2", 2);
    Property * plateau[NB_PROPRIETE] = {NULL}; 
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); 

    player_add_property(p1, plateau[0]);
    player_add_property(p1, plateau[1]);
    player_add_property(p2, plateau[2]);

    assert_int_equal(full_color(plateau,p1,MARRON),1);
    assert_int_equal(full_color(plateau,p2,GRIS),0);
    
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL) free(plateau[i]);
    }
    free(p1); free(p2);
}

static void test_full_comp(void **state){
    (void) state;
    Player *p1 = init_player("Testfullcolor1", 1);
    Compagnies * tableau[NB_COMPAGNIE] = {NULL};
    int nb = chargement_COMPAGNIE_loyer(tableau, "compagnie.txt");
    
    player_add_comp(p1,tableau[0]);
    assert_int_equal(full_Comp(tableau, p1),0);
    player_add_comp(p1,tableau[1]);
    assert_int_equal(full_Comp(tableau, p1),1);
    
    for (int i = 0; i < nb; i++) {
        if (tableau[i] != NULL) free(tableau[i]);
    }
    free(p1);
}

static void test_nb_Gare(void **state){
    (void) state;
    Player *p1 = init_player("Testnbg", 1);
    Gares * tableau[NB_GARE] = {NULL};
    int nb = chargement_GARE_loyer(tableau, "gares.txt");
    
    assert_int_equal(nb_Gare(tableau, p1),0);   
    player_add_gar(p1,tableau[0]);
    assert_int_equal(nb_Gare(tableau, p1),1);
    player_add_gar(p1,tableau[1]);
    assert_int_equal(nb_Gare(tableau, p1),2);
    player_add_gar(p1,tableau[2]);
    assert_int_equal(nb_Gare(tableau, p1),3);
    player_add_gar(p1,tableau[3]);
    assert_int_equal(nb_Gare(tableau, p1),4);
    
    for (int i = 0; i < nb; i++) {
        if (tableau[i] != NULL) free(tableau[i]);
    }
    free(p1);
}

/* NOUVEAU : Test des branches NULL et cas d'erreurs orphelins (Property) */
static void test_property_null_and_errors(void **state) {
    (void) state;
    Property *plateau[NB_PROPRIETE] = {NULL};
    Gares *gares[NB_GARE] = {NULL};
    Compagnies *comps[NB_COMPAGNIE] = {NULL};

    // Fichiers inexistants
    assert_int_equal(chargement_propriete_loyer(plateau, "nexiste_pas.txt"), 0);
    assert_int_equal(chargement_GARE_loyer(gares, "nexiste_pas.txt"), 0);
    assert_int_equal(chargement_COMPAGNIE_loyer(comps, "nexiste_pas.txt"), 0);

    // Tests de sécurité NULL
    assert_int_equal(check_uniformity(plateau, NULL, 1), 0);
}

/* NOUVEAU : Couverture des types Logement/Parc et Faillite (Banqueroute) */
static void test_property_types_and_banqueroute(void **state) {
    (void) state;
    Player *p1 = init_player("P1", 1);
    Player *p2 = init_player("P2_Pauvre", 2);

    // SOLUTION : On charge les vrais tableaux pour éviter les pointeurs NULL
    Property *plateau[NB_PROPRIETE] = {NULL};
    int nb_p = chargement_propriete_loyer(plateau, "loyer_insapoly.txt");
    
    Property *prop = plateau[0];
    Property *prop2 = plateau[1];
    prop->owner = 1;
    prop2->owner = 1; // Simulation de la couleur complète (Full Color)
    
    // Type Social
    prop->type = Social; prop->level = 2; 
    prop->rent_logement[1] = 10; // On force le loyer pour le calcul exact
    p2->credit = 50;
    must_rent_Property(plateau, prop, p2, p1);
    assert_int_equal(p2->credit, 40); // 50 - 10 = 40

    // Type Parc
    prop->type = Parc; prop->level = 3; 
    prop->rent_parc[2] = 6;
    must_rent_Property(plateau, prop, p2, p1);
    assert_int_equal(p2->credit, 34); // 40 - 6 = 34

    // Banqueroute sur Propriété
    p2->credit = 5; // Moins que le loyer
    must_rent_Property(plateau, prop, p2, p1); // Doit afficher BANQUEROUTE
    assert_int_equal(p2->credit, 0); // Le crédit s'arrête à 0

    // Banqueroute sur Compagnie
    Compagnies *comps[NB_COMPAGNIE] = {NULL};
    int nb_c = chargement_COMPAGNIE_loyer(comps, "compagnie.txt");
    Compagnies *comp = comps[0];
    comp->owner = 1;
    
    p2->credit = 5;
    must_rent_Compagnies(comps, comp, p2, p1, 2); 
    assert_int_equal(p2->credit, 0); // Banqueroute
    
    // Banqueroute sur Gare
    Gares *gares[NB_GARE] = {NULL};
    int nb_g = chargement_GARE_loyer(gares, "gares.txt");
    Gares *gare = gares[0];
    gare->owner = 1;
    
    p2->credit = 5;
    must_rent_Gares(gares, gare, p2, p1); 
    assert_int_equal(p2->credit, 0); // Banqueroute

    // Echec remove_level (niveau 0)
    prop->level = 0;
    assert_int_equal(remove_level(plateau, prop, p1), 0);

    // Echec remove_level (mauvais propriétaire)
    prop->level = 1; prop->owner = 2;
    assert_int_equal(remove_level(plateau, prop, p1), 0);
    
    // add_level avec type Social et Parc
    prop->level = 0; prop->owner = 1;
    add_level(plateau, prop, p1, Social); 
    assert_int_equal(prop->type, Social);
    
    prop->level = 0;
    add_level(plateau, prop, p1, Parc);
    assert_int_equal(prop->type, Parc);

    // Impossible d'ajouter si couleur incomplète
    prop2->owner = 2; // P2 prend l'autre carte de la couleur
    assert_int_equal(add_level(plateau, prop, p1, Maison), 0); // P1 ne peut plus construire

    // Nettoyage complet
    for (int i = 0; i < nb_p; i++) if (plateau[i]) free(plateau[i]);
    for (int i = 0; i < nb_c; i++) if (comps[i]) free(comps[i]);
    for (int i = 0; i < nb_g; i++) if (gares[i]) free(gares[i]);
    free(p1); free(p2);
}

/* ========================================================================= */
/* TESTS DE PLAYER                                  */
/* ========================================================================= */

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

    free(p);
}

static void test_calcul_moyenne_eco_index_player(void **state) {
    (void) state;
    Player *p = init_player("Ecolo", 1);
    Property * board[2];
    
    board[0] = calloc(1, sizeof(Property));
    board[1] = calloc(1, sizeof(Property));
    board[0]->owner = 1; 
    board[0]->indice = GRADE_B;
    board[1]->owner = 1; 
    board[1]->indice = GRADE_D;
    
    calcul_moyenne_eco_index_player(p, board, 2);
    assert_int_equal(p->eco_index, GRADE_C);
    
    free(p);
    free(board[0]); free(board[1]);
}

static void test_player_move(void **state) {
    (void) state;
    Player *p = init_player("TestMove", 1);
    p->position = 35;
    p->credit = 1000;

    player_move(p, 10); 
    assert_int_equal(p->position, 5);
    
    free(p);
}

static void test_player_credits_and_faillite(void **state) {
    (void) state;
    Player *p = init_player("Test player crédit", 1);
    p->credit = 100;

    player_add_credits(p, 50);
    assert_int_equal(p->credit, 150);

    int success = player_remove_credits(p, 200);
    assert_int_equal(success, 0); 
    assert_int_equal(p->credit, 0);

    free(p);
}

static void test_player_acquisition_propriete(void **state) {
    (void) state;
    Player *p = init_player("Owner", 1);
    int rm[3]={0}, rl[3]={0}, rp[3]={0};
    Property *prop = init_Property("Test Prop", MARRON, -1, 60, 50, 2, rm, rl, rp, 0, 0, 10);

    player_add_property(p, prop);
    assert_int_equal(prop->owner, p->number);

    player_remove_property(p, prop);
    assert_int_equal(prop->owner, 0);

    free(p); free(prop);
}

static void test_player_addremove_comp(void **state){
    (void) state;
    Player *p = init_player("Owner", 1);
    int loyer[2] = {4,10};
    Compagnies * c = init_Compagnies("test comp", COMP, 0, 150, loyer);
    
    player_add_comp(p,c);
    assert_int_equal(c->owner, p->number);
    
    player_remove_comp(p, c);
    assert_int_equal(c->owner, 0);

    free(p); free(c);
}

static void test_player_addremove_gar(void **state){
    (void) state;
    Player *p = init_player("Owner", 1);
    int loyer[4] = {25,50,100,200};
    Gares * g = init_Gares("test gare", COMP, 0, 200, loyer);
    
    player_add_gar(p,g);
    assert_int_equal(g->owner, p->number);
    
    player_remove_gar(p, g);
    assert_int_equal(g->owner, 0);

    free(p); free(g);
}

/* NOUVEAU : Test de toutes les conditions de sécurité (Player) */
static void test_player_edge_cases(void **state) {
    (void) state;
    Player *p = init_player("P", 1);
    Property *prop = calloc(1, sizeof(Property));

    // Tests de NULL
    calcul_moyenne_eco_index_player(NULL, &prop, 1);
    calcul_moyenne_eco_index_player(p, NULL, 1);
    player_move(NULL, 5);
    player_add_credits(NULL, 10);
    assert_int_equal(player_remove_credits(NULL, 10), 1);
    player_add_property(NULL, prop);
    player_add_property(p, NULL);
    player_add_comp(NULL, NULL);
    player_add_gar(NULL, NULL);
    player_remove_property(NULL, prop);
    player_remove_property(p, NULL);
    player_remove_comp(NULL, NULL);
    player_remove_gar(NULL, NULL);

    // Test extrêmes sur la moyenne Eco
    prop->owner = 1;
    prop->indice = 0; // Sous le seuil GRADE_A
    Property *board[1] = {prop};
    calcul_moyenne_eco_index_player(p, board, 1);
    assert_int_equal(p->eco_index, GRADE_A);

    prop->indice = 10; // Au-dessus de GRADE_F
    calcul_moyenne_eco_index_player(p, board, 1);
    assert_int_equal(p->eco_index, GRADE_F);

    // Le joueur ne possède rien
    prop->owner = 2; 
    calcul_moyenne_eco_index_player(p, board, 1);
    assert_int_equal(p->eco_index, GRADE_A);

    free(prop); free(p);
}

/* ========================================================================= */
/* TESTS DE TOOL                                    */
/* ========================================================================= */

static void test_underscore_conversion(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE tool                \n");
    printf("==========================================================\n");
    char nom_test[] = "Rue_de_Vaugirard";
    replace_underscore_with_space(nom_test);
    assert_string_equal(nom_test, "Rue de Vaugirard");
}

/* ========================================================================= */
/* TESTS DE HYPO                                    */
/* ========================================================================= */

static void test_hypotheque_et_rachat_PROP(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE hypo                \n");
    printf("==========================================================\n");
    Player *p1 = init_player("Joueur1", 1);
    Player *p2 = init_player("Joueur2", 2);
    
    Property *prop = calloc(1, sizeof(Property));
    strcpy(prop->name, "Rue de la Soif");
    prop->owner = 1;      
    prop->value = 200;    
    prop->hypo = 0;
    prop->level = 0;

    assert_int_equal(hypotheque_PROP(prop, p2), 0); 
    
    prop->level = 1; 
    assert_int_equal(hypotheque_PROP(prop, p1), 0); 
    prop->level = 0; 

    p1->credit = 1000;
    assert_int_equal(hypotheque_PROP(prop, p1), 1); 
    assert_int_equal(prop->hypo, 1);
    assert_int_equal(p1->credit, 1100); 

    assert_int_equal(hypotheque_PROP(prop, p1), 0); 

    assert_int_equal(rachathypo_PROP(prop, p2), 0); 
    
    p1->credit = 100; 
    assert_int_equal(rachathypo_PROP(prop, p1), 0);
    assert_int_equal(prop->hypo, 1); 

    p1->credit = 1000;
    assert_int_equal(rachathypo_PROP(prop, p1), 1); 
    assert_int_equal(prop->hypo, 0); 
    assert_int_equal(p1->credit, 890); 

    free(prop); free(p1); free(p2);
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

    assert_int_equal(hypotheque_GARE(gare, p1), 1);
    assert_int_equal(gare->hypo, 1);
    assert_int_equal(p1->credit, 1100);

    assert_int_equal(rachathypo_GARE(gare, p1), 1);
    assert_int_equal(gare->hypo, 0);
    assert_int_equal(p1->credit, 990); 

    free(gare); free(p1);
}

static void test_hypotheque_et_rachat_COMP(void **state) {
    (void) state;
    Player *p1 = init_player("Joueur1", 1);
    Compagnies *comp = calloc(1, sizeof(Compagnies));
    strcpy(comp->name, "Compagnie des Eaux");
    comp->owner = 1;
    comp->value = 150; 
    comp->hypo = 0;
    p1->credit = 1000;

    assert_int_equal(hypotheque_COMP(comp, p1), 1);
    assert_int_equal(comp->hypo, 1);
    assert_int_equal(p1->credit, 1075); 

    assert_int_equal(rachathypo_COMP(comp, p1), 1);
    assert_int_equal(comp->hypo, 0);
    assert_int_equal(p1->credit, 993); 

    free(comp); free(p1);
}

/* NOUVEAU : Test des branches NULL et cas d'erreurs orphelins (Hypo) */
static void test_hypo_edge_cases(void **state) {
    (void) state;
    Player *p1 = init_player("P1", 1);
    Player *p2 = init_player("P2", 2);

    // Tests de NULL
    assert_int_equal(hypotheque_PROP(NULL, p1), 0);
    assert_int_equal(hypotheque_PROP(NULL, NULL), 0);
    assert_int_equal(rachathypo_PROP(NULL, p1), 0);
    assert_int_equal(hypotheque_GARE(NULL, p1), 0);
    assert_int_equal(rachathypo_GARE(NULL, p1), 0);
    assert_int_equal(hypotheque_COMP(NULL, p1), 0);
    assert_int_equal(rachathypo_COMP(NULL, p1), 0);

    // COMP : Mauvais proprio, deja hypo, pas hypo pour rachat, pas d'argent
    int rc[2] = {10, 20};
    Compagnies *comp = init_Compagnies("Comp", COMP, 1, 100, rc);
    assert_int_equal(hypotheque_COMP(comp, p2), 0); 
    assert_int_equal(rachathypo_COMP(comp, p1), 0); 
    
    hypotheque_COMP(comp, p1);
    assert_int_equal(hypotheque_COMP(comp, p1), 0); 
    
    p1->credit = 0;
    assert_int_equal(rachathypo_COMP(comp, p1), 0); 

    // GARE : Mauvais proprio, deja hypo, pas hypo pour rachat, pas d'argent
    int rg[4] = {25, 50, 100, 200};
    Gares *gare = init_Gares("Gare", GAR, 1, 200, rg);
    assert_int_equal(hypotheque_GARE(gare, p2), 0); 
    assert_int_equal(rachathypo_GARE(gare, p1), 0); 
    
    p1->credit = 1000;
    hypotheque_GARE(gare, p1);
    assert_int_equal(hypotheque_GARE(gare, p1), 0); 
    
    p1->credit = 0;
    assert_int_equal(rachathypo_GARE(gare, p1), 0); 

    free(comp); free(gare); free(p1); free(p2);
}

/* ========================================================================= */
/* TESTS DE TRADE                                   */
/* ========================================================================= */

static void test_init_empty_offer(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE trade                \n");
    printf("==========================================================\n");
    Player *p1 = init_player("Négociateur", 1);
    assert_non_null(p1);

    TradeOffer *offer = init_empty_offer(p1);

    assert_non_null(offer); 
    assert_ptr_equal(offer->proposant, p1); 
    assert_int_equal(offer->ects, 0);
    assert_int_equal(offer->nb_props, 0);
    assert_int_equal(offer->nb_gares, 0);
    assert_int_equal(offer->nb_comps, 0);
    assert_int_equal(offer->nb_cartes, 0);

    assert_null(offer->props[0]); 
    assert_null(offer->gares[0]);
    assert_null(offer->comps[0]);
    assert_null(offer->car[0]);

    free(offer); free(p1);
}

static void test_is_trade_valid(void **state) {
    (void) state;
    Player *p1 = init_player("Alice", 1);
    Player *p2 = init_player("Bob", 2);
    p1->credit = 1000;
    p2->credit = 1000;

    TradeOffer *off1 = init_empty_offer(p1);
    TradeOffer *off2 = init_empty_offer(p2);
    Property * tableau_prop[NB_PROPRIETE] = {NULL};
    
    assert_int_equal(is_trade_valid(NULL, off2, tableau_prop), 0);
    assert_int_equal(is_trade_valid(off1, NULL ,tableau_prop), 0);

    off1->ects = 1500; 
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    off1->ects = 0; 

    off2->ects = 1500;
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    off2->ects = 0;

    int loyers_compagnie[2] = {10, 25};
    int loyers_gare[4] = {25, 50, 100, 200};

    Compagnies *c1 = init_Compagnies("foyer", 8, 0, 700, loyers_compagnie);
    Gares *g1 = init_Gares("gare de lyon", 9, 0, 2000, loyers_gare);
    Property *prop1 = calloc(1, sizeof(Property));
    strcpy(prop1->name, "Rue de la Soif");
    prop1->owner = 2; 
    prop1->level = 0;
    prop1->color = 1;
    tableau_prop[0] = prop1;
    
    trade_add_property(off1, prop1);
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    trade_remove_property(off1, prop1);

    prop1->owner = 1; 
    trade_add_property(off2, prop1);
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    trade_remove_property(off2, prop1);

    c1->owner = 2; 
    trade_add_compagnie(off1, c1); 
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    trade_remove_compagnie(off1, c1);
    
    c1->owner = 1; 
    trade_add_compagnie(off2, c1); 
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    trade_remove_compagnie(off2, c1);

    g1->owner = 2;
    trade_add_gare(off1, g1);
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    trade_remove_gare(off1, g1);

    g1->owner = 1;
    trade_add_gare(off2, g1);
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    trade_remove_gare(off2, g1);

    prop1->owner = 1; 
    prop1->level = 1; 
    trade_add_property(off1, prop1);
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    trade_remove_property(off1, prop1);
    
    prop1->owner = 2; 
    trade_add_property(off2, prop1);
    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 0);
    trade_remove_property(off2, prop1);

    prop1->level = 0; 
    trade_add_property(off1, prop1); 
    prop1->owner = 1; 
    off1->ects = 100; 
    off2->ects = 50;  

    assert_int_equal(is_trade_valid(off1, off2,tableau_prop), 1);

    free(prop1); free(c1); free(g1);
    free(off1); free(off2); free(p1); free(p2);
}

static void test_trade_modifiers(void **state) {
    (void) state; 
    Player *p1 = init_player("Négociateur", 1);
    TradeOffer *offer = init_empty_offer(p1);

    Property prop1 = {.owner = 1}; strcpy(prop1.name, "Rue 1");
    Property prop2 = {.owner = 1}; strcpy(prop2.name, "Rue 2");
    Property prop3 = {.owner = 1}; strcpy(prop3.name, "Rue 3");
    
    Gares gare1 = {.owner = 1}; strcpy(gare1.name, "Gare 1");
    Gares gare_fantome = {.owner = 1}; strcpy(gare_fantome.name, "Gare Inconnue");
    Compagnies comp1 = {.owner = 1}; strcpy(comp1.name, "Compagnie 1");
    Compagnies comp_fantome = {.owner = 1}; strcpy(comp_fantome.name, "Compagnie Inconnue");

    trade_set_credits(NULL, 500);
    assert_int_equal(trade_add_property(NULL, &prop1), 0);
    assert_int_equal(trade_add_property(offer, NULL), 0);
    assert_int_equal(trade_remove_property(NULL, &prop1), 0);
    assert_int_equal(trade_add_gare(NULL, &gare1), 0);
    assert_int_equal(trade_remove_gare(offer, NULL), 0);
    assert_int_equal(trade_add_compagnie(offer, NULL), 0);
    assert_int_equal(trade_remove_compagnie(NULL, &comp1), 0);

    trade_set_credits(offer, 500);
    assert_int_equal(offer->ects, 500);

    assert_int_equal(trade_add_property(offer, &prop1), 1); 
    assert_int_equal(offer->nb_props, 1);
    assert_ptr_equal(offer->props[0], &prop1);

    assert_int_equal(trade_add_property(offer, &prop1), 0); 
    assert_int_equal(offer->nb_props, 1); 

    trade_add_property(offer, &prop2);
    trade_add_property(offer, &prop3);
    assert_int_equal(offer->nb_props, 3);

    assert_int_equal(trade_remove_property(offer, &prop2), 1); 
    assert_int_equal(offer->nb_props, 2); 
    assert_ptr_equal(offer->props[0], &prop1); 
    assert_ptr_equal(offer->props[1], &prop3); 
    assert_null(offer->props[2]); 

    assert_int_equal(trade_remove_property(offer, &prop2), 0); 

    assert_int_equal(trade_add_gare(offer, &gare1), 1);
    assert_int_equal(offer->nb_gares, 1);
    assert_int_equal(trade_remove_gare(offer, &gare1), 1);
    assert_int_equal(trade_remove_gare(offer, &gare_fantome), 0);
    assert_int_equal(offer->nb_gares, 0);
    assert_null(offer->gares[0]);

    assert_int_equal(trade_add_compagnie(offer, &comp1), 1);
    assert_int_equal(offer->nb_comps, 1);
    assert_int_equal(trade_remove_compagnie(offer, &comp1), 1);
    assert_int_equal(trade_remove_compagnie(offer, &comp_fantome), 0);
    assert_int_equal(offer->nb_comps, 0);
    assert_null(offer->comps[0]);

    free(offer); free(p1);
}

static void test_execute_trade_full(void **state) {
    (void) state;
    Player *p1 = init_player("Alice", 1);
    Player *p2 = init_player("Bob", 2);
    p1->credit = 1000;
    p2->credit = 1000;
    Property * tableau_prop[NB_PROPRIETE] = {NULL};
    
    Property *prop_alice = calloc(1, sizeof(Property));
    strcpy(prop_alice->name, "Rue de la Paix");
    prop_alice->owner = 1; prop_alice->level = 0;
    
    Property *prop_bob = calloc(1, sizeof(Property));
    strcpy(prop_bob->name, "Avenue Foch");
    prop_bob->owner = 2; prop_bob->level = 0;

    Gares *gare_alice = calloc(1, sizeof(Gares));
    strcpy(gare_alice->name, "Gare Montparnasse");
    gare_alice->owner = 1; gare_alice->hypo = 0;

    Gares *gare_bob = calloc(1, sizeof(Gares));
    strcpy(gare_bob->name, "Gare de Lyon");
    gare_bob->owner = 2; gare_bob->hypo = 0;

    Compagnies *comp_alice = calloc(1, sizeof(Compagnies));
    strcpy(comp_alice->name, "Electricité");
    comp_alice->owner = 1; comp_alice->hypo = 0;

    Compagnies *comp_bob = calloc(1, sizeof(Compagnies));
    strcpy(comp_bob->name, "Eaux");
    comp_bob->owner = 2; comp_bob->hypo = 0;

    tableau_prop[0] = prop_alice;
    tableau_prop[1] = prop_bob;

    TradeOffer *off1 = init_empty_offer(p1);
    TradeOffer *off2 = init_empty_offer(p2);

    trade_add_property(off1, prop_alice);
    trade_add_gare(off1, gare_alice);
    trade_add_compagnie(off1, comp_alice);
    off1->ects = 200;

    trade_add_property(off2, prop_bob);
    trade_add_gare(off2, gare_bob);
    trade_add_compagnie(off2, comp_bob);
    off2->ects = 50;

    off1->ects = 5000; 
    execute_trade(off1, off2, tableau_prop);
    assert_int_equal(p1->credit, 1000); 
    off1->ects = 200; 

    assert_int_equal(is_trade_valid(off1, off2, tableau_prop), 1);
    execute_trade(off1, off2, tableau_prop);

    assert_int_equal(p1->credit, 850);
    assert_int_equal(p2->credit, 1150);

    assert_int_equal(prop_alice->owner, 2);
    assert_int_equal(gare_alice->owner, 2);
    assert_int_equal(comp_alice->owner, 2);

    assert_int_equal(prop_bob->owner, 1);
    assert_int_equal(gare_bob->owner, 1);
    assert_int_equal(comp_bob->owner, 1);

    free(off1); free(off2);
    free(prop_alice); free(prop_bob);
    free(gare_alice); free(gare_bob);
    free(comp_alice); free(comp_bob);
    free(p1); free(p2);
}

/* ========================================================================= */
/* TESTS DE PRISON                                  */
/* ========================================================================= */

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
    Player * p1 = init_player("p1", 1); 
    Player * p2 = init_player("p2", 2);
    Player * p3 = init_player("p3", 3);
    Player * p4 = init_player("p4", 4);
    Player * p5 = init_player("p5", 5);

    go_to_jail(p1, jail);
    go_to_jail(p2, jail);
    go_to_jail(p3, jail);
    go_to_jail(p4, jail);
    
    assert_int_equal(jail->nb_j, 4);
    assert_int_equal(is_in_jail(p4, jail), 1);

    go_to_jail(p5, jail);
    assert_int_equal(jail->nb_j, 4); 
    assert_int_equal(is_in_jail(p5, jail), 0);
    
    free(p1); free(p2); free(p3); free(p4); free(p5);
    free(jail);
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
    
    get_out_of_jail(p2, jail);
    
    assert_int_equal(jail->nb_j, 2);
    assert_int_equal(is_in_jail(p2, jail), 0); 
    assert_int_equal(is_in_jail(p1, jail), 1); 
    assert_int_equal(is_in_jail(p3, jail), 1); 
    assert_ptr_equal(jail->liste[1], p3);
    
    free(p1); free(p2); free(p3); free(jail);
}

static void test_tours_in_jail(void **state) {
    (void) state;
    Prison *jail = init_prison();
    Player  * p1 = init_player("p1", 1);
    
    go_to_jail(p1, jail);
    assert_int_equal(istreetour_in_jail(p1, jail), 0); 
    
    add_tour_jail(p1, jail);
    assert_int_equal(istreetour_in_jail(p1, jail), 0); 
    
    add_tour_jail(p1, jail);
    add_tour_jail(p1, jail);
    assert_int_equal(istreetour_in_jail(p1, jail), 1);
    
    free(p1); free(jail);
}

static void test_pay_to_get_out(void **state) {
    (void) state;
    Prison *jail = init_prison();
    Player * p1 = init_player("p1", 1);
    
    go_to_jail(p1, jail);
    
    pay_to_get_out(p1, jail);
    assert_int_equal(p1->credit, 1450);
    assert_int_equal(is_in_jail(p1, jail), 0);
    assert_int_equal(jail->nb_j, 0);
    
    free(jail); free(p1);
}

/* ========================================================================= */
/* TESTS DE VOE                                     */
/* ========================================================================= */

static void test_auction_system(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE voe               \n");
    printf("==========================================================\n");
    Player *p1 = init_player("Alice (Refuse)", 1);
    Player *p2 = init_player("Bob", 2);
    Player *p3 = init_player("Charlie", 3);
    Player *p4 = init_player("David", 4);
    
    p2->credit = 1000;
    p3->credit = 100; 
    
    Player *tous_les_joueurs[4] = {p1, p2, p3, p4};

    Property *prop = calloc(1, sizeof(Property));
    strcpy(prop->name, "Rue de la Soif");
    prop->owner = 0; 

    AuctionState *auction = init_auction_prop(prop, p1, tous_les_joueurs, 4);

    assert_non_null(auction);
    assert_int_equal(auction->players_left, 3); 
    assert_int_equal(auction->current_bid, 10);
    assert_ptr_equal(auction->prop_item, prop);
    assert_null(auction->gar_item);

    assert_int_equal(process_bid(auction, p1, 20), 0);

    assert_int_equal(process_bid(auction, p2, 50), 1);
    assert_ptr_equal(auction->highest_bidder, p2);

    assert_int_equal(process_bid(auction, p3, 20), 0);
    assert_int_equal(process_bid(auction, p3, 200), 0);

    fold_player(auction, p3); 
    fold_player(auction, p4); 
    assert_int_equal(auction->players_left, 1); 

    finalize_auction(auction);

    assert_int_equal(p2->credit, 950); 
    assert_int_equal(prop->owner, 2);  

    free(auction); free(prop);
    free(p1); free(p2); free(p3); free(p4);
}

static void test_auction_system_comp(void **state) {
    (void) state;
    Player *p1 = init_player("Alice (Refuse)", 1);
    Player *p2 = init_player("Bob", 2);
    Player *p3 = init_player("Charlie", 3);
    Player *p4 = init_player("David", 4);
    
    p2->credit = 1000;
    p3->credit = 100; 
    
    Player *tous_les_joueurs[4] = {p1, p2, p3, p4};

    Compagnies *prop = calloc(1, sizeof(Compagnies));
    strcpy(prop->name, "Rue de la Soif");
    prop->owner = 0; 

    AuctionState *auction = init_auction_comp(prop, p1, tous_les_joueurs, 4);

    assert_non_null(auction);
    assert_int_equal(auction->players_left, 3); 
    assert_int_equal(auction->current_bid, 10);
    assert_ptr_equal(auction->comp_item, prop);
    assert_null(auction->gar_item);

    assert_int_equal(process_bid(auction, p1, 20), 0);
    assert_int_equal(process_bid(auction, p2, 50), 1);
    assert_ptr_equal(auction->highest_bidder, p2);
    assert_int_equal(process_bid(auction, p3, 20), 0);
    assert_int_equal(process_bid(auction, p3, 200), 0);

    fold_player(auction, p3); 
    fold_player(auction, p4); 
    assert_int_equal(auction->players_left, 1); 

    finalize_auction(auction);

    assert_int_equal(p2->credit, 950); 
    assert_int_equal(prop->owner, 2);  

    free(auction); free(prop);
    free(p1); free(p2); free(p3); free(p4);
}

static void test_auction_system_gar(void **state) {
    (void) state;
    Player *p1 = init_player("Alice (Refuse)", 1);
    Player *p2 = init_player("Bob", 2);
    Player *p3 = init_player("Charlie", 3);
    Player *p4 = init_player("David", 4);
    
    p2->credit = 1000;
    p3->credit = 100; 
    
    Player *tous_les_joueurs[4] = {p1, p2, p3, p4};

    Gares *prop = calloc(1, sizeof(Gares));
    strcpy(prop->name, "Rue de la Soif");
    prop->owner = 0; 

    AuctionState *auction = init_auction_gar(prop, p1, tous_les_joueurs, 4);

    assert_non_null(auction);
    assert_int_equal(auction->players_left, 3); 
    assert_int_equal(auction->current_bid, 10);
    assert_ptr_equal(auction->gar_item, prop);
    assert_null(auction->comp_item);

    assert_int_equal(process_bid(auction, p1, 20), 0);
    assert_int_equal(process_bid(auction, p2, 50), 1);
    assert_ptr_equal(auction->highest_bidder, p2);
    assert_int_equal(process_bid(auction, p3, 20), 0);
    assert_int_equal(process_bid(auction, p3, 200), 0);

    fold_player(auction, p3); 
    fold_player(auction, p4); 
    assert_int_equal(auction->players_left, 1); 

    finalize_auction(auction);

    assert_int_equal(p2->credit, 950); 
    assert_int_equal(prop->owner, 2);  

    free(auction); free(prop);
    free(p1); free(p2); free(p3); free(p4);
}

/* NOUVEAU : Test des branches NULL et cas d'erreurs orphelins (VOE) */
static void test_voe_edge_cases(void **state) {
    (void) state;
    
    // Tests de NULL
    assert_int_equal(process_bid(NULL, NULL, 10), 0);
    fold_player(NULL, NULL);
    finalize_auction(NULL);

    // finalize_auction avec personne qui n'enchérit
    Player *p1 = init_player("P1", 1);
    Player *players[1] = {p1};
    Property *prop = calloc(1, sizeof(Property));
    
    AuctionState *auction = init_auction_prop(prop, p1, players, 1);
    auction->highest_bidder = NULL;
    finalize_auction(auction); 

    free(auction); free(prop); free(p1);
}

/* ========================================================================= */
/* TESTS DE EVENEMENTS                              */
/* ========================================================================= */

/* NOUVEAU : Couverture à 100% du système d'évènements */
static void test_evenements_all(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE evenements              \n");
    printf("==========================================================\n");
    EtatEvenementGlobal e;
    
    // Sécurité NULL
    evenements_init(NULL);
    assert_int_equal(evenements_tenter_demarrage(NULL, 1.0, 0.0, 40), 0);

    // Initialisation
    evenements_init(&e);
    assert_int_equal(e.id, EVT_AUCUN);
    
    // Accesseurs avec aucun event actif
    assert_int_equal(evenements_est_actif(&e), 0);
    assert_double_equal(evenements_mult_cout_construction(&e), 1.0, 0.01);
    assert_double_equal(evenements_mult_loyers(&e), 1.0, 0.01);
    assert_double_equal(evenements_mult_gains_cartes(&e), 1.0, 0.01);
    assert_int_equal(evenements_interdit_construire(&e), 0);
    assert_int_equal(evenements_autorise_arret_chez_soi(&e), 0);
    assert_int_equal(evenements_zone_bloquee(&e), 0);
    assert_int_equal(evenements_case_est_bloquee(&e, 5), 0);
    
    // evenements_def securité
    assert_ptr_not_equal(evenements_def(-1), NULL);
    assert_ptr_not_equal(evenements_def(999), NULL);

    // Démarrage forcé avec 100% (proba_base = 2.0 clampé à 1.0)
    int success = evenements_tenter_demarrage(&e, 2.0, 0.0, 40); 
    assert_int_equal(success, 1);
    assert_true(e.id != EVT_AUCUN);
    
    // Tentative pendant qu'il est déjà actif
    assert_int_equal(evenements_tenter_demarrage(&e, 1.0, 0.0, 40), 0);

    // Ticks pour vider le compteur
    while(e.tours_restants > 0) {
        evenements_tick(&e);
    }
    assert_int_equal(e.id, EVT_AUCUN); 
    
    // Forçage d'events spécifiques pour couvrir tous les accesseurs
    e.id = EVT_CRISE_ENERGETIQUE; e.tours_restants = 2;
    assert_double_equal(evenements_mult_cout_construction(&e), 1.2, 0.01);
    
    e.id = EVT_INFLATION; e.tours_restants = 2;
    assert_double_equal(evenements_mult_loyers(&e), 1.15, 0.01);
    assert_double_equal(evenements_mult_gains_cartes(&e), 0.9, 0.01);

    e.id = EVT_MOUVEMENT_ETUDIANT; e.tours_restants = 2;
    assert_int_equal(evenements_interdit_construire(&e), 1);
    
    e.id = EVT_CONFINEMENT; e.tours_restants = 2;
    assert_int_equal(evenements_autorise_arret_chez_soi(&e), 1);

    e.id = EVT_INONDATIONS; e.tours_restants = 2; e.zone_debut = 5; e.zone_longueur = 3;
    assert_int_equal(evenements_zone_bloquee(&e), 1);
    assert_int_equal(evenements_case_est_bloquee(&e, 6), 1);
    assert_int_equal(evenements_case_est_bloquee(&e, 10), 0);

    // Différents Bonus Risque EcoGrade
    assert_double_equal(evenements_bonus_risque_depuis_eco(GRADE_A), 0.00, 0.01);
    assert_double_equal(evenements_bonus_risque_depuis_eco(GRADE_B), 0.01, 0.01);
    assert_double_equal(evenements_bonus_risque_depuis_eco(GRADE_C), 0.03, 0.01);
    assert_double_equal(evenements_bonus_risque_depuis_eco(GRADE_D), 0.05, 0.01);
    assert_double_equal(evenements_bonus_risque_depuis_eco(GRADE_E), 0.08, 0.01);
    assert_double_equal(evenements_bonus_risque_depuis_eco(GRADE_F), 0.12, 0.01);
    assert_double_equal(evenements_bonus_risque_depuis_eco((EcoGrade)99), 0.00, 0.01); 
    
    // Inondations sur un tout petit plateau
    for(int i = 0; i < 100; i++) {
        evenements_init(&e);
        evenements_tenter_demarrage(&e, 1.0, 0.0, 2);
        if (e.id == EVT_INONDATIONS) {
            assert_int_equal(e.zone_longueur, 1); // Clamp à 1 si taille_plateau < 3
            break;
        }
    }
}

/* ========================================================================= */
/* TESTS DE CARTES                                  */
/* ========================================================================= */

/* NOUVEAU : Couverture à 100% du système de cartes (Chance/Communauté) */
static void test_cartes_all(void **state) {
    (void) state;
    printf("==========================================================\n");
    printf("              TESTS DE cartes              \n");
    printf("==========================================================\n");
    PiocheChance p_chance;
    PiocheCommunaute p_comm;
    
    // Sécurités NULL
    init_pioche_chance(NULL);
    init_pioche_communaute(NULL);
    melanger_pioche_chance(NULL);
    melanger_pioche_communaute(NULL);
    assert_null(piocher_carte_chance(NULL, 1));
    assert_null(piocher_carte_communaute(NULL, 1));
    assert_int_equal(est_carte_sortie_prison(NULL), 0);
    donner_carte_a_joueur(NULL, 1);
    utiliser_carte(NULL);
    remettre_carte_dans_paquet(NULL);
    assert_int_equal(joueur_possede_sortie_prison(NULL, 5, 1), 0);

    // Initialisation
    init_pioche_chance(&p_chance);
    init_pioche_communaute(&p_comm);

    // Vidage total de la pioche Chance
    Carte *c = NULL;
    for(int i = 0; i < NB_CARTES_CHANCE; i++) {
        c = piocher_carte_chance(&p_chance, 1);
        assert_non_null(c);
        c->etat = ETAT_CARTE_EN_MAIN;
    }
    // Piocher une fois de plus doit échouer (retour -1)
    assert_null(piocher_carte_chance(&p_chance, 1));

    // Vidage total de la pioche Communauté
    for(int i = 0; i < NB_CARTES_COMMUNAUTE; i++) {
        c = piocher_carte_communaute(&p_comm, 1);
        assert_non_null(c);
        c->etat = ETAT_CARTE_EN_MAIN;
    }
    // Piocher une fois de plus doit échouer (retour -1)
    assert_null(piocher_carte_communaute(&p_comm, 1));

    // Taxe ecologique Switch default
    assert_int_equal(calcul_taxe_ecologique(100, (EcoGrade)99), 100);

    // Outils des cartes Sortie de Prison
    Carte deck[1];
    deck[0].effet = CARTE_SORTIE_PRISON;
    deck[0].etat = ETAT_CARTE_EN_MAIN;
    deck[0].owner_number = 1;
    
    assert_int_equal(joueur_possede_sortie_prison(deck, 1, 1), 1);
    assert_int_equal(est_carte_sortie_prison(&deck[0]), 1);

    utiliser_carte(&deck[0]);
    assert_int_equal(deck[0].etat, ETAT_CARTE_UTILISEE);

    remettre_carte_dans_paquet(&deck[0]);
    assert_int_equal(deck[0].etat, ETAT_CARTE_DANS_PAQUET);
}


/* ========================================================================= */
/* MAIN                                        */
/* ========================================================================= */

int main(void) {
    const struct CMUnitTest tests[] = {
        // property
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
        cmocka_unit_test(test_property_null_and_errors),       // NOUVEAU
        cmocka_unit_test(test_property_types_and_banqueroute), // NOUVEAU

        // player
        cmocka_unit_test(test_init_player_values),
        cmocka_unit_test(test_calcul_moyenne_eco_index_player),
        cmocka_unit_test(test_player_move),
        cmocka_unit_test(test_player_credits_and_faillite),
        cmocka_unit_test(test_player_acquisition_propriete),
        cmocka_unit_test(test_player_addremove_comp),
        cmocka_unit_test(test_player_addremove_gar),
        cmocka_unit_test(test_player_edge_cases),              // NOUVEAU

        // tool
        cmocka_unit_test(test_underscore_conversion),

        // hypo
        cmocka_unit_test(test_hypotheque_et_rachat_PROP),
        cmocka_unit_test(test_hypotheque_et_rachat_GARE),
        cmocka_unit_test(test_hypotheque_et_rachat_COMP),
        cmocka_unit_test(test_hypo_edge_cases),                // NOUVEAU

        // trade
        cmocka_unit_test(test_init_empty_offer),
        cmocka_unit_test(test_is_trade_valid),
        cmocka_unit_test(test_trade_modifiers),
        cmocka_unit_test(test_execute_trade_full),

        // prison
        cmocka_unit_test(test_init_prison),
        cmocka_unit_test(test_go_to_jail_and_limit),
        cmocka_unit_test(test_get_out_of_jail),
        cmocka_unit_test(test_tours_in_jail),
        cmocka_unit_test(test_pay_to_get_out),

        // voe
        cmocka_unit_test(test_auction_system),
        cmocka_unit_test(test_auction_system_comp),
        cmocka_unit_test(test_auction_system_gar),
        cmocka_unit_test(test_voe_edge_cases),                 // NOUVEAU

        // evenements & cartes (NOUVEAU)
        cmocka_unit_test(test_evenements_all),
        cmocka_unit_test(test_cartes_all)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}