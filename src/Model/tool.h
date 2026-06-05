/**
 * @file tool.h
 * @brief Fonctions utilitaires génériques pour le projet INSA'poly.
 *
 * Ce module regroupe les outils de manipulation de données ou de chaînes 
 * de caractères qui sont transverses à l'application. Ces fonctions 
 * sont indépendantes des règles du jeu.
 */

#ifndef TOOL_H
#define TOOL_H

/**
 * @brief Remplace tous les underscores ('_') d'une chaîne par des espaces (' ').
 * * @note Cette fonction est utile lors du chargement des données 
 * depuis des fichiers texte (comme les propriétés, gares ou compagnies), où les 
 * noms composés utilisent des underscores pour éviter les problèmes de parsing 
 * avec `fscanf`.
 * * @param str Pointeur vers la chaîne de caractères à traiter. 
 */

void replace_underscore_with_space(char *str);  


#endif