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

// Define game screen
extern int gameScreen;
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
TTF_Font* gFont;