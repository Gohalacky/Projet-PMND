#include "view.h"
#include "Struct.h"
#include "assets.h"
#include "bridge.h"
#include "building.h"
#include "pawn.h"
#include "anim.h"
#include "propwindow.h"
#include "prisonwindow.h"
#include "pausemenu.h"
#include "loadmenu.h"
#include "rules.h"
#include "ui/playercard.h"
#include "ui/text.h"
#include "ui/button.h"
#include "ui/checkbox.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

static void renderPlateau(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex, TextsPack* texts, TTF_Font* fontButton, TTF_Font* fontNormal, EtatPartie* partie, ContexteJeu* ctx, ButtonsPack* btn, Pawn pawns[MAX_PLAYERS]){
    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    SDL_Texture* board = tex->gameBoard2Player;
    if      (state == PLATEAU3J) board = tex->gameBoard3Player;
    else if (state == PLATEAU4J) board = tex->gameBoard4Player;
    SDL_RenderCopy(renderer, board, NULL, NULL);
    renderBuildings(renderer, ctx, tex, winW, winH);
    renderJoueurActif(renderer, fontButton, partie, ctx);
    renderButtonDisabled(renderer, &btn->throwDice);
    renderButtonDisabled(renderer, &btn->endTurn);
    renderPawns(renderer, tex, pawns, partie->nbJoueurs, winW, winH);
    
 
    printTextResponsive(renderer, fontButton, &texts->btnThrowDice);
    printTextResponsive(renderer, fontButton, &texts->btnProperties);
    printTextResponsive(renderer, fontButton, &texts->btnEndTurn);
    printTextResponsive(renderer, fontButton, &texts->indNbTurn);

    /* Numéro du tour affiché sous le label "Tour" : X/Y ou X/infini */
        char bufferTour[32];
        int lineH = TTF_FontHeight(fontNormal);
        if (partie->tourMax > 0) {
            snprintf(bufferTour, sizeof(bufferTour), "%d/%d", partie->tourActuel, partie->tourMax);
        } else {
            snprintf(bufferTour, sizeof(bufferTour), "%d/infini", partie->tourActuel);
        }
        SDL_Color couleur = {235, 221, 192, 255};
        printText(renderer, fontNormal, bufferTour, couleur, texts->indNbTurn.x, texts->indNbTurn.y + lineH +5);
 
    const CardRef* cards;
    switch (partie->nbJoueurs) {
        case 3:  cards = CARDS_3J; break;
        case 4:  cards = CARDS_4J; break;
        default: cards = CARDS_2J; break;
    }
    for (int i = 0; i < partie->nbJoueurs; i++) {
        SDL_Rect rect = {
            (int)(cards[i].x * winW / REF_W),
            (int)(cards[i].y * winH / REF_H),
            (int)(cards[i].w * winW / REF_W),
            (int)(cards[i].h * winH / REF_H),
        };
        renderPlayerCard(renderer, fontButton, &partie->joueurs[i], i + 1, rect);
    }

    if (state == LANCER) {

        int ja = partie->joueursActif;
        SDL_Rect rect = {
            (int)(cards[ja].x * winW / REF_W),
            (int)(cards[ja].y * winH / REF_H),
            (int)(cards[ja].w * winW / REF_W),
            (int)(cards[ja].h * winH / REF_H),
        };
        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer,0,0,0,180);
        SDL_Rect overlay ={0,0,winW,winH};
        SDL_RenderFillRect(renderer,&overlay);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        renderPlayerCard(renderer, fontButton, &partie->joueurs[ja],ja+1,rect);

        SDL_Rect fenetreLancer = {
            (int)(340.0f * winW / REF_W),
            (int)(100.0f * winH / REF_H),
            (int)(600.0f * winW / REF_W),
            (int)(400.0f * winH / REF_H),
        };
        SDL_RenderCopy(renderer, tex->throws, NULL, &fenetreLancer);

        SDL_Color blanc = {235,221,192,255};
        SDL_Color rouge = {255,72,41,255};
        if(ctx->dernier_de>0){
            char bufferDe[8];
            snprintf(bufferDe, sizeof(bufferDe), "%d",ctx->dernier_de);
            int tw, th;
            TTF_SizeUTF8(fontNormal, bufferDe, &tw, &th);
            int textY = fenetreLancer.y + fenetreLancer.h/3 - 100;
            int pad = 12;
            int size = (tw > th ? tw : th) + pad*2;
            SDL_Rect fond = { winW/2 - size/2, textY + th/2 - size/2, size, size };
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 10, 8, 5, 210);
            SDL_RenderFillRect(renderer, &fond);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            printTextCentered(renderer, fontNormal, bufferDe, blanc, textY, winW);
        }

        if (bridge_is_double(ctx))
            printTextCentered(renderer, fontNormal, "DOUBLE !", rouge,fenetreLancer.y + (int)(fenetreLancer.h * 0.50f)+100, winW);

        char bufferCase[64];
        if (bridge_is_in_jail(ctx, partie))
            snprintf(bufferCase, sizeof(bufferCase), "Case : En prison !");
        else
            snprintf(bufferCase, sizeof(bufferCase), "Case : %s", bridge_get_case_joueur_actif(ctx, partie));
        printTextCentered(renderer, fontNormal, bufferCase, blanc,fenetreLancer.y + (int)(fenetreLancer.h * 0.65f)+10,winW);
    }

    renderPauseButton(renderer, btn);
}

void renderJoueurActif(SDL_Renderer* renderer, TTF_Font* font, const EtatPartie* partie, ContexteJeu* ctx){
    if(!partie || partie->joueursActif < 0 || partie->joueursActif >= partie->nbJoueurs) return;
    if(!ctx || !ctx->joueurs[partie->joueursActif]) return;  

    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);
    SDL_Color blanc = {235, 221, 192, 255};

    char bufferJoueur[64];
    snprintf(bufferJoueur, sizeof(bufferJoueur), "Au tour de : %s", partie->joueurs[partie->joueursActif].nom);
    printTextCentered(renderer, font, bufferJoueur, blanc, 10*winH/REF_H, winW);

    char bufferCase[64];
    if (bridge_is_in_jail(ctx, partie))
        snprintf(bufferCase, sizeof(bufferCase), "Case : En prison !");
    else
        snprintf(bufferCase, sizeof(bufferCase), "Case : %s", bridge_get_case_joueur_actif(ctx, partie));
    printTextCentered(renderer, font, bufferCase, blanc, 40*winH/REF_H, winW);


}

void renderEvenementActif(SDL_Renderer* renderer, TTF_Font* fontNormal,const ContexteJeu* ctx, int winW, int winH) {
    EvenementInfo ev;
    bridge_get_evenement(ctx, &ev);
    if (!ev.actif) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 180, 60, 30, 200);
    SDL_Rect bandeau = { 0, (int)(65.0f * winH / REF_H)+20, (int)(winW), (int)(46.0f * winH / REF_H) };
    SDL_RenderFillRect(renderer, &bandeau);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    char buf[160];
    snprintf(buf, sizeof(buf), "EVENEMENT : %s  (%d tour(s) restant(s))", ev.nom, ev.tours_restants);
    SDL_Color blanc = {255, 240, 220, 255};
    int lineH = TTF_FontHeight(fontNormal);
    int midY = bandeau.y + bandeau.h / 2;
    printTextCentered(renderer, fontNormal, buf, blanc, midY - lineH, winW);
    printTextCentered(renderer, fontNormal, ev.cases_affectees, blanc, midY + 2, winW);
}


void renderCartePiochee(SDL_Renderer* renderer, TTF_Font* fontNormal,
                         TTF_Font* fontTitle, const EtatPartie* partie,
                         ButtonsPack* btn) {
    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    /* Fond assombri */
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect full = {0, 0, winW, winH};
    SDL_RenderFillRect(renderer, &full);

    /* Fenêtre de carte : couleur selon Chance (orange) ou Communauté (violet) */
    int isComm = (partie->derniereCarte.paquet == 1);
    SDL_Rect fenetre = {
        (int)(340.0f * winW / REF_W),
        (int)(160.0f * winH / REF_H),
        (int)(600.0f * winW / REF_W),
        (int)(360.0f * winH / REF_H),
    };

    /* Fond carte */
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 245);
    SDL_RenderFillRect(renderer, &fenetre);

    /* Bordure colorée */
    if (isComm) SDL_SetRenderDrawColor(renderer, 160, 80, 220, 255);
    else        SDL_SetRenderDrawColor(renderer, 220, 160, 30,  255);
    for (int t = 0; t < 3; t++) {
        SDL_Rect border = { fenetre.x - t, fenetre.y - t,fenetre.w + 2*t, fenetre.h + 2*t };
        SDL_RenderDrawRect(renderer, &border);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    /* Titre du paquet */
    SDL_Color couleurPaquet = isComm ? (SDL_Color){200, 140, 255, 255}: (SDL_Color){255, 200, 50,  255};
    const char* labelPaquet = isComm ? "CAISSE DE COMMUNAUTE" : "CHANCE";
    int lineH = TTF_FontHeight(fontTitle);
    printTextCentered(renderer, fontTitle, labelPaquet, couleurPaquet,fenetre.y + 18, winW);

    /* Séparateur */
    SDL_SetRenderDrawColor(renderer, 100, 100, 130, 255);
    SDL_RenderDrawLine(renderer, fenetre.x + 20, fenetre.y + lineH + 28,fenetre.x + fenetre.w - 20, fenetre.y + lineH + 28);

    /* Titre de la carte */
    SDL_Color blanc = {235, 221, 192, 255};
    int lineHN = TTF_FontHeight(fontNormal);
    int y = fenetre.y + lineH + 44;
    printTextCentered(renderer, fontNormal, partie->derniereCarte.titre, blanc, y, winW);

    /* Description */
    SDL_Color gris = {180, 170, 155, 255};
    y += lineHN + 14;
    printTextCentered(renderer, fontNormal, partie->derniereCarte.description, gris, y, winW);

    /* Valeur : vert si gain, rouge si perte/taxe */
    if (partie->derniereCarte.valeur != 0) {
        char valBuf[48];
        int estPerte = (partie->derniereCarte.effet == 1   /* CARTE_PERTE_ECTS */
                     || partie->derniereCarte.effet == 2); /* CARTE_TAXE_ECO   */
        int valAbs = partie->derniereCarte.valeur < 0 ? -partie->derniereCarte.valeur : partie->derniereCarte.valeur;
        if (estPerte)
            snprintf(valBuf, sizeof(valBuf), "-%d ECTS", valAbs);
        else
            snprintf(valBuf, sizeof(valBuf), "+%d ECTS", valAbs);
        SDL_Color couleurVal = estPerte ? (SDL_Color){220, 70, 70, 255}: (SDL_Color){100, 220, 100, 255};
        y += lineHN + 20;
        printTextCentered(renderer, fontNormal, valBuf, couleurVal, y, winW);

        /* Mention du bonus/malus éco pour les taxes écologiques */
        if (partie->derniereCarte.effet == 2) { /* CARTE_TAXE_ECO */
            EcoGrade grade = (EcoGrade)(int)partie->joueurs[partie->joueursActif].noteEco;
            int reel = calcul_taxe_ecologique(valAbs, grade);
            char ecoBuf[64];
            if (reel != valAbs)
                snprintf(ecoBuf, sizeof(ecoBuf), "(Taxe reelle : %d ECTS - bonus eco Grade %c)",
                         reel, 'A' + (int)grade - 1);
            else
                snprintf(ecoBuf, sizeof(ecoBuf), "(Taxe pleine - Grade %c)", 'A' + (int)grade - 1);
            SDL_Color grisEco = {160, 200, 160, 255};
            y += lineHN + 6;
            printTextCentered(renderer, fontNormal, ecoBuf, grisEco, y, winW);
        }
    }

    /* Bouton OK */
    btn->okCarte.rect = (SDL_Rect){
        (int)(570.0f * winW / REF_W),
        (int)(460.0f * winH / REF_H),
        (int)(140.0f * winW / REF_W),
        (int)(44.0f  * winH / REF_H),
    };
    SDL_SetRenderDrawColor(renderer, isComm ? 140 : 200, isComm ? 60 : 140, isComm ? 200 : 20, 255);
    SDL_RenderFillRect(renderer, &btn->okCarte.rect);
    SDL_SetRenderDrawColor(renderer, 235, 221, 192, 255);
    SDL_RenderDrawRect(renderer, &btn->okCarte.rect);
    printTextCentered(renderer, fontNormal, "OK", blanc,btn->okCarte.rect.y + (btn->okCarte.rect.h - lineHN) / 2, winW);
}

void render(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex, TextsPack* texts, TTF_Font* fontButton, TTF_Font* fontNormal, TTF_Font* fontTitle,Counter* counterJoueurs, TextBoxPack* tboxes, CheckboxPack* checkboxes, EtatPartie* partie, ContexteJeu* ctx, ButtonsPack* btn, PropButtonsPack* pb, TradeButtonsPack* tb, TradeState* ts, Pawn pawns[MAX_PLAYERS], LoadMenuState* lm, RulesMenuState* rm, Animation* diceAnim, PrisonButtonsPack* pb_prison){
    
    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);
    SDL_RenderClear(renderer);

    switch (state) {

        case MENU:
            SDL_RenderCopy(renderer, tex->menu, NULL, NULL);
            printTextCentered(renderer, fontTitle,  texts->menuLoadGame.text,  texts->menuLoadGame.color,  texts->menuLoadGame.y, winW);
            printTextCentered(renderer, fontTitle,  texts->menuNewGame.text,  texts->menuNewGame.color,  texts->menuNewGame.y,  winW);
            printTextCentered(renderer, fontTitle,  texts->menuSave.text,  texts->menuSave.color,  texts->menuSave.y, winW);
            break;

        case NOUVPARTIE:
            SDL_RenderCopy(renderer, tex->newGame, NULL, NULL);
            printTextCentered(renderer, fontTitle,  texts->titleNewGame.text,  texts->titleNewGame.color,  texts->titleNewGame.y,  winW);
            printTextCentered(renderer, fontTitle,  texts->chooseParam.text,   texts->chooseParam.color,   texts->chooseParam.y,   winW);
            printTextCentered(renderer, fontTitle,  texts->numberPlayer.text,  texts->numberPlayer.color,  texts->numberPlayer.y,  winW);
            
            printTextResponsive(renderer, fontButton, &texts->btnLaunchGame);
            printTextResponsive(renderer, fontButton, &texts->btnCancel);
            printTextResponsive(renderer, fontNormal, &texts->initialMoney);
            printTextResponsive(renderer, fontNormal, &texts->tillBankruptcy);
            printTextResponsive(renderer, fontNormal, &texts->numbTurns);
            printTextResponsive(renderer, fontNormal, &texts->event);

            renderCounter(renderer, counterJoueurs, fontTitle);

            renderTextBox(renderer, fontNormal, &tboxes->initialMoney);
            renderTextBox(renderer, fontNormal, &tboxes->numbTurns);
            renderTextBox(renderer, fontNormal, &tboxes->eventFreq);

            SDL_Color couleur = {235, 221, 192, 255};
            renderCheckbox(renderer, fontNormal, &checkboxes->tourInfini,
                           "Tours infinis", couleur);
            break;

        case CHARGERPARTIE:
            renderLoadMenu(renderer, fontTitle, fontNormal, fontButton, lm, tex);
            break;

        case REGLEMENT:
            renderRulesMenu(renderer, fontTitle, fontNormal, fontButton, rm, tex);
            break;

        case PROPRIETE: {
            EtatJeu plateauActuel = PLATEAU2J;
            if(partie->nbJoueurs==3){ 
                plateauActuel=PLATEAU3J;
            }else if(partie->nbJoueurs==4){
                plateauActuel=PLATEAU4J;
            }
            renderPlateau(renderer, plateauActuel, tex, texts, fontButton, fontNormal, partie, ctx, btn, pawns);
            renderEvenementActif(renderer, fontNormal, ctx, winW, winH);
            renderPropWindow(renderer, fontTitle, fontNormal, fontButton, tex, ctx, partie, pb, winW, winH);
            break;
        }

        case TRADE: {
            EtatJeu plateauActuel = PLATEAU2J;
            if(partie->nbJoueurs==3){
                plateauActuel=PLATEAU3J;
            }else if(partie->nbJoueurs==4){
                plateauActuel=PLATEAU4J;
            }
            renderPlateau(renderer, plateauActuel, tex, texts, fontButton, fontNormal, partie, ctx, btn, pawns);
            renderEvenementActif(renderer, fontNormal, ctx, winW, winH);
            renderTradeWindow(renderer, fontTitle, fontNormal, fontButton, tex, ctx, partie, tb, ts, winW, winH);
            break;
        }

        case TEST:
            printf("BONJOUR");
            break;
        case PLATEAU2J:
        case PLATEAU3J:
        case PLATEAU4J:
            renderPlateau(renderer, state, tex, texts, fontButton, fontNormal, partie, ctx, btn, pawns);
            renderEvenementActif(renderer, fontNormal, ctx, winW, winH);
            break;

        case LANCER:
            renderPlateau(renderer, state, tex, texts, fontButton, fontNormal, partie, ctx, btn, pawns);
            renderEvenementActif(renderer, fontNormal, ctx, winW, winH);
            if (diceAnim && diceAnim->sheet) {
                int animSize = (int)(150.0f * winW / REF_W);
                SDL_Rect dst = {(winW - animSize) / 2, (winH - animSize) / 2 - 60, animSize, animSize};
                renderAnim(renderer, diceAnim, 0, 0, &dst);
            }
            break;

        case PRISON: {
            EtatJeu plateauPrison = PLATEAU2J;
            if (partie->nbJoueurs == 3) plateauPrison = PLATEAU3J;
            else if (partie->nbJoueurs == 4) plateauPrison = PLATEAU4J;
            renderPlateau(renderer, plateauPrison, tex, texts, fontButton, fontNormal, partie, ctx, btn, pawns);
            renderEvenementActif(renderer, fontNormal, ctx, winW, winH);
            renderPrisonWindow(renderer, fontTitle, fontNormal, ctx, partie, pb_prison, winW, winH);
            break;
        }

        case CARTE: {
            EtatJeu plateauCarte = PLATEAU2J;
            if (partie->nbJoueurs == 3) plateauCarte = PLATEAU3J;
            else if (partie->nbJoueurs == 4) plateauCarte = PLATEAU4J;
            renderPlateau(renderer, plateauCarte, tex, texts, fontButton, fontNormal, partie, ctx, btn, pawns);
            renderEvenementActif(renderer, fontNormal, ctx, winW, winH);
            renderCartePiochee(renderer, fontNormal, fontTitle, partie, btn);
            break;
        }

        case PAUSE: {
            EtatJeu plateauPause = PLATEAU2J;
            if (partie->nbJoueurs == 3) plateauPause = PLATEAU3J;
            else if (partie->nbJoueurs == 4) plateauPause = PLATEAU4J;
            renderPlateau(renderer, plateauPause, tex, texts, fontButton, fontNormal, partie, ctx, btn, pawns);
            renderPauseMenu(renderer, fontNormal, btn);
            break;
        }

        case GAMEOVER:
            renderGameOver(renderer, fontTitle, fontNormal, fontButton, partie, ctx, btn);
            break;
    }
    SDL_RenderPresent(renderer);
}

void renderGameOver(SDL_Renderer* renderer, TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontButton,const EtatPartie* partie, ContexteJeu* ctx, ButtonsPack* btn) {
    int winW, winH;
    SDL_GetRendererOutputSize(renderer, &winW, &winH);

    /* Fond sombre semi-transparent */
    SDL_SetRenderDrawColor(renderer, 18, 20, 30, 255);
    SDL_RenderClear(renderer);

    /* Panneau central */
    int panW = (int)(winW * 0.6f);
    int panH = (int)(winH * 0.7f);
    int panX = (winW - panW) / 2;
    int panY = (winH - panH) / 2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 30, 28, 22, 230);
    SDL_Rect panel = {panX, panY, panW, panH};
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 200, 175, 100, 255);
    SDL_RenderDrawRect(renderer, &panel);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    /* Déterminer le gagnant via le bridge */
    char winner_name[50] = "???";
    int  winner_value = 0;
    int  winner_mode  = 0; /* 0=tours, 1=faillite */
    bridge_get_winner(ctx, partie, winner_name, &winner_value, &winner_mode);

    SDL_Color gold   = {220, 180, 60,  255};
    SDL_Color blanc  = {235, 221, 192, 255};
    SDL_Color gris   = {160, 150, 130, 255};
    SDL_Color vert   = {100, 220, 100, 255};

    int lineTitle  = TTF_FontHeight(fontTitle);
    int lineNormal = TTF_FontHeight(fontNormal);

    int y = panY + (int)(panH * 0.08f);

    /* Titre : FIN DE PARTIE */
    printTextCentered(renderer, fontTitle, "FIN DE PARTIE", gold, y, winW);
    y += lineTitle + 8;

    /* Ligne décorative */
    SDL_SetRenderDrawColor(renderer, 200, 175, 100, 255);
    SDL_RenderDrawLine(renderer, panX + 20, y, panX + panW - 20, y);
    y += 20;

    /* Sous-titre selon le mode */
    if (winner_mode == 0) {
        char buf[80];
        snprintf(buf, sizeof(buf), "Fin des %d tours", partie->tourMax);
        printTextCentered(renderer, fontNormal, buf, gris, y, winW);
    } else {
        printTextCentered(renderer, fontNormal, "Dernier joueur en vie", gris, y, winW);
    }
    y += lineNormal + 24;

    /* Trophée textuel */
    printTextCentered(renderer, fontTitle, "VAINQUEUR", gold, y, winW);
    y += lineTitle + 16;

    /* Nom du gagnant en grand */
    printTextCentered(renderer, fontTitle, winner_name, blanc, y, winW);
    y += lineTitle + 16;

    /* Valeur totale */
    char valBuf[64];
    snprintf(valBuf, sizeof(valBuf), "Patrimoine total : %d ECTS", winner_value);
    printTextCentered(renderer, fontNormal, valBuf, vert, y, winW);
    y += lineNormal + 28;

    /* Classement des autres joueurs */
    printTextCentered(renderer, fontNormal, "Classement :", blanc, y, winW);
    y += lineNormal + 8;

    /* Trier les joueurs par valeur totale décroissante (simple) */
    int order[4];
    int vals[4];
    int n = partie->nbJoueurs;
    for (int i = 0; i < n; i++) {
        order[i] = i;
        if (ctx && ctx->joueurs[i]) {
            vals[i] = engine_get_player_total_value(ctx->joueurs[i],
                          (Property**)ctx->tableau_prop,
                          (Gares**)ctx->tableau_gar,
                          (Compagnies**)ctx->tableau_comp);
        } else {
            vals[i] = partie->joueurs[i].credits;
        }
    }
    /* Tri à bulles décroissant */
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (vals[order[j]] > vals[order[i]]) {
                int tmp = order[i]; order[i] = order[j]; order[j] = tmp;
            }
        }
    }

    static const char* medals[] = {"1er", "2eme", "3eme", "4eme"};
    for (int rank = 0; rank < n && y < panY + panH - 60; rank++) {
        int idx = order[rank];
        char rowBuf[80];
        const char* pname = ctx && ctx->joueurs[idx] ? ctx->joueurs[idx]->name
                                                      : partie->joueurs[idx].nom;
        int alive = ctx && ctx->joueurs[idx] && ctx->joueurs[idx]->credit >= 0;
        snprintf(rowBuf, sizeof(rowBuf), "%s  %s  —  %d ECTS%s",
                 medals[rank], pname, vals[idx], alive ? "" : " (elimine)");
        SDL_Color rowColor = (rank == 0) ? gold : (alive ? blanc : gris);
        printTextCentered(renderer, fontNormal, rowBuf, rowColor, y, winW);
        y += lineNormal + 4;
    }

    /* Bouton Retour au menu */
    int bW = (int)(panW * 0.4f);
    int bH = 44;
    int bX = panX + (panW - bW) / 2;
    int bY = panY + panH - bH - 20;
    btn->backToMenu.rect = (SDL_Rect){bX, bY, bW, bH};
    SDL_SetRenderDrawColor(renderer, 60, 50, 35, 255);
    SDL_RenderFillRect(renderer, &btn->backToMenu.rect);
    SDL_SetRenderDrawColor(renderer, 200, 175, 100, 255);
    SDL_RenderDrawRect(renderer, &btn->backToMenu.rect);
    int lineBtn = TTF_FontHeight(fontButton);
    printTextCentered(renderer, fontButton, "Retour au menu",blanc, bY + (bH - lineBtn) / 2, winW);
}

void cleanup(SDL_Window* win, SDL_Renderer* rdr, TexturesPack* tex){
    freeTextures(tex);
    SDL_DestroyRenderer(rdr);
    SDL_DestroyWindow(win);
    SDL_Quit();
}