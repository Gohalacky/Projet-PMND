# INSA'poly

Bienvenue sur le dépôt d'INSA'poly. Il s'agit d'un jeu de plateau stratégique et éco-responsable que nous développons en langage C (C11) pour le module Projet et Méthodologie pour le Numérique Durable (PMND).

Le concept est le suivant : les joueurs incarnent des étudiants de l'INSA Rennes devant aménager un campus. L'objectif est de trouver le bon équilibre entre la réussite académique/financière (gagner des crédits ECTS) et l'impact environnemental de leurs constructions (Indice Écologique). 

---

## Architecture du projet (MVC)

Pour garder un code propre et testable, nous avons structuré le projet selon le modèle Modèle-Vue-Contrôleur :

* **Le Modèle (dossier `Modele/`) :** C'est le cœur des règles du jeu. Il gère l'état des joueurs (ECTS, position, note écologique), l'état du plateau (propriétés, gares, compagnies), l'économie des loyers et les événements dynamiques.
* **La Vue (dossier `Interface/`) :** C'est notre interface graphique développée avec SDL2. Elle permet d'afficher le plateau, les stats des joueurs et de paramétrer la partie via un menu interactif.
* **Le Contrôleur (dossier `Controlleur/`) :** Il fait le pont entre les deux. C'est lui qui charge les fichiers de configuration au lancement et qui fait tourner la boucle principale du jeu.

---

## Fonctionnalités actuelles

À ce stade du développement, le jeu gère :
* La création de partie jusqu'à 4 joueurs avec des paramètres personnalisables (argent initial, nombre de tours, etc.).
* Un système de propriétés où l'on peut construire des aménagements (maisons, logements sociaux, parcs) qui influencent directement l'indice écologique global et personnel.
* La gestion des événements aléatoires (cartes Chance et Caisse Communauté).
* Une couverture de tests sur la logique grâce à CMocka.

---

## Prérequis pour la compilation

Si vous souhaitez recompiler le projet chez vous (environnement Linux recommandé), vous aurez besoin des paquets suivants :

* **Utilitaires de base :** `gcc` et `make`
* **Pour l'interface graphique :** Les bibliothèques `libsdl2-dev`, `libsdl2-image-dev` et `libsdl2-ttf-dev`
* **Pour les tests unitaires :** `libcmocka-dev`
* **Pour générer la documentation :** `doxygen` et `graphviz` (pour les graphes d'appels)

---

## Compilation et exécution

Nous avons configuré un Makefile à la racine pour vous simplifier la vie. 

### 1. Pour compiler
* Tout le projet : `make`
* Juste le moteur de jeu (sans interface) : `make controlleur`
* Juste l'interface graphique : `make interface`
* Les tests unitaires : `make test` et `make test_cartes`
* Générer le site web de la documentation et l'ouvre (sous linux ouverture de index.html dans docs/): `make doc`
* Nettoyer les fichiers de compilation : `make clean`

### 2. Pour lancer le jeu
**Attention :** Pour éviter les problèmes de chemins relatifs avec les fichiers de sauvegarde ou les images, nous vous conseillons d'utiliser nos commandes préconfigurées plutôt que de lancer les exécutables à la main.

* Lancer le jeu avec l'interface graphique : 
  `make run_interface`
* Lancer le jeu en mode terminal (debug) : 
  `make run_controlleur`
* Lancer les tests unitaires du modèle : 
  `make run_unit_tests`
* Lancer les tests sur les cartes : 
  `make run_test`

---
Projet réalisé dans le cadre du cursus INSA Rennes 3EII par
Arthur GALLIC
Arnaud PASTRE
Thomas LELONG
Titouan CHIFFOLEAU