#include "defs.h"

/*
 * Global Variables
 */

extern int quit;

// Pause variable
extern int gPause;
extern int gSound;
extern int gMusic;

// Power up
extern int gPowerUp;
extern int powerup_x;
extern int powerup_y;
extern int controlInverter;
extern int bigracket;
extern int smallracket;

// Bonus time
extern int gTime;
extern int bonus;

// Define game screen
extern int gameScreen;
extern int gScreen;
extern int gGameMode;
extern int gPhysics;

extern int can_music_play;

// The window to render to
extern SDL_Window* gWindow;

// The ball
extern NPC ball[LEN];
extern BLOCK brick[COLUMNS][LINES];
extern RACKET player;

// The surface contained by the window
extern SDL_Surface* gScreenSurface;

// The ball image
extern SDL_Surface* gBall;

// Current displayed PNG image
extern SDL_Surface* gBRICKSurface;

// Current displayed PNG image (racket)
extern SDL_Surface* gPLAYERSurface;
extern SDL_Surface* gPLAYERSMALLSurface;
extern SDL_Surface* gPLAYERLARGESurface;

// Icon image
extern SDL_Surface* gIcon;

// Image for buttons
extern SDL_Surface* buttonnew;
extern SDL_Surface* buttonoptions;
extern SDL_Surface* buttonrankings;
extern SDL_Surface* buttonhome;
extern SDL_Surface* buttonendless;
extern SDL_Surface* buttoncampaign;
extern SDL_Surface* buttonclassic;
extern SDL_Surface* buttonalternate;
extern SDL_Surface* buttonplay;
extern SDL_Surface* breakout;
extern SDL_Surface* config;
extern SDL_Surface* pause;
extern SDL_Surface* buttonquit;
extern SDL_Surface* optionsback;
extern SDL_Surface* optionsback1;
extern SDL_Surface* arrow_right;
extern SDL_Surface* arrow_left;

// Image for power up
extern SDL_Surface* power_up;

// Image for side_bar
extern SDL_Surface* side_bar;

//Image for sound icon
extern SDL_Surface* sound;

// Control variable for optimal FPS handling
extern Uint32 next_time;

// Check Level Clearance
extern int levelClear;
extern int level;

// Check if ball is in game
extern int ball_in_game;

// Brick breaking sound
extern Mix_Chunk* gBrickWAV;

// Racket collision sound
extern Mix_Chunk* gRacketWAV;

// Game music
extern Mix_Music* gMusicWAV;

// Game font
extern TTF_Font* gFont;

// Check if ttf needs to update
extern int contalevel;
extern int contascore;
extern int contalives;
extern int contabonus;
extern int blocklevel;
extern int blockscore;
extern int blocklives;
extern int blockbonus;
