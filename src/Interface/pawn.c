#include "pawn.h"
#include "assets.h"
#include "view.h"

#include <SDL2/SDL_render.h>
#include <math.h>
#include <string.h>

typedef struct {
    float x,y;
}CasePos;

static const CasePos BOARD_POS[BOARD_SIZE]={

    // TOUT A CHANGER QUAND NOUVEAU PLATEAU + IL MANQUE UNE CASE
    { 930.0f, 660.0f },
    { 867.0f, 660.0f },
    { 813.0f, 660.0f },
    { 752.0f, 660.0f },
    { 690.0f, 660.0f },
    { 645.0f, 660.0f },
    { 586.0f, 660.0f },
    { 514.0f, 660.0f },
    { 454.0f, 660.0f },
    { 410.0f, 660.0f },
    { 328.0f, 660.0f },
    
 
    { 325.0f, 574.0f },
    { 325.0f, 510.0f },
    { 325.0f, 460.0f },
    { 325.0f, 416.0f },
    { 325.0f, 354.0f },
    { 325.0f, 298.0f },
    { 325.0f, 260.0f },
    { 325.0f, 234.0f },
    { 325.0f,  169.0f },
    { 325.0f,  85.0f },
 
    { 407.0f,  70.0f },
    { 462.0f,  70.0f },
    { 532.0f,  70.0f },
    { 588.0f,  70.0f },
    { 641.0f,  70.0f },
    { 691.0f,  70.0f },
    { 760.0f,  70.0f },
    { 820.0f,  70.0f },
    { 862.0f,  70.0f },
    { 953.0f,  70.0f },

 
    { 950.0f,  156.0f },
    { 950.0f, 227.0f },
    { 950.0f, 260.0f },
    { 950.0f, 292.0f },
    { 950.0f, 367.0f },
    { 950.0f, 407.0f },
    { 950.0f, 471.0f },
    { 950.0f, 516.0f },
    { 950.0f, 572.0f },
};

typedef struct{
    float dx,dy;
}SlotOffset;

static const SlotOffset SLOT_OFFSETS[MAX_PLAYERS][MAX_PLAYERS]={
    { {  0.0f,  0.0f }, {  0.0f,  0.0f }, {  0.0f,  0.0f }, {  0.0f,  0.0f } },
    { { -7.0f,  0.0f }, {  7.0f,  0.0f }, {  0.0f,  0.0f }, {  0.0f,  0.0f } },
    { { -7.0f,  5.0f }, {  7.0f,  5.0f }, {  0.0f, -7.0f }, {  0.0f,  0.0f } },
    { { -7.0f, -7.0f }, {  7.0f, -7.0f }, { -7.0f,  7.0f }, {  7.0f,  7.0f } },
};

static int nextBoardCase(int current){
    return (current+1)%BOARD_SIZE;
}

static int prevBoardCase(int current){
    return (current-1+BOARD_SIZE)%BOARD_SIZE;
}

static void computeSlotOffset(const Pawn* pawn, int targetCase, const Pawn pawns[MAX_PLAYERS], int nbJoueurs, float* outDx, float* outDy){
    int coloc[MAX_PLAYERS];
    int count=0;

    for(int i=0; i<nbJoueurs; i++){
        const Pawn* other=&pawns[i];
        if(other->anim.active){
            continue;
        }
        if(other->currentCase != targetCase){
            continue;
        }
        coloc[count++]=other->playerIndex;
    }
    if(count==0){
        *outDx=0.0f;
        *outDy=0.0f;
        return;
    }

    int slot=0;
    for(int s=0; s<count; s++){
        if(coloc[s]==pawn->playerIndex){
            slot=s;
            break;
        }
    }

    int row=(count-1<MAX_PLAYERS) ? count-1 : MAX_PLAYERS-1;
    *outDx = SLOT_OFFSETS[row][slot].dx;
    *outDy = SLOT_OFFSETS[row][slot].dy;
}

static void refreshCaseLayout(int caseIndex, Pawn pawns[MAX_PLAYERS], int nbJoueurs){
    for(int i =0; i<nbJoueurs;i++){
        Pawn* p =&pawns[i];
        if(p->anim.active){
            continue;
        }
        if(p->currentCase!=caseIndex){
            continue;
        }
        float dx,dy;
        computeSlotOffset(p, caseIndex, pawns, nbJoueurs, &dx, &dy);
        p->x=BOARD_POS[caseIndex].x+dx;
        p->y=BOARD_POS[caseIndex].y+dy;
    }
}

void getPawnCasePos(int caseIndex, float *outX, float *outY){
    if(caseIndex<0 || caseIndex >= BOARD_SIZE){
        *outX=0.0f;
        *outY=0.0f;
        return;
    }
    *outX=BOARD_POS[caseIndex].x;
    *outY=BOARD_POS[caseIndex].y;
}

void initPawns(Pawn pawns[MAX_PLAYERS], int nbJoueurs){
    for(int i=0; i<MAX_PLAYERS;i++){
        pawns[i].playerIndex=i;
        pawns[i].currentCase=0;
        pawns[i].anim.active=false;
        pawns[i].anim.stepsRemaining=0;
        pawns[i].anim.direction=1;
        pawns[i].anim.nextCase=0;
        pawns[i].anim.srcX=BOARD_POS[0].x;
        pawns[i].anim.srcY=BOARD_POS[0].y;
        pawns[i].anim.curX=BOARD_POS[0].x;
        pawns[i].anim.curY=BOARD_POS[0].y;
        pawns[i].anim.dstX=BOARD_POS[0].x;
        pawns[i].anim.dstY=BOARD_POS[0].y;
        pawns[i].x=BOARD_POS[0].x;
        pawns[i].y=BOARD_POS[0].y;
    }
    refreshCaseLayout(0, pawns, nbJoueurs);
}

void snapPawnToCase(Pawn* pawn, int caseIndex, Pawn pawns[MAX_PLAYERS], int nbJoueurs){
    if(!pawn || caseIndex<0 || caseIndex>=BOARD_SIZE){
        return;
    }

    int previousCase=pawn->currentCase;
    pawn->currentCase=caseIndex;
    pawn->anim.active=false;
    pawn->anim.stepsRemaining=0;
    pawn->x=BOARD_POS[caseIndex].x;
    pawn->y=BOARD_POS[caseIndex].y;

    if(previousCase!=caseIndex) {
        refreshCaseLayout(previousCase, pawns, nbJoueurs);
    }
    refreshCaseLayout(caseIndex, pawns, nbJoueurs);
}

void startPawnMove(Pawn* pawn, int fromCase, int steps){
    if(!pawn || steps<=0){
        return;
    }

    PawnAnim* a =&pawn->anim;

    a->srcX=pawn->x;
    a->srcY=pawn->y;
    a->curX=pawn->x;
    a->curY=pawn->y;

    a->direction=1;
    a->nextCase=nextBoardCase(fromCase);
    a->dstX=BOARD_POS[a->nextCase].x;
    a->dstY=BOARD_POS[a->nextCase].y;

    a->stepsRemaining=steps;
    a->active=true;
}

void startPawnMoveCard(Pawn* pawn, int fromCase, int steps){
    if(!pawn || steps==0){
        return;
    }

    PawnAnim* a=&pawn->anim;

    a->srcX=pawn->x;
    a->srcY=pawn->y;
    a->curX=pawn->x;
    a->curY=pawn->y;

    a->direction=(steps>0)?1:-1;
    a->nextCase=(a->direction>0)?nextBoardCase(fromCase):prevBoardCase(fromCase);
    a->dstX=BOARD_POS[a->nextCase].x;
    a->dstY=BOARD_POS[a->nextCase].y;

    a->stepsRemaining=(steps>0)?steps:-steps;
    a->active=true;
}

bool isPawnMoving(const Pawn *pawn){
    if(!pawn){
        return false;
    }
    return pawn->anim.active;
}

void updatePawns(Pawn pawns[MAX_PLAYERS], int nbJoueurs,Uint32 deltaMs){
    float dt= deltaMs/1000.0f;

    for(int i=0; i<nbJoueurs;i++){
        Pawn* p=&pawns[i];
        PawnAnim* a=&p->anim;

        if(!a->active){continue;}

        float dx= a->dstX-a->curX;
        float dy= a->dstY-a->curY;
        float dist=sqrtf(dx*dx+dy*dy);

        if(dist<1.0f){
            a->curX=a->dstX;
            a->curY=a->dstY;
            p->x=a->dstX;
            p->y=a->dstY;

            int arrivedCase =a->nextCase;
            a->stepsRemaining--;
            if(a->stepsRemaining<=0){
                a->active=false;
                p->currentCase=arrivedCase;
                refreshCaseLayout(arrivedCase, pawns, nbJoueurs);
            }else{
                a->srcX=a->curX;
                a->srcY=a->curY;
                a->nextCase=(a->direction>=0)?nextBoardCase(arrivedCase):prevBoardCase(arrivedCase);
                a->dstX=BOARD_POS[a->nextCase].x;
                a->dstY=BOARD_POS[a->nextCase].y;
            }
        }else{
            float speed=PAWN_ANIM_SPEED*dt;
            if(speed>=dist){
                a->curX=a->dstX;
                a->curY=a->dstY;
            }else{
                a->curX+=(dx/dist)*speed;
                a->curY+=(dy/dist)*speed;
            }
            p->x=a->curX;
            p->y=a->curY;
        }
    }
}

void renderPawns(SDL_Renderer* renderer, const TexturesPack* tex, Pawn pawns[MAX_PLAYERS], int nbJoueurs, int winW, int winH){
    if(!renderer|| !tex||!pawns){
        return;
    }

    float scaleX= winW/REF_W;
    float scaleY= winH/REF_H;
    int pawnW = PAWN_REF_W*scaleX;
    int pawnH = PAWN_REF_H*scaleY;

    for(int i=0; i<nbJoueurs;i++){
        const Pawn* p=&pawns[i];
        int screenX= (p->x*scaleX)-pawnW/2;
        int screenY= (p->y*scaleY)-pawnH/2;
        SDL_Rect dst={screenX, screenY, pawnW, pawnH};

        if(tex->pion[i]){
            SDL_RenderCopy(renderer, tex->pion[i], NULL, &dst);
        }else{
            static const SDL_Color PAWN_COLORS[MAX_PLAYERS]={
                {220,50,50,255},
                {50,120,220,255},
                {50,200,80,255},
                {230,180,30,255},
            };
            const SDL_Color* c = &PAWN_COLORS[i];
            SDL_SetRenderDrawColor(renderer, c->r,c->g,c->b,c->a);
            SDL_RenderFillRect(renderer,&dst);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &dst);
        }
    }
}