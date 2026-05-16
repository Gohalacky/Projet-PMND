#ifndef PAWN_H
#define PAWN_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "bridge.h"
#include "assets.h"

#define BOARD_SIZE 40
#define MAX_PLAYERS 4
#define PAWN_REF_W 32
#define PAWN_REF_H 32
#define PAWN_ANIM_SPEED 400.0f
#define PAWN_OFFSET_X 10
#define PAWN_OFFSET_Y 10

typedef struct{
    bool active;
    float srcX, srcY;
    float dstX, dstY;
    float curX, curY;
    int stepsRemaining;
    int nextCase;
    int direction; /* +1 = avance, -1 = recule */
} PawnAnim;

typedef struct {
    int playerIndex;
    int currentCase;
    float x,y;
    PawnAnim anim;
} Pawn;

void initPawns(Pawn pawns[MAX_PLAYERS], int nbJoueurs);

void renderPawns(SDL_Renderer* renderer, const TexturesPack* tex, Pawn pawns[MAX_PLAYERS], int nbJoueurs, int winW, int winH);

void updatePawns(Pawn pawns[MAX_PLAYERS], int nbJoueurs,Uint32 deltaMs);

void startPawnMove(Pawn* pawn, int fromCase, int steps);

void startPawnMoveCard(Pawn* pawn, int fromCase, int steps);

bool isPawnMoving(const Pawn* pawn);

void snapPawnToCase(Pawn* pawn, int caseIndex, Pawn pawns[MAX_PLAYERS], int nbJoueurs);

void getPawnCasePos(int caseIndex, float* outX, float* outY);



#endif