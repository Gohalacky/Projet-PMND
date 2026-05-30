#include "savegame.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include <cjson/cJSON.h>


static void build_save_path(char out[SAVE_MAX_PATH]) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(out, SAVE_MAX_PATH,
             SAVE_DIR "/save_%04d%02d%02d_%02d%02d%02d.json",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour,        t->tm_min,      t->tm_sec);
}

static void date_from_filename(const char* fname, char out[32]) {
    const char* base = strrchr(fname, '/');
    base = base ? base + 1 : fname;
    int y, mo, d, h, mi, s;
    if (sscanf(base, "save_%4d%2d%2d_%2d%2d%2d", &y, &mo, &d, &h, &mi, &s) == 6) {
        snprintf(out, 32, "%02d/%02d/%04d %02d:%02d", d, mo, y, h, mi);
    } else {
        strncpy(out, base, 31);
    }
}


int savegame_save(const EtatPartie* partie, const ContexteJeu* ctx) {
    if (!partie || !ctx) return -1;

    cJSON* root = cJSON_CreateObject();
    if (!root) return -1;

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char datebuf[32];
    snprintf(datebuf, sizeof(datebuf), "%04d-%02d-%02d %02d:%02d:%02d",t->tm_year+1900, t->tm_mon+1, t->tm_mday,t->tm_hour, t->tm_min, t->tm_sec);

    cJSON_AddNumberToObject(root, "version", 1);
    cJSON_AddStringToObject(root, "date",    datebuf);

    cJSON* ep = cJSON_CreateObject();
    cJSON_AddNumberToObject(ep, "nbJoueurs",    partie->nbJoueurs);
    cJSON_AddNumberToObject(ep, "joueursActif", partie->joueursActif);
    cJSON_AddNumberToObject(ep, "tourActuel",   partie->tourActuel);
    cJSON_AddNumberToObject(ep, "tourMax",      partie->tourMax);
    cJSON_AddNumberToObject(ep, "dejaLance",    partie->dejaLance);

    cJSON* joueursEP = cJSON_CreateArray();
    for (int i = 0; i < partie->nbJoueurs; i++) {
        cJSON* j = cJSON_CreateObject();
        cJSON_AddStringToObject(j, "nom",     partie->joueurs[i].nom);
        cJSON_AddNumberToObject(j, "credits", partie->joueurs[i].credits);
        cJSON_AddNumberToObject(j, "noteEco", (double)partie->joueurs[i].noteEco);
        cJSON_AddItemToArray(joueursEP, j);
    }
    cJSON_AddItemToObject(ep, "joueurs", joueursEP);
    cJSON_AddItemToObject(root, "etatPartie", ep);

    cJSON* players = cJSON_CreateArray();
    for (int i = 0; i < partie->nbJoueurs; i++) {
        if (!ctx->joueurs[i]) continue;
        cJSON* p = cJSON_CreateObject();
        cJSON_AddStringToObject(p, "name",      ctx->joueurs[i]->name);
        cJSON_AddNumberToObject(p, "number",    ctx->joueurs[i]->number);
        cJSON_AddNumberToObject(p, "credit",    ctx->joueurs[i]->credit);
        cJSON_AddNumberToObject(p, "position",  ctx->joueurs[i]->position);
        cJSON_AddNumberToObject(p, "eco_index", (int)ctx->joueurs[i]->eco_index);
        cJSON_AddItemToArray(players, p);
    }
    cJSON_AddItemToObject(root, "joueurs", players);

    cJSON* props = cJSON_CreateArray();
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (!ctx->tableau_prop[i]) continue;
        cJSON* p = cJSON_CreateObject();
        cJSON_AddStringToObject(p, "name",   ctx->tableau_prop[i]->name);
        cJSON_AddNumberToObject(p, "owner",  ctx->tableau_prop[i]->owner);
        cJSON_AddNumberToObject(p, "level",  ctx->tableau_prop[i]->level);
        cJSON_AddNumberToObject(p, "type",   (int)ctx->tableau_prop[i]->type);
        cJSON_AddNumberToObject(p, "indice", ctx->tableau_prop[i]->indice);
        cJSON_AddNumberToObject(p, "hypo",   ctx->tableau_prop[i]->hypo);
        cJSON_AddItemToArray(props, p);
    }
    cJSON_AddItemToObject(root, "proprietes", props);

    cJSON* gares = cJSON_CreateArray();
    for (int i = 0; i < NB_GARE; i++) {
        if (!ctx->tableau_gar[i]) continue;
        cJSON* g = cJSON_CreateObject();
        cJSON_AddStringToObject(g, "name",  ctx->tableau_gar[i]->name);
        cJSON_AddNumberToObject(g, "owner", ctx->tableau_gar[i]->owner);
        cJSON_AddNumberToObject(g, "hypo",  ctx->tableau_gar[i]->hypo);
        cJSON_AddItemToArray(gares, g);
    }
    cJSON_AddItemToObject(root, "gares", gares);

    cJSON* comps = cJSON_CreateArray();
    for (int i = 0; i < NB_COMPAGNIE; i++) {
        if (!ctx->tableau_comp[i]) continue;
        cJSON* c = cJSON_CreateObject();
        cJSON_AddStringToObject(c, "name",  ctx->tableau_comp[i]->name);
        cJSON_AddNumberToObject(c, "owner", ctx->tableau_comp[i]->owner);
        cJSON_AddNumberToObject(c, "hypo",  ctx->tableau_comp[i]->hypo);
        cJSON_AddItemToArray(comps, c);
    }
    cJSON_AddItemToObject(root, "compagnies", comps);

    cJSON* prison_arr = cJSON_CreateArray();
    if (ctx->jail) {
        for (int i = 0; i < partie->nbJoueurs; i++) {
            if (!ctx->joueurs[i]) continue;
            int tours = 0;
            int en_prison = 0;
            for (int k = 0; k < ctx->jail->nb_j; k++) {
                if (ctx->jail->liste[k] == ctx->joueurs[i]) {
                    en_prison = 1;
                    tours = ctx->jail->comptage[k];
                    break;
                }
            }
            cJSON* pj = cJSON_CreateObject();
            cJSON_AddNumberToObject(pj, "joueur_idx", i);
            cJSON_AddNumberToObject(pj, "en_prison",  en_prison);
            cJSON_AddNumberToObject(pj, "tours",      tours);
            cJSON_AddItemToArray(prison_arr, pj);
        }
    }
    cJSON_AddItemToObject(root, "prison", prison_arr);

    char filepath[SAVE_MAX_PATH];
    build_save_path(filepath);

    char* json_str = cJSON_Print(root);
    cJSON_Delete(root);
    if (!json_str) return -1;

    mkdir(SAVE_DIR, 0755); 
    FILE* f = fopen(filepath, "w");
    if (!f) {
        printf("[savegame] Impossible d'ouvrir %s : %s\n", filepath, strerror(errno));
        free(json_str);
        return -1;
    }
    fputs(json_str, f);
    fclose(f);
    free(json_str);

    printf("[savegame] Sauvegarde écrite : %s\n", filepath);
    return 0;
}


int savegame_load_file(const char* filepath, EtatPartie* partie, ContexteJeu* ctx) {
    if (!filepath || !partie || !ctx) return -1;

    FILE* f = fopen(filepath, "r");
    if (!f) {
        printf("[savegame] Fichier %s introuvable.\n", filepath);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char* buf = malloc(len + 1);
    if (!buf) { fclose(f); return -1; }
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);

    cJSON* root = cJSON_Parse(buf);
    free(buf);
    if (!root) {
        printf("[savegame] JSON invalide dans %s\n", filepath);
        return -1;
    }

    cJSON* ep = cJSON_GetObjectItem(root, "etatPartie");
    if (!ep) { cJSON_Delete(root); return -1; }

#define GETINT(obj, key, def) \
    (cJSON_GetObjectItem(obj, key) ? (int)cJSON_GetObjectItem(obj, key)->valuedouble : (def))

    partie->nbJoueurs    = GETINT(ep, "nbJoueurs",    2);
    partie->joueursActif = GETINT(ep, "joueursActif", 0);
    partie->tourActuel   = GETINT(ep, "tourActuel",   1);
    partie->tourMax      = GETINT(ep, "tourMax",      0);
    partie->dejaLance    = GETINT(ep, "dejaLance",    0);
    partie->temps_lancer = 0;

    cJSON* joueursEP = cJSON_GetObjectItem(ep, "joueurs");
    if (joueursEP) {
        int n = cJSON_GetArraySize(joueursEP);
        for (int i = 0; i < n && i < 4; i++) {
            cJSON* j   = cJSON_GetArrayItem(joueursEP, i);
            cJSON* nom = cJSON_GetObjectItem(j, "nom");
            if (nom) strncpy(partie->joueurs[i].nom, nom->valuestring, 31);
            partie->joueurs[i].credits = GETINT(j, "credits", 1500);
            cJSON* eco = cJSON_GetObjectItem(j, "noteEco");
            if (eco) partie->joueurs[i].noteEco = (float)eco->valuedouble;
        }
    }

    if (bridge_init(ctx, partie) < 0) {
        printf("[savegame] Erreur bridge_init lors du chargement.\n");
        cJSON_Delete(root);
        return -1;
    }

    cJSON* players = cJSON_GetObjectItem(root, "joueurs");
    if (players) {
        int n = cJSON_GetArraySize(players);
        for (int i = 0; i < n && i < 4; i++) {
            cJSON* p = cJSON_GetArrayItem(players, i);
            if (!ctx->joueurs[i]) continue;
            ctx->joueurs[i]->position  = GETINT(p, "position",  0);
            ctx->joueurs[i]->credit    = GETINT(p, "credit",    1500);
            ctx->joueurs[i]->eco_index = GETINT(p, "eco_index", 1);
        }
    }

    cJSON* props = cJSON_GetObjectItem(root, "proprietes");
    if (props) {
        int n = cJSON_GetArraySize(props);
        for (int i = 0; i < n; i++) {
            cJSON* p  = cJSON_GetArrayItem(props, i);
            cJSON* nm = cJSON_GetObjectItem(p, "name");
            if (!nm) continue;
            for (int j = 0; j < NB_PROPRIETE; j++) {
                if (!ctx->tableau_prop[j]) continue;
                if (strcmp(ctx->tableau_prop[j]->name, nm->valuestring) == 0) {
                    ctx->tableau_prop[j]->owner  = GETINT(p, "owner",  0);
                    ctx->tableau_prop[j]->level  = GETINT(p, "level",  0);
                    ctx->tableau_prop[j]->type   = GETINT(p, "type",   0);
                    ctx->tableau_prop[j]->indice = GETINT(p, "indice", 0);
                    ctx->tableau_prop[j]->hypo   = GETINT(p, "hypo",   0);
                    break;
                }
            }
        }
    }

    cJSON* gares = cJSON_GetObjectItem(root, "gares");
    if (gares) {
        int n = cJSON_GetArraySize(gares);
        for (int i = 0; i < n; i++) {
            cJSON* g  = cJSON_GetArrayItem(gares, i);
            cJSON* nm = cJSON_GetObjectItem(g, "name");
            if (!nm) continue;
            for (int j = 0; j < NB_GARE; j++) {
                if (!ctx->tableau_gar[j]) continue;
                if (strcmp(ctx->tableau_gar[j]->name, nm->valuestring) == 0) {
                    ctx->tableau_gar[j]->owner = GETINT(g, "owner", 0);
                    ctx->tableau_gar[j]->hypo  = GETINT(g, "hypo",  0);
                    break;
                }
            }
        }
    }

    cJSON* comps = cJSON_GetObjectItem(root, "compagnies");
    if (comps) {
        int n = cJSON_GetArraySize(comps);
        for (int i = 0; i < n; i++) {
            cJSON* c  = cJSON_GetArrayItem(comps, i);
            cJSON* nm = cJSON_GetObjectItem(c, "name");
            if (!nm) continue;
            for (int j = 0; j < NB_COMPAGNIE; j++) {
                if (!ctx->tableau_comp[j]) continue;
                if (strcmp(ctx->tableau_comp[j]->name, nm->valuestring) == 0) {
                    ctx->tableau_comp[j]->owner = GETINT(c, "owner", 0);
                    ctx->tableau_comp[j]->hypo  = GETINT(c, "hypo",  0);
                    break;
                }
            }
        }
    }

#undef GETINT

    cJSON* prison_arr = cJSON_GetObjectItem(root, "prison");
    if (prison_arr && ctx->jail) {
        int n = cJSON_GetArraySize(prison_arr);
        for (int i = 0; i < n; i++) {
            cJSON* pj = cJSON_GetArrayItem(prison_arr, i);
            if (!pj) continue;
            cJSON* idx_item = cJSON_GetObjectItem(pj, "joueur_idx");
            cJSON* ep_item  = cJSON_GetObjectItem(pj, "en_prison");
            cJSON* tr_item  = cJSON_GetObjectItem(pj, "tours");
            if (!idx_item || !ep_item) continue;
            int idx       = (int)idx_item->valuedouble;
            int en_prison = (int)ep_item->valuedouble;
            int tours     = tr_item ? (int)tr_item->valuedouble : 0;
            if (en_prison && idx >= 0 && idx < partie->nbJoueurs && ctx->joueurs[idx]) {
                go_to_jail(ctx->joueurs[idx], ctx->jail);
                for (int k = 0; k < ctx->jail->nb_j; k++) {
                    if (ctx->jail->liste[k] == ctx->joueurs[idx]) {
                        ctx->jail->comptage[k] = tours;
                        break;
                    }
                }
            }
        }
    }

    cJSON_Delete(root);
    printf("[savegame] Chargé : %s\n", filepath);
    return 0;
}


int savegame_list(SaveInfo out[MAX_SAVES]) {
    int count = 0;
    DIR* dir = opendir(SAVE_DIR);
    if (!dir) return 0;

    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL && count < MAX_SAVES) {
        if (strncmp(ent->d_name, "save_", 5) != 0) continue;
        const char* ext = strrchr(ent->d_name, '.');
        if (!ext || strcmp(ext, ".json") != 0) continue;

        SaveInfo* si = &out[count];
        snprintf(si->filename, SAVE_MAX_PATH, SAVE_DIR "/%s", ent->d_name);

        si->nbJoueurs  = 0;
        si->tourActuel = 0;
        si->tourMax    = 0;

        FILE* f = fopen(si->filename, "r");
        if (f) {
            fseek(f, 0, SEEK_END);
            long len = ftell(f);
            rewind(f);
            char* buf = malloc(len + 1);
            if (buf) {
                fread(buf, 1, len, f);
                buf[len] = '\0';
                cJSON* root = cJSON_Parse(buf);
                free(buf);
                if (root) {
                    cJSON* ep = cJSON_GetObjectItem(root, "etatPartie");
                    if (ep) {
                        cJSON* nb = cJSON_GetObjectItem(ep, "nbJoueurs");
                        cJSON* ta = cJSON_GetObjectItem(ep, "tourActuel");
                        cJSON* tm = cJSON_GetObjectItem(ep, "tourMax");
                        if (nb) si->nbJoueurs  = (int)nb->valuedouble;
                        if (ta) si->tourActuel = (int)ta->valuedouble;
                        if (tm) si->tourMax    = (int)tm->valuedouble;
                    }
                    cJSON_Delete(root);
                }
            }
            fclose(f);
        }

        char datestr[32];
        date_from_filename(ent->d_name, datestr);
        if (si->tourMax > 0) {
            snprintf(si->label, sizeof(si->label),
                     "%s  |  %dJ  Tour %d/%d",
                     datestr, si->nbJoueurs, si->tourActuel, si->tourMax);
        } else {
            snprintf(si->label, sizeof(si->label),
                     "%s  |  %dJ  Tour %d",
                     datestr, si->nbJoueurs, si->tourActuel);
        }

        count++;
    }
    closedir(dir);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(out[i].filename, out[j].filename) < 0) {
                SaveInfo tmp = out[i];
                out[i] = out[j];
                out[j] = tmp;
            }
        }
    }

    return count;
}

int savegame_delete(const char* filepath) {
    if (!filepath) return -1;
    if (remove(filepath) != 0) {
        printf("[savegame] Impossible de supprimer %s : %s\n",
               filepath, strerror(errno));
        return -1;
    }
    printf("[savegame] Supprimé : %s\n", filepath);
    return 0;
}

int savegame_exists(void) {
    DIR* dir = opendir(SAVE_DIR);
    if (!dir) return 0;
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strncmp(ent->d_name, "save_", 5) == 0) {
            const char* ext = strrchr(ent->d_name, '.');
            if (ext && strcmp(ext, ".json") == 0) {
                closedir(dir);
                return 1;
            }
        }
    }
    closedir(dir);
    return 0;
}