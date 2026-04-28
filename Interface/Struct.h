#ifndef STRUCT 
#define STRUCT

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define TEXTBOX_MAX_LEN 16

typedef enum { ALIGN_LEFT, ALIGN_CENTER_X, ALIGN_RIGHT} TextAlign;
typedef enum {MENU,NOUVPARTIE,CHARGERPARTIE,PLATEAU2J,PLATEAU3J,PLATEAU4J,LANCER,PROPRIETE,TRADE, TEST} EtatJeu;

typedef struct {
    SDL_Rect rect;      
    SDL_Rect ref;  
    int disabled;     
} Button;

typedef struct {
    char     text[TEXTBOX_MAX_LEN]; 
    int      cursor_pos;            
    int      active;                
    SDL_Rect rect;                  
    SDL_Rect ref;                   
} TextBox;

typedef struct {
    const char* text;
    int refX, refY;     
    int refSize;        
    int x, y;          
    int size;
    TextAlign align;           
    SDL_Color color;
} Text;

typedef struct {
    int value;
    int min;
    int max;
    Text label;
    Button btnPlus;
    Button btnMoins;
} Counter;

/*A MODIFIER LORS DU LIEN INTERFACE <-> CONTROLEUR*/
typedef struct{
    char nom[32];
    int  credits;
    float noteEco;
}Joueur;

typedef struct{
    Joueur joueurs[4];
    int nbJoueurs;
    int joueursActif;
    int tourActuel;
    int tourMax;
    int dejaLance;
    Uint32 temps_lancer;
}EtatPartie;

#endif