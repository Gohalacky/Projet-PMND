#include "building.h"
#include "../Model/property.h"

#include <stddef.h>

#define BOARD_W 1280.0f
#define BOARD_H 720.0f

#define BUILD_MAISON_W 36
#define BUILD_MAISON_H 29
#define BUILD_IMMEUBLE_W 57
#define BUILD_IMMEUBLE_H 52
#define BUILD_PARC_W 36
#define BUILD_PARC_H 29

typedef struct{
    float cx; 
    float cy;
} PropPos;

static const PropPos PROP_POS[NB_PROPRIETE]={
    { 860, 613 }, 
    {  746, 613 }, 
    {  570, 613 }, 
    {  452, 613 }, 
    {  398, 613 }, 

    {  380, 570 }, 
    {  380, 465 }, 
    {  380, 413 }, 
    {  380, 280 }, 
    {  380, 219 }, 
    {  380, 133 }, 

    {  404,  115 }, 
    {  516,  115 }, 
    {  583,  115 }, 
    {  691,  115 }, 
    {  760,  115 }, 
    { 851,  115 }, 

    { 905, 159 }, 
    { 905, 223 }, 
    { 905, 299 }, 
    { 905, 418 }, 
    { 905, 471 }, 
};

void renderBuildings(SDL_Renderer *renderer, const ContexteJeu *ctx, const TexturesPack *tex, int winW, int winH){
    
    if(!renderer|| ! ctx || !tex){
        return;
    }

    if(!ctx->tableau_prop[0]){
        return;
    }
    
    float rx = (float)winW / BOARD_W;   /* ratio horizontal */
    float ry = (float)winH / BOARD_H;   /* ratio vertical */

    for(int i=0; i<NB_PROPRIETE;i++){
        const Property* prop= ctx->tableau_prop[i];

        if(!prop||prop->type==Vide||prop->level<=0){
            continue;
        }

        int lvl = prop->level-1;
        if(lvl<0)  lvl=0;
        if(lvl>2)  lvl=2;

        SDL_Texture* buildTex = NULL;
        int refW, refH;

        switch(prop->type){
            case Maison:
                buildTex=tex->maison[lvl];
                refW=BUILD_MAISON_W;
                refH=BUILD_MAISON_H;
                break;
            case Social:
                buildTex=tex->immeuble[lvl];
                refW=BUILD_IMMEUBLE_W;
                refH=BUILD_IMMEUBLE_H;
                break;
            case Parc:
                buildTex=tex->parc[lvl];
                refW=BUILD_PARC_W;
                refH=BUILD_PARC_H;
                break;
            default:
                continue;
        }

        if(!buildTex) continue;

        int drawW = (int)(refW * rx);
        int drawH = (int)(refH * ry);
        int drawX = (int)(PROP_POS[i].cx * rx) - drawW / 2;   
        int drawY = (int)(PROP_POS[i].cy * ry) - drawH / 2;

        SDL_Rect dst = {drawX, drawY, drawW, drawH};
        SDL_RenderCopy(renderer, buildTex, NULL, &dst);
    }
}