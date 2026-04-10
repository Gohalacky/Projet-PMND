#include "anim.h"

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <stdint.h>
#include <stdio.h>

void initAnim(Animation *anim, SDL_Texture *sheet, int frameW, int frameH, int frameCount, int fps, bool loop){
    anim->sheet = sheet;
    anim->frameW = frameW;
    anim->frameH = frameH;
    anim->frameCount = frameCount;
    anim->fps =fps;
    anim->loop=loop;
    anim->currentFrame = 0;
    anim->accumulator=0;
}

bool updateAnim(Animation* anim, Uint32 deltaMs){
    if(anim->frameCount<=1) return false;

    Uint32 frameDuration = (anim->fps > 0)? (1000u / (Uint32)anim->fps) : 1000u;

    anim->accumulator += deltaMs;

    if(anim->accumulator < frameDuration) return false;

    anim->accumulator -= frameDuration;
    anim->currentFrame++;

    if(anim->currentFrame >= anim->frameCount){
        if(anim->loop){
            anim->currentFrame=0;
        }else{
            anim->currentFrame=anim->frameCount-1;
            return true;
        }
    }
    return false;
}

void renderAnim(SDL_Renderer *renderer, Animation *anim, int x, int y, SDL_Rect *dst){
    if(!anim->sheet){
        printf("renderAnim : texture NULL\n");
        return;
    }

    SDL_Rect src={anim->currentFrame*anim->frameW,0,anim->frameW, anim->frameH};

    if(dst){
        SDL_RenderCopy(renderer, anim->sheet, &src, dst);
    }else{
        SDL_Rect fallback = {x,y,anim->frameW,anim->frameH};
        SDL_RenderCopy(renderer, anim->sheet, &src, &fallback);
    }
}

void resetAnim(Animation* anim){
    anim->currentFrame=0;
    anim->accumulator=0;
}