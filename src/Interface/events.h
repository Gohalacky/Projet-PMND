#ifndef EVENST_H
#define EVENST_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "Struct.h"
#include "bridge.h"
#include "propwindow.h"
#include "tradewindow.h"
#include "view.h"
#include "pawn.h"
#include "loadmenu.h"
#include "rules.h"

void handleEvents(SDL_Event* event, EtatJeu* state, ButtonsPack* btn, TextsPack* texts,
                  TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton,
                  bool* quit, Counter* counterJoueurs, TextBoxPack* tboxes,
                  CheckboxPack* checkboxes, EtatPartie* partie, ContexteJeu* ctx,
                  PropButtonsPack* pb, TradeButtonsPack* tb, TradeState* ts,
                  Pawn pawns[MAX_PLAYERS],
                  LoadMenuState* lm,
                  RulesMenuState* rm);

#endif