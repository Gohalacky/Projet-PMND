#ifndef UI_TEXT_H
#define UI_TEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../Struct.h"

void ScaleText(Text *t, float ratioX, float ratioY);
void ScaleAllTexts(Text **texts, int count, float ratioX, float ratioY);

void ScaleFonts(TTF_Font **fontNormal, TTF_Font **fontTitle, TTF_Font **fontButton, float ratioY, const char* fontPath);

void printText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int x, int y);

void printTextCentered(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int y, int winW);
void printTextResponsive(SDL_Renderer *renderer, TTF_Font *font, const Text *t);

#endif