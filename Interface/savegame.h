#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "Struct.h"
#include "bridge.h"


#define SAVE_DIR      "../saves"
#define SAVE_MAX_PATH 256
#define MAX_SAVES     10


typedef struct {
    char filename[SAVE_MAX_PATH]; 
    char label[64];               
    int  nbJoueurs;               
    int  tourActuel;              
    int  tourMax;                 
} SaveInfo;


int savegame_save(const EtatPartie* partie, const ContexteJeu* ctx);

int savegame_load_file(const char* filepath, EtatPartie* partie, ContexteJeu* ctx);

int savegame_list(SaveInfo out[MAX_SAVES]);

int savegame_delete(const char* filepath);

int savegame_exists(void);

#endif 