#include "events.h"

#include "Struct.h"
#include "bridge.h"
#include "propwindow.h"
#include "tradewindow.h"
#include "ui/button.h"
#include "ui/text.h"
#include "ui/counter.h"
#include "ui/textbox.h"
#include "view.h"
#include "pawn.h"
#include "savegame.h"
#include "pausemenu.h"
#include "loadmenu.h"

#include <stdio.h>
#include <stdlib.h>

#define FONT "FONT/fontPixel.ttf"
#define DELAI_LANCER 2500 //en ms

static void handleResize(float ratioX, float ratioY, ButtonsPack* btn, TextsPack* texts,TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton,Counter* counterJoueurs, TextBoxPack* tboxes, CheckboxPack* checkboxes, PropButtonsPack* pb, TradeButtonsPack* tb, LoadMenuState* lm){
    
    Button* allButtons[] = {
       &btn->newGame, &btn->loadGame, &btn->save,
       &btn->backToMenu, &btn->throwDice, &btn->properties,
       &btn->trade, &btn->launchGame, &btn->cancel, &btn->endTurn,
       &btn->pauseMenu, &btn->pauseResume, &btn->pauseSaveQuit,
    };
    int btnCount = sizeof(allButtons) / sizeof(allButtons[0]);
    for (int i = 0; i < btnCount; i++) {
        ScaleButton(allButtons[i], ratioX, ratioY);
    }
    ScaleButton(&counterJoueurs->btnPlus,  ratioX, ratioY);
    ScaleButton(&counterJoueurs->btnMoins, ratioX, ratioY);
    ScaleText(&counterJoueurs->label, ratioX, ratioY);

    Text* allTexts[] = {
        &texts->btnLaunchGame, &texts->btnCancel, &texts->initialMoney,
        &texts->tillBankruptcy, &texts->numbTurns, &texts->event, 
        &texts->btnThrowDice, &texts->btnProperties,&texts->btnEndTurn, &texts->indNbTurn
    };
    int textCount = sizeof(allTexts) / sizeof(allTexts[0]);
    ScaleAllTexts(allTexts, textCount, ratioX, ratioY);

    Text* centeredTexts[]={
        &texts->menuNewGame, &texts->menuLoadGame, &texts->menuSave, 
        &texts->titleNewGame,&texts->chooseParam, &texts->numberPlayer
    };
    int centeredCount= sizeof(centeredTexts)/sizeof(centeredTexts[0]);
    for (int i =0 ; i<centeredCount; i++){
        centeredTexts[i]->y=(int)(centeredTexts[i]->refY*ratioY);
    }
    ScaleAllTextBoxes(tboxes, ratioX, ratioY);
    ScaleFonts(fontNormal, fontTitle, fontButton, ratioY,FONT);
    ScaleCheckbox(&checkboxes->tourInfini, ratioX, ratioY);
    scalePropButtons(pb, ratioX, ratioY);
    scaleTradeButtons(tb, ratioX, ratioY);
    scaleLoadMenu(lm, ratioX, ratioY);
}

static void handleClick(int x, int y, EtatJeu* state, ButtonsPack* btn, Counter* counterJoueurs,TextBoxPack* tboxes, CheckboxPack* checkboxes, EtatPartie* partie,ContexteJeu* ctx, PropButtonsPack* pb, TradeButtonsPack* tb, TradeState* ts, Pawn pawns[MAX_PLAYERS], LoadMenuState* lm){
    if (*state == NOUVPARTIE) handleTextBoxClick(tboxes, x, y);
 
    switch(*state){
        case MENU: 
            if (isClicked(&btn->newGame,  x, y)) *state = NOUVPARTIE;
            if (isClicked(&btn->loadGame, x, y)) {
                initLoadMenu(lm);
                *state = CHARGERPARTIE;
            }
            if (isClicked(&btn->save,     x, y)) *state = TEST;
            break;
        
        case NOUVPARTIE:
            if (isClicked(&btn->launchGame, x, y)) {
                int argentInitial = atoi(tboxes->initialMoney.text);
                if(argentInitial <=0) argentInitial =1500;
 
                /* tourMax = 0 si tours infinis, sinon valeur saisie */
                int tourMax = 0;
                if (!checkboxes->tourInfini.checked) {
                    tourMax = atoi(tboxes->numbTurns.text);
                    if(tourMax <= 0) tourMax = 10;
                }
 
                partie->nbJoueurs    = counterJoueurs->value;
                partie->joueursActif = 0;
                partie->tourActuel   = 1;
                partie->tourMax      = tourMax;
 
                for(int i=0; i<partie->nbJoueurs;i++){
                    partie->joueurs[i].credits=argentInitial;
                    partie->joueurs[i].noteEco=10.0f;
                    snprintf(partie->joueurs[i].nom,32,"Etudiant %d",i+1);
                }
 
                if(bridge_init(ctx,partie)<0){
                    printf("Erreur : bridge_init a échoué, partie annulée.\n");
                    break;
                }

                initPawns(pawns,partie->nbJoueurs);
 
                printf("Argent initial : %s | Tours : %s | Evenements : %s\n",
                        tboxes->initialMoney.text,
                        tboxes->numbTurns.text,
                        tboxes->eventFreq.text);
 
                switch (counterJoueurs->value) {
                    case 2: *state = PLATEAU2J; break;
                    case 3: *state = PLATEAU3J; break;
                    case 4: *state = PLATEAU4J; break;
                }
            }
            if (isClicked(&btn->cancel, x, y)) *state = MENU;
            handleCounter(counterJoueurs, x, y);
            /* clic sur la checkbox tours infinis */
            if (isChecked(&checkboxes->tourInfini, x, y))
                toggleCheckbox(&checkboxes->tourInfini);
            break;
 
        case PLATEAU2J:
        case PLATEAU3J:
        case PLATEAU4J:
            if (isClicked(&btn->backToMenu, x, y)) {
                bridge_free(ctx);
                *state = MENU;
            }

            /* Bouton menu pause ☰ */
            if (isClicked(&btn->pauseMenu, x, y)) {
                *state = PAUSE;
                break;
            }

            if (isClicked(&btn->throwDice, x, y) && btn->throwDice.disabled != 1) {
                int ja = partie->joueursActif;
                int previousPos = ctx->joueurs[ja]->position;
                bridge_roll_and_move(ctx, partie);

                int dice = ctx->dernier_de;
                startPawnMove(&pawns[ja], previousPos, dice);
                partie->temps_lancer = SDL_GetTicks();
                partie->dejaLance = 1;

                btn->endTurn.disabled = 0;
                if (bridge_is_double(ctx)) {
                    btn->throwDice.disabled = 0; 
                } else {
                    btn->throwDice.disabled = 1;
                }
                *state = LANCER;
            }
            if (isClicked(&btn->endTurn, x, y)) {
                bridge_play_turn(ctx, partie);
                partie->dejaLance = 0;
                btn->throwDice.disabled = 0;
                btn->endTurn.disabled = 1;
                bridge_reset_doubles(ctx);
                int fini = bridge_next_turn(partie);
                if (fini) {
                    bridge_free(ctx); *state = MENU;
                } else {
                    switch(partie->nbJoueurs){ case 2: *state=PLATEAU2J; break; case 3: *state=PLATEAU3J; break; case 4: *state=PLATEAU4J; break; }
                }
            }
            if (isClicked(&btn->properties, x, y)) {
                *state = PROPRIETE;
            }
            break;
 
        case LANCER:
            if (isClicked(&btn->backToMenu, x, y)) {
                partie->temps_lancer = 0;
                bridge_free(ctx);
                *state = MENU;
            }
            /* Bouton menu pause ☰ */
            if (isClicked(&btn->pauseMenu, x, y)) {
                *state = PAUSE;
                break;
            }
            if (isClicked(&btn->endTurn, x, y)) {
                bridge_play_turn(ctx, partie);
                partie->temps_lancer = 0;
                partie->dejaLance = 0;
                btn->throwDice.disabled = 0;
                btn->endTurn.disabled = 1;
                bridge_reset_doubles(ctx);           /* reset avant next_turn */
                int fini = bridge_next_turn(partie);
                if (fini) { 
                    bridge_free(ctx); *state = MENU; 
                } else { 
                    switch(partie->nbJoueurs){ 
                        case 2: *state=PLATEAU2J; break; 
                        case 3: *state=PLATEAU3J; break; 
                        case 4: *state=PLATEAU4J; break; 
                    }
                }
            }
            break;

        case CHARGERPARTIE:
            handleLoadMenuClick(lm, x, y, state, partie, ctx, pawns);
            break;

        case PROPRIETE:
            handlePropClick(x, y, state, pb, ctx, partie, ts);
            break;

        case TRADE:
            handleTradeClick(x, y, state, tb, ctx, partie, ts);
            break;

        case PAUSE:
            /* Reprendre la partie */
            if (isClicked(&btn->pauseResume, x, y)) {
                switch (partie->nbJoueurs) {
                    case 2: *state = PLATEAU2J; break;
                    case 3: *state = PLATEAU3J; break;
                    case 4: *state = PLATEAU4J; break;
                    default: *state = PLATEAU2J; break;
                }
            }
            /* Sauvegarder & Quitter */
            if (isClicked(&btn->pauseSaveQuit, x, y)) {
                savegame_save(partie, ctx);
                bridge_free(ctx);
                *state = MENU;
            }
            break;

        default: break;
    }
}

static void endThrow(EtatJeu* state, EtatPartie* partie, ContexteJeu* ctx, ButtonsPack* btn){
    partie->temps_lancer = 0;
    switch(partie->nbJoueurs){
        case 2: *state = PLATEAU2J; break;
        case 3: *state = PLATEAU3J; break;
        case 4: *state = PLATEAU4J; break;
    }
}

void handleEvents(SDL_Event* event, EtatJeu* state,ButtonsPack* btn, TextsPack* texts, TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton, bool* quit, Counter* counterJoueurs, TextBoxPack* tboxes, CheckboxPack* checkboxes, EtatPartie* partie, ContexteJeu* ctx, PropButtonsPack* pb, TradeButtonsPack* tb, TradeState* ts, Pawn pawns[MAX_PLAYERS], LoadMenuState* lm) {

    if(*state==LANCER && partie->temps_lancer>0){
        if(SDL_GetTicks()- partie->temps_lancer>= DELAI_LANCER){
            endThrow(state, partie, ctx, btn);
            return;
        }
    }
    
    while (SDL_PollEvent(event)) {
 
        if (event->type == SDL_QUIT) {
            bridge_free(ctx);
            *quit = true;
            return;
        }
 
        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
            /* ESC : toggle menu pause si on est sur le plateau, sinon retour menu */
            if (*state == PLATEAU2J || *state == PLATEAU3J || *state == PLATEAU4J || *state == LANCER) {
                *state = PAUSE;
            } else if (*state == PAUSE) {
                switch (partie->nbJoueurs) {
                    case 2: *state = PLATEAU2J; break;
                    case 3: *state = PLATEAU3J; break;
                    case 4: *state = PLATEAU4J; break;
                    default: *state = PLATEAU2J; break;
                }
            } else {
                bridge_free(ctx);
                *state = MENU;
            }
        }
 
        if (*state == NOUVPARTIE) {
            handleTextBoxEvent(event, &tboxes->initialMoney);
            handleTextBoxEvent(event, &tboxes->numbTurns);
            handleTextBoxEvent(event, &tboxes->eventFreq);
        }
 
        if (event->type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            printf("Click en x : %d ; y : %d\n", x,y);
            handleClick(x, y, state, btn, counterJoueurs, tboxes, checkboxes, partie, ctx, pb, tb, ts, pawns, lm);
        }
 
        if (event->type == SDL_WINDOWEVENT &&
            event->window.event == SDL_WINDOWEVENT_RESIZED) {
            float ratioX = (float)event->window.data1 / REF_W;
            float ratioY = (float)event->window.data2 / REF_H;
            handleResize(ratioX, ratioY, btn, texts, fontNormal, fontTitle, fontButton, counterJoueurs, tboxes, checkboxes, pb, tb, lm);
        }
    }
}