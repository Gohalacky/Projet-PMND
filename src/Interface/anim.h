#ifndef ANIM_H
#define ANIM_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct{
    SDL_Texture* sheet;
    int frameW,frameH;
    int frameCount;
    int framesPerRow;
    int fps;
    bool loop;
    int currentFrame;
    Uint32 accumulator;
}Animation;

void initAnim(Animation* anim, SDL_Texture* sheet, int frameW, int frameH, int frameCount, int framesPerRow, int fps, bool loop);

bool updateAnim(Animation* anim, Uint32 deltaMs);

void renderAnim(SDL_Renderer* renderer, Animation* anim, int x, int y, SDL_Rect* dst);

void resetAnim(Animation* anim);


#endif