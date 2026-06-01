# INSA'poly

Bienvenue sur le dépôt d'INSA'poly. Il s'agit d'un jeu de plateau stratégique et éco-responsable développé en langage C pour le module Projet et Méthodologie pour le Numérique Durable (PMND).

Les joueurs incarnent des étudiants de l'INSA Rennes devant aménager un campus. L'objectif est de trouver le bon équilibre entre la réussite académique/financière (crédits ECTS) et l'impact environnemental de leurs constructions (Indice Écologique).

---

## Architecture du projet (MVC)

Le projet suit le modèle Modèle-Vue-Contrôleur :

- **Modèle (`src/Model/`)** — cœur des règles : état des joueurs, plateau, propriétés, loyers, cartes, événements, prison, hypothèques, échanges.
- **Vue (`src/Interface/`)** — interface graphique SDL2 : plateau, cartes joueurs, animations, menus, fenêtres contextuelles.
- **Contrôleur (`src/Controller/`)** — boucle de jeu, chargement des fichiers de configuration, pont entre Modèle et Vue.

---

## Fonctionnalités

### Partie et paramètres
- Création de partie de 2 à 4 joueurs
- Paramètres personnalisables : argent initial, nombre de tours maximum (ou tours infinis jusqu'à la faillite), fréquence des événements
- Sauvegarde et chargement de partie (format JSON via cJSON)
- Menu pause avec sauvegarde & quitter

### Plateau et déplacements
- Plateau adapté à 2, 3 ou 4 joueurs
- Pions animés avec déplacement case par case
- Animation de dé au lancer
- Affichage de la case atterrie et indication de double

### Propriétés
- Achat de propriétés, gares et compagnies
- Construction d'aménagements (maisons, logements sociaux, parcs) influençant l'indice écologique
- Système de loyers proportionnel aux constructions
- Hypothèque et rachat de propriétés
- Fenêtre de gestion des propriétés possédées

### Échanges
- Système d'échange entre joueurs (propriétés et crédits)

### Prison
- Tentative de double pour sortir de prison
- Utilisation d'une carte de libération
- Indication visuelle en cas d'échec du double ("Double raté !")

### Cartes et événements
- Cartes Chance et Caisse de Communauté avec effets variés (gains, pertes, déplacements, taxes)
- Événements aléatoires actifs affectant des cases spécifiques, affichés dans un bandeau dédié

### Interface et retours visuels
- Interface entièrement redimensionnable (mise à l'échelle de tous les éléments)
- Indicateur animé de gain/perte de crédits (texte vert/rouge flottant sur la carte joueur)
- Affichage du règlement du jeu
- Écran de fin de partie avec classement

---

## Prérequis

Environnement Linux recommandé. Paquets nécessaires :

| Usage | Paquets |
|---|---|
| Compilation | `gcc`, `make` |
| Interface graphique | `libsdl2-dev`, `libsdl2-image-dev`, `libsdl2-ttf-dev` |
| Sérialisation JSON | `libcjson-dev` |
| Tests unitaires | `libcmocka-dev` |
| Documentation | `doxygen`, `graphviz` |

---

## Compilation et exécution

### Compilation

```bash
make              # Compile tout le projet → bin/insapoly
make controlleur  # Compile uniquement le moteur de jeu (sans interface)
make test         # Compile les tests unitaires du modèle
make test_cartes  # Compile les tests des cartes/événements
```

### Exécution

> **Important :** Toujours lancer depuis la racine du projet pour que les chemins relatifs (assets, sauvegardes) soient correctement résolus.

```bash
make run              # Lance le jeu avec interface graphique
make run_controlleur  # Lance le jeu en mode terminal (debug)
make run_unit_tests   # Exécute les tests unitaires du modèle
make run_test         # Exécute les tests des cartes
```

### Couverture et documentation

```bash
make coverage   # Génère le rapport de couverture de code (gcovr)
make doc        # Génère la documentation Doxygen et l'ouvre dans le navigateur
make clean      # Supprime les fichiers objets et binaires
make mrproper   # clean + suppression de la documentation générée
```

---

## Structure des sources

```
src/
├── Model/          # Logique de jeu (indépendante de l'affichage)
│   ├── player.c/h, property.c/h, cartes.c/h, evenements.c/h
│   ├── prison.c/h, trade.c/h, hypo.c/h, voe.c/h, tool.c/h
├── Controller/     # Moteur de jeu et pont Modèle↔Vue
│   ├── game_engine.c/h, engine_logic.c/h, plateau.c/h
│   └── game_engine_console.c/h
└── Interface/      # Couche graphique SDL2
    ├── main_interface.c   # Point d'entrée, boucle principale
    ├── view.c/h           # Rendu principal (dispatch par état)
    ├── bridge.c/h         # Pont Interface↔Contrôleur
    ├── events.c/h         # Gestion des événements SDL
    ├── Struct.h           # Structures partagées (EtatPartie, Joueur…)
    ├── assets.c/h, anim.c/h, pawn.c/h, building.c/h
    ├── propwindow, tradewindow, prisonwindow, pausemenu…
    └── ui/                # Composants réutilisables
        ├── button, text, textbox, counter, checkbox, playercard
```

---

## Tests

Les tests couvrent la logique du Modèle (indépendamment de l'interface) :

```bash
make run_unit_tests   # Tests joueurs, propriétés, prison, échanges
make run_test         # Tests cartes Chance et Caisse de Communauté
make coverage         # Rapport de couverture combiné
```

---

Projet réalisé dans le cadre du cursus INSA Rennes 3EII par
Arthur GALLIC, Arnaud PASTRE, Thomas LELONG, Titouan CHIFFOLEAU
