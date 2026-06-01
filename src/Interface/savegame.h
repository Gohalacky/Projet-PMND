/**
 * @file savegame.h
 * @brief Sauvegarde et chargement de parties (format JSON via cJSON).
 *
 * Les fichiers sont stockés dans le répertoire @c saves/ (créé si absent).
 * Chaque fichier correspond à une sauvegarde nommée par horodatage.
 */

#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "Struct.h"
#include "bridge.h"

#define SAVE_DIR      "saves"      /**< Répertoire de stockage des sauvegardes. */
#define SAVE_MAX_PATH 270          /**< Longueur maximale d'un chemin de fichier. */
#define MAX_SAVES     10           /**< Nombre maximum de sauvegardes listées. */

/**
 * @brief Métadonnées d'une sauvegarde, utilisées par le menu de chargement.
 */
typedef struct {
    char filename[SAVE_MAX_PATH]; /**< Chemin complet du fichier de sauvegarde. */
    char label[64];               /**< Libellé affiché dans le menu (date + info). */
    int  nbJoueurs;               /**< Nombre de joueurs sauvegardés. */
    int  tourActuel;              /**< Numéro du tour au moment de la sauvegarde. */
    int  tourMax;                 /**< Nombre de tours maximum de la partie. */
} SaveInfo;

/**
 * @brief Sauvegarde l'état courant de la partie dans un nouveau fichier JSON.
 * @param partie État de la partie côté Interface.
 * @param ctx    Contexte de jeu complet (joueurs, plateau, prison…).
 * @return 0 en cas de succès, -1 en cas d'erreur d'écriture.
 */
int savegame_save(const EtatPartie* partie, const ContexteJeu* ctx);

/**
 * @brief Charge une sauvegarde depuis un fichier JSON.
 *
 * Réinitialise @p ctx avant de le remplir.  Après l'appel, les pions doivent
 * être synchronisés manuellement avec snapPawnToCase().
 *
 * @param filepath Chemin du fichier à charger.
 * @param partie   État de la partie à remplir.
 * @param ctx      Contexte de jeu à reconstruire.
 * @return 0 en cas de succès, -1 en cas d'erreur de lecture ou de format.
 */
int savegame_load_file(const char* filepath, EtatPartie* partie, ContexteJeu* ctx);

/**
 * @brief Liste les fichiers de sauvegarde disponibles dans @c SAVE_DIR.
 * @param out Tableau de SaveInfo à remplir (taille minimale MAX_SAVES).
 * @return Nombre de sauvegardes trouvées (0 si aucune, -1 si erreur).
 */
int savegame_list(SaveInfo out[MAX_SAVES]);

/**
 * @brief Supprime un fichier de sauvegarde.
 * @param filepath Chemin du fichier à supprimer.
 * @return 0 en cas de succès, -1 si la suppression échoue.
 */
int savegame_delete(const char* filepath);

/**
 * @brief Vérifie si au moins un fichier de sauvegarde existe.
 * @return 1 si une sauvegarde existe, 0 sinon.
 */
int savegame_exists(void);

#endif
