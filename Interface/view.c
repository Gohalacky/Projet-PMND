#include "view.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_video.h>

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* Textures*/

int loadTextures(SDL_Renderer* renderer, TexturesPack* tex){
    tex->menu                   = IMG_LoadTexture(renderer, "IMAGE/menu.png");
    tex->newGame                = IMG_LoadTexture(renderer, "IMAGE/launchGame.png");
    tex->launchGame             = IMG_LoadTexture(renderer, "IMAGE/chargeGame.png");
    tex->gameBoard2Player       = IMG_LoadTexture(renderer, "IMAGE/gameBoard2Player.png");
    tex->gameBoard3Player       = IMG_LoadTexture(renderer, "IMAGE/gameBoard3Player.png");
    tex->gameBoard4Player       = IMG_LoadTexture(renderer, "IMAGE/gameBoard4Player.png");
    tex->throws                 = IMG_LoadTexture(renderer, "IMAGE/throws.png");
    tex->menuProperties         = IMG_LoadTexture(renderer, "IMAGE/Menu_interne.png");
    SDL_Texture* all[] = {
        tex->menu, tex->newGame, tex->launchGame,
        tex->gameBoard2Player, tex->gameBoard3Player, tex->gameBoard4Player,
        tex->throws, tex->menuProperties
    };
    for (int i = 0; i < 7; i++) {
        if (!all[i]) {
            printf("Erreur chargement texture %d : %s\n", i, IMG_GetError());
            return -1;
        }
    }
    return 0;
}

/* Rendu*/

void render(SDL_Renderer* renderer, EtatJeu state, TexturesPack* tex, TextsPack* texts, TTF_Font* fontButton, TTF_Font* fontNormal, TTF_Font* fontTitle,Counter* counterJoueurs, TextBoxPack* tboxes){
    SDL_RenderClear(renderer);

    switch (state) {

        case MENU:
            SDL_RenderCopy(renderer, tex->menu, NULL, NULL);
            break;

        case NOUVPARTIE:
            SDL_RenderCopy(renderer, tex->newGame, NULL, NULL);
            printText(renderer, fontTitle,  texts->titleNewGame.text,  texts->titleNewGame.color,  texts->titleNewGame.x,  texts->titleNewGame.y);
            printText(renderer, fontTitle,  texts->chooseParam.text,   texts->chooseParam.color,   texts->chooseParam.x,   texts->chooseParam.y);
            printText(renderer, fontTitle,  texts->numberPlayer.text,  texts->numberPlayer.color,  texts->numberPlayer.x,  texts->numberPlayer.y);
            printText(renderer, fontButton, texts->btnLaunchGame.text, texts->btnLaunchGame.color, texts->btnLaunchGame.x, texts->btnLaunchGame.y);
            printText(renderer, fontButton, texts->btnCancel.text,     texts->btnCancel.color,     texts->btnCancel.x,     texts->btnCancel.y);
            printText(renderer, fontButton, texts->btnValid.text,      texts->btnValid.color,      texts->btnValid.x,      texts->btnValid.y);
            printText(renderer, fontNormal, texts->initialMoney.text,  texts->initialMoney.color,  texts->initialMoney.x,  texts->initialMoney.y);
            printText(renderer, fontNormal, texts->tillBankruptcy.text,texts->tillBankruptcy.color,texts->tillBankruptcy.x,texts->tillBankruptcy.y);
            printText(renderer, fontNormal, texts->numbTurns.text,     texts->numbTurns.color,     texts->numbTurns.x,     texts->numbTurns.y);
            printText(renderer, fontNormal, texts->event.text,         texts->event.color,         texts->event.x,         texts->event.y);

            renderCounter(renderer, counterJoueurs, fontTitle, 634, 176);

            /* Zones de saisie */
            renderTextBox(renderer, fontNormal, &tboxes->initialMoney);
            renderTextBox(renderer, fontNormal, &tboxes->numbTurns);
            renderTextBox(renderer, fontNormal, &tboxes->eventFreq);
            break;

        case CHARGERPARTIE:
            SDL_RenderCopy(renderer, tex->launchGame, NULL, NULL);
            break;

        case PROPRIETE:
            SDL_RenderCopy(renderer, tex->menuProperties, NULL, NULL);
            break;

        case TEST:
            printf("BONJOUR");
            break;

        case PLATEAU3J:
            SDL_RenderCopy(renderer, tex->gameBoard3Player, NULL, NULL);
            printText(renderer, fontNormal, texts->btnThrowDice.text,  texts->btnThrowDice.color,  texts->btnThrowDice.x,  texts->btnThrowDice.y);
            printText(renderer, fontNormal, texts->btnProperties.text, texts->btnProperties.color, texts->btnProperties.x, texts->btnProperties.y);
            printText(renderer, fontNormal, texts->btnEndTurn.text,    texts->btnEndTurn.color,    texts->btnEndTurn.x,    texts->btnEndTurn.y);
            break;

        case PLATEAU4J:
            SDL_RenderCopy(renderer, tex->gameBoard4Player, NULL, NULL);
            printText(renderer, fontNormal, texts->btnThrowDice.text,  texts->btnThrowDice.color,  texts->btnThrowDice.x,  texts->btnThrowDice.y);
            printText(renderer, fontNormal, texts->btnProperties.text, texts->btnProperties.color, texts->btnProperties.x, texts->btnProperties.y);
            printText(renderer, fontNormal, texts->btnEndTurn.text,    texts->btnEndTurn.color,    texts->btnEndTurn.x,    texts->btnEndTurn.y);
            break;

        case PLATEAU2J:
        case LANCER:
            SDL_RenderCopy(renderer, tex->gameBoard2Player, NULL, NULL);
            printText(renderer, fontNormal, texts->btnThrowDice.text,  texts->btnThrowDice.color,  texts->btnThrowDice.x,  texts->btnThrowDice.y);
            printText(renderer, fontNormal, texts->btnProperties.text, texts->btnProperties.color, texts->btnProperties.x, texts->btnProperties.y);
            printText(renderer, fontNormal, texts->btnEndTurn.text,    texts->btnEndTurn.color,    texts->btnEndTurn.x,    texts->btnEndTurn.y);
            if (state == LANCER) {
                SDL_Rect fenetreLancer = {340, 100, 600, 400};
                SDL_RenderCopy(renderer, tex->throws, NULL, &fenetreLancer);
            }
            break;
    }

    SDL_RenderPresent(renderer);
}

/* Nettoyage */

void cleanup(SDL_Window* win, SDL_Renderer* rdr, TexturesPack* tex){
    SDL_DestroyTexture(tex->menu);
    SDL_DestroyTexture(tex->newGame);
    SDL_DestroyTexture(tex->launchGame);
    SDL_DestroyTexture(tex->gameBoard2Player);
    SDL_DestroyTexture(tex->gameBoard3Player);
    SDL_DestroyTexture(tex->gameBoard4Player);
    SDL_DestroyTexture(tex->throws);
    SDL_DestroyTexture(tex->menuProperties);
    SDL_DestroyTexture(tex->test);
    SDL_DestroyRenderer(rdr);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
}

/* Evenements */
bool isClicked(Button* btn, int x, int y) {
    return x >= btn->rect.x && x <= btn->rect.x + btn->rect.w &&
           y >= btn->rect.y && y <= btn->rect.y + btn->rect.h;
}

void handleEvents(SDL_Event* event, EtatJeu* state, ButtonsPack* btn, TextsPack* texts,TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton,bool* quit, Counter* counterJoueurs, TextBoxPack* tboxes){
    while (SDL_PollEvent(event)) {

        if (event->type == SDL_QUIT) {
            *quit = true;
            return;
        }

        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE)
            *state = MENU;

        
        if (*state == NOUVPARTIE) {
            handleTextBoxEvent(event, &tboxes->initialMoney);
            handleTextBoxEvent(event, &tboxes->numbTurns);
            handleTextBoxEvent(event, &tboxes->eventFreq);
        }

        if (event->type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            printf("clic de souris : x=%d, y=%d\n", x, y);

        
            if (*state == NOUVPARTIE)
                handleTextBoxClick(tboxes, x, y);

            switch (*state) {
                case MENU:
                    if (isClicked(&btn->newGame,  x, y)) *state = NOUVPARTIE;
                    if (isClicked(&btn->loadGame, x, y)) *state = CHARGERPARTIE;
                    if (isClicked(&btn->save,     x, y)) *state = TEST;
                    if (isClicked(&btn->ranking,  x, y)) *state = TEST;
                    break;

                case NOUVPARTIE:
                    if (isClicked(&btn->launchGame, x, y)) {
                        switch (counterJoueurs->value) {
                            case 2: *state = PLATEAU2J; break;
                            case 3: *state = PLATEAU3J; break;
                            case 4: *state = PLATEAU4J; break;
                        }
                    }
                    if (isClicked(&btn->cancel,     x, y)) *state = MENU;
                    if (isClicked(&btn->validMoney, x, y)) {
                        printf("Argent initial : %s | Tours : %s | Evenements : %s\n",
                               tboxes->initialMoney.text,
                               tboxes->numbTurns.text,
                               tboxes->eventFreq.text);
                    }
                    handleCounter(counterJoueurs, x, y);
                    break;

                case PLATEAU2J:
                case PLATEAU3J:
                case PLATEAU4J:
                case LANCER:
                    if (isClicked(&btn->backToMenu, x, y)) *state = MENU;
                    if (isClicked(&btn->throwDice,  x, y)) *state = LANCER;
                    if (isClicked(&btn->properties, x, y)) *state = PROPRIETE;
                    break;

                default: break;
            }
        }

        if (event->type == SDL_WINDOWEVENT &&
            event->window.event == SDL_WINDOWEVENT_RESIZED)
        {
            float ratioX = (float)event->window.data1 / REF_W;
            float ratioY = (float)event->window.data2 / REF_H;

            Button* allButtons[] = {
                &btn->newGame, &btn->loadGame, &btn->save, &btn->ranking,
                &btn->backToMenu, &btn->throwDice, &btn->properties,
                &btn->launchGame, &btn->cancel, &btn->validMoney
            };
            int count = sizeof(allButtons) / sizeof(allButtons[0]);
            for (int i = 0; i < count; i++) {
                ScaleButton(allButtons[i], ratioX, ratioY);
                ScaleButton(&counterJoueurs->btnPlus,  ratioX, ratioY);
                ScaleButton(&counterJoueurs->btnMoins, ratioX, ratioY);
                ScaleText(&counterJoueurs->label, ratioX, ratioY);
            }

            Text* allTexts[] = {
                &texts->titleNewGame, &texts->btnLaunchGame, &texts->btnCancel, &texts->event,
                &texts->numberPlayer, &texts->chooseParam, &texts->initialMoney,
                &texts->tillBankruptcy, &texts->numbTurns, &texts->btnValid
            };
            int textCount = sizeof(allTexts) / sizeof(allTexts[0]);
            for (int i = 0; i < textCount; i++)
                ScaleText(allTexts[i], ratioX, ratioY);

            ScaleAllTextBoxes(tboxes, ratioX, ratioY);
            ScaleFonts(fontNormal, fontTitle, fontButton, ratioY);
        }
    }
}


void printText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y) {
    if (!font) { printf("Erreur : font NULL\n"); return; }
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) { SDL_FreeSurface(surface); return; }
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void ScaleText(Text* t, float ratioX, float ratioY){
    t->x    = (int)(t->refX    * ratioX);
    t->y    = (int)(t->refY    * ratioY);
    t->size = (int)(t->refSize * ratioY);
}

void ScaleFonts(TTF_Font** fontNormal, TTF_Font** fontTitle, TTF_Font** fontButton, float ratioY) {
    TTF_CloseFont(*fontNormal);
    TTF_CloseFont(*fontTitle);
    TTF_CloseFont(*fontButton);
    *fontNormal = TTF_OpenFont("FONT/fontPixel.ttf", (int)(32 * ratioY));
    *fontTitle  = TTF_OpenFont("FONT/fontPixel.ttf", (int)(38 * ratioY));
    *fontButton = TTF_OpenFont("FONT/fontPixel.ttf", (int)(22 * ratioY));
    if (!*fontNormal || !*fontTitle)
        printf("Erreur ScaleFonts : %s\n", TTF_GetError());
}


void ScaleButton(Button* btn, float ratioX, float ratioY){
    btn->rect.x = (int)(btn->ref.x * ratioX);
    btn->rect.y = (int)(btn->ref.y * ratioY);
    btn->rect.w = (int)(btn->ref.w * ratioX);
    btn->rect.h = (int)(btn->ref.h * ratioY);
}


void initCounter(Counter* c, int min, int max, int startValue,int xPlus, int yPlus, int xMoins, int yMoins,int w, int h, int xText, int yText){
    c->value = startValue;
    c->min   = min;
    c->max   = max;
    c->label = (Text){
        .refX    = xText,
        .refY    = yText,
        .refSize = 38,
        .x       = xText,
        .y       = yText,
        .size    = 38,
        .color   = {235, 221, 192, 255}
    };
    c->btnPlus.ref  = (SDL_Rect){xPlus,  yPlus,  w, h};
    c->btnPlus.rect = c->btnPlus.ref;
    c->btnMoins.ref  = (SDL_Rect){xMoins, yMoins, w, h};
    c->btnMoins.rect = c->btnMoins.ref;
}

void handleCounter(Counter* c, int mouseX, int mouseY) {
    if (isClicked(&c->btnPlus,  mouseX, mouseY) && c->value < c->max) c->value++;
    if (isClicked(&c->btnMoins, mouseX, mouseY) && c->value > c->min) c->value--;
}

void renderCounter(SDL_Renderer* renderer, Counter* c, TTF_Font* font, int xText, int yText) {
    (void)xText; (void)yText; 
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", c->value);
    printText(renderer, font, buf, c->label.color, c->label.x, c->label.y);
}


void initTextBox(TextBox* tb, int refX, int refY, int refW, int refH) {
    memset(tb->text, 0, sizeof(tb->text));
    tb->cursor_pos = 0;
    tb->active     = 0;
    tb->ref  = (SDL_Rect){refX, refY, refW, refH};
    tb->rect = tb->ref;
}

void ScaleTextBox(TextBox* tb, float ratioX, float ratioY) {
    tb->rect.x = (int)(tb->ref.x * ratioX);
    tb->rect.y = (int)(tb->ref.y * ratioY);
    tb->rect.w = (int)(tb->ref.w * ratioX);
    tb->rect.h = (int)(tb->ref.h * ratioY);
}

void ScaleAllTextBoxes(TextBoxPack* tboxes, float ratioX, float ratioY) {
    ScaleTextBox(&tboxes->initialMoney, ratioX, ratioY);
    ScaleTextBox(&tboxes->numbTurns,    ratioX, ratioY);
    ScaleTextBox(&tboxes->eventFreq,    ratioX, ratioY);
}

static int isInTextBox(TextBox* tb, int x, int y) {
    return x >= tb->rect.x && x <= tb->rect.x + tb->rect.w &&
           y >= tb->rect.y && y <= tb->rect.y + tb->rect.h;
}


void handleTextBoxClick(TextBoxPack* tboxes, int mouseX, int mouseY) {
    TextBox* all[] = { &tboxes->initialMoney, &tboxes->numbTurns, &tboxes->eventFreq };
    for (int i = 0; i < 3; i++) {
        if (isInTextBox(all[i], mouseX, mouseY)) {
            all[i]->active = 1;
            SDL_StartTextInput();
        } else {
            all[i]->active = 0;
        }
    }

    if (!tboxes->initialMoney.active &&
        !tboxes->numbTurns.active    &&
        !tboxes->eventFreq.active)
        SDL_StopTextInput();
}


void handleTextBoxEvent(SDL_Event* event, TextBox* tb) {
    if (!tb->active) return;

    if (event->type == SDL_TEXTINPUT) {

        int len = (int)strlen(tb->text);
        int addLen = (int)strlen(event->text.text);
        if (len + addLen >= TEXTBOX_MAX_LEN) return;

    
        memmove(&tb->text[tb->cursor_pos + addLen],
                &tb->text[tb->cursor_pos],
                len - tb->cursor_pos + 1);
        memcpy(&tb->text[tb->cursor_pos], event->text.text, addLen);
        tb->cursor_pos += addLen;
    }

    if (event->type == SDL_KEYDOWN) {
        int len = (int)strlen(tb->text);
        switch (event->key.keysym.sym) {

            case SDLK_BACKSPACE:
                if (tb->cursor_pos > 0) {
                    memmove(&tb->text[tb->cursor_pos - 1],
                            &tb->text[tb->cursor_pos],
                            len - tb->cursor_pos + 1);
                    tb->cursor_pos--;
                }
                break;

            case SDLK_DELETE:
                if (tb->cursor_pos < len)
                    memmove(&tb->text[tb->cursor_pos],
                            &tb->text[tb->cursor_pos + 1],
                            len - tb->cursor_pos);
                break;

            case SDLK_LEFT:
                if (tb->cursor_pos > 0) tb->cursor_pos--;
                break;

            case SDLK_RIGHT:
                if (tb->cursor_pos < len) tb->cursor_pos++;
                break;

            case SDLK_HOME:
                tb->cursor_pos = 0;
                break;

            case SDLK_END:
                tb->cursor_pos = len;
                break;

            default: break;
        }
    }
}

void renderTextBox(SDL_Renderer* renderer, TTF_Font* font, TextBox* tb) {

    SDL_SetRenderDrawColor(renderer, 245, 240, 225, 255);
    SDL_RenderFillRect(renderer, &tb->rect);

    
    if (tb->active)
        SDL_SetRenderDrawColor(renderer, 180, 140, 60, 255);
    else
        SDL_SetRenderDrawColor(renderer, 120, 110, 90, 255);
    SDL_RenderDrawRect(renderer, &tb->rect);

    if (font && strlen(tb->text) > 0) {
        SDL_Color color = {40, 30, 20, 255};
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, tb->text, color);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            if (tex) {
            
                int pad = 6;
                SDL_Rect dst = {
                    tb->rect.x + pad,
                    tb->rect.y + (tb->rect.h - surf->h) / 2,
                    surf->w,
                    surf->h
                };
            
                if (dst.w > tb->rect.w - 2 * pad)
                    dst.w = tb->rect.w - 2 * pad;
                SDL_RenderCopy(renderer, tex, NULL, &dst);
                SDL_DestroyTexture(tex);
            }
            SDL_FreeSurface(surf);
        }
    }

    if (tb->active && (SDL_GetTicks() / 500) % 2 == 0) {
        int cursor_x = tb->rect.x + 6; 

        if (font && tb->cursor_pos > 0) {
            char tmp[TEXTBOX_MAX_LEN];
            strncpy(tmp, tb->text, tb->cursor_pos);
            tmp[tb->cursor_pos] = '\0';
            int w = 0;
            TTF_SizeUTF8(font, tmp, &w, NULL);
            cursor_x += w;
        }

        SDL_SetRenderDrawColor(renderer, 40, 30, 20, 255);
        SDL_RenderDrawLine(renderer,
                           cursor_x, tb->rect.y + 4,
                           cursor_x, tb->rect.y + tb->rect.h - 4);
    }
}
