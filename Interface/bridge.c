#include "bridge.h"
#include "Struct.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bridge_init(ContexteJeu* ctx, const EtatPartie* partie) {

    memset(ctx, 0, sizeof(ContexteJeu));

    int nbp = chargement_propriete_loyer(ctx->tableau_prop, "loyer_insapoly.txt");
    int nbg = chargement_GARE_loyer(ctx->tableau_gar,       "gares.txt");
    int nbc = chargement_COMPAGNIE_loyer(ctx->tableau_comp, "compagnie.txt");
 
    if (nbp <= 0 || nbg <= 0 || nbc <= 0) {
        printf("Erreur bridge_init : chargement des fichiers échoué "
               "(prop=%d, gares=%d, comp=%d)\n", nbp, nbg, nbc);
        return -1;
    }

    init_plateau(&ctx->plateau,
                 ctx->tableau_prop,
                 ctx->tableau_gar,
                 ctx->tableau_comp);

    for (int i = 0; i < partie->nbJoueurs; i++) {
        ctx->joueurs[i] = init_player(partie->joueurs[i].nom, i + 1);
        if (!ctx->joueurs[i]) {
            printf("Erreur bridge_init : init_player(%d) échoué\n", i);
            return -1;
        }
        ctx->joueurs[i]->credit = partie->joueurs[i].credits;
    }
 
    ctx->admin_mode = 0;  
    return 0;
}
 
void bridge_play_turn(ContexteJeu* ctx, EtatPartie* partie) {
    int ja = partie->joueursActif;
    Player* p = ctx->joueurs[ja];
 
    if (!p) return;
 
    engine_play_turn(p,ctx->joueurs,ctx->tableau_prop,ctx->tableau_gar,ctx->tableau_comp,&ctx->plateau,ctx->admin_mode);
    bridge_sync(ctx, partie);
}
 
 
void bridge_sync(const ContexteJeu* ctx, EtatPartie* partie) {
    for (int i = 0; i < partie->nbJoueurs; i++) {
        Player* p = ctx->joueurs[i];
        if (!p) continue;
        partie->joueurs[i].credits = p->credit;
        partie->joueurs[i].noteEco = (float)p->eco_index;
    }
}
 
int bridge_next_turn(EtatPartie* partie) {

    partie->joueursActif = (partie->joueursActif + 1) % partie->nbJoueurs;

    if (partie->joueursActif == 0){
        partie->tourActuel++;
    }
 
    if (partie->tourMax > 0 && partie->tourActuel > partie->tourMax){
        return 1;
    }
    return 0;
}

 
void bridge_free(ContexteJeu* ctx) {
    for (int i = 0; i < 4; i++) {
        if (ctx->joueurs[i]) {
            free(ctx->joueurs[i]);
            ctx->joueurs[i] = NULL;
        }
    }
    for (int i = 0; i < NB_PROPRIETE; i++)
        if (ctx->tableau_prop[i]) { free(ctx->tableau_prop[i]); ctx->tableau_prop[i] = NULL; }
    for (int i = 0; i < NB_GARE; i++)
        if (ctx->tableau_gar[i])  { free(ctx->tableau_gar[i]);  ctx->tableau_gar[i]  = NULL; }
    for (int i = 0; i < NB_COMPAGNIE; i++)
        if (ctx->tableau_comp[i]) { free(ctx->tableau_comp[i]); ctx->tableau_comp[i] = NULL; }
}