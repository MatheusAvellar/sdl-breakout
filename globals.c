#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "defs.h"

/*
 * Global Variables
 */

// Main loop flag
int quit = 0;

// Pause variable
int gPause;
int gSound;
int gMusic;

// Define game screen
int gameScreen = 0;
int gScreen = 0;
int gGameMode = 1;
int gPhysics = 1;

int can_music_play = 1;

//Power up
int gPowerUp;
int powerup_x;
int powerup_y;

// Bonus time
int gTime;
int bonus;

// The window to render to
SDL_Window* gWindow = NULL;

// The ball
NPC ball[LEN];
BLOCK brick[COLUMNS][LINES];
RACKET player;

// The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

// The ball image
SDL_Surface* gBall = NULL;

// Current displayed PNG image
SDL_Surface* gBRICKSurface = NULL;

// Current displayed PNG image (racket)
SDL_Surface* gPLAYERSurface = NULL;

// Icon image
SDL_Surface* gIcon = NULL;

// Image for buttons
SDL_Surface* buttonnew = NULL;
SDL_Surface* buttonoptions = NULL;
SDL_Surface* buttonrankings = NULL;
SDL_Surface* buttonhome = NULL;
SDL_Surface* buttonendless = NULL;
SDL_Surface* buttoncampaign = NULL;
SDL_Surface* buttonclassic = NULL;
SDL_Surface* buttonalternate = NULL;
SDL_Surface* buttonplay = NULL;
SDL_Surface* breakout = NULL;
SDL_Surface* config = NULL;
SDL_Surface* optionsback = NULL;
SDL_Surface* optionsback1 = NULL;
SDL_Surface* pause = NULL;
SDL_Surface* buttonquit = NULL;
SDL_Surface* arrow_right = NULL;
SDL_Surface* arrow_left = NULL;

//Image for power up
SDL_Surface* power_up = NULL;

// Image for side_bar
SDL_Surface* side_bar = NULL;

//Image for sound icon
SDL_Surface* sound = NULL;

// Control variable for optimal FPS handling
Uint32 next_time;

// Check Level Clearance
int levelClear = COLUMNS*LINES;
int level = 0;

// Check if ball is in game
int ball_in_game;

// Brick breaking sound
Mix_Chunk* gBrickWAV;

// Racket collision sound
Mix_Chunk* gRacketWAV;

// Game music
Mix_Music* gMusicWAV;

// Game font
TTF_Font* gFont;

// Check if ttf needs to update
int contalevel = 1;
int contascore = 0;
int contalives = 3;
int contabonus = 600;
int blocklevel = 0;
int blockscore = 0;
int blocklives = 0;
int blockbonus = 0;
