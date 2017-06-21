/*
 * Type definitions
 */

#ifndef TYPES
#define TYPES
typedef struct _NPC {
    int posX;
    int posY;
    int stepX;
    int stepY;
    SDL_Surface* image;
    int imgW;
    int imgH;
} NPC;

typedef struct _BLOCK {
    int posX;
    int posY;
    int resist;
    SDL_Surface* image;
} BLOCK;

typedef struct _RACKET {
    int posX;
    int posY;
    int stepX;
    SDL_Surface* image;
    int imgW;
    int imgH;
    int _left;
    int _right;
    int score;
    int aux_score;
    int lives;
    float factor;
} RACKET;

typedef int _0;
#endif

/*
 * Constants
 */

// Screen dimension constants
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern const int false;
extern const int true;

extern const int BALL_WIDTH;
extern const int BALL_HEIGHT;

extern const int BLOCK_WIDTH;
extern const int BLOCK_HEIGHT;

extern const int RACKET_WIDTH;
extern const int RACKET_HEIGHT;

/* Fixed framerate
 * ---------------
 * Formula: 1000 / (desired fps)
 * TICK_INTERVAL = 16 (~62 fps)
 * TICK_INTERVAL = 17 (~58 fps)
 * TICK_INTERVAL = 33 (~30 fps)
 */
extern const unsigned int TICK_INTERVAL;

// Speed multiplier
extern const int BALL_SPEED;
extern const int BALL_MAX_SPEED;

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

// Debug mode
#ifndef _DEBUG
#define _DEBUG 1
#endif

// Constants of proportion
// Default PROP = 10
// In order to utilize 640 x 480 screen, set PROP to 8
#ifndef PROP
#define PROP 10
#endif

// Amount of balls on screen
#ifndef LEN
#define LEN 1
#endif

// Amount of brick on screen
#ifndef COLUMNS
#define COLUMNS 8
#endif
#ifndef LINES
#define LINES 5
#endif

// Error codes
#ifndef ERR_INIT
#define ERR_INIT 0
#endif
#ifndef ERR_WINDOW_CREATE
#define ERR_WINDOW_CREATE 1
#endif
#ifndef ERR_BLIT
#define ERR_BLIT 2
#endif
#ifndef ERR_OPTIMIZE
#define ERR_OPTIMIZE 3
#endif
#ifndef ERR_IMG_LOAD
#define ERR_IMG_LOAD 4
#endif
#ifndef ERR_INIT_AUDIO
#define ERR_INIT_AUDIO 5
#endif
#ifndef ERR_WAV_LOAD
#define ERR_WAV_LOAD 6
#endif
#ifndef ERR_FONT_LOAD
#define ERR_FONT_LOAD 7
#endif
#ifndef ERR_EST_EGG
#define ERR_EST_EGG -99
#endif

/*
 * Function Prototypes
 */

// Starts up SDL and creates window
int init(void);

// Loads media
int loadMedia(void);

// Frees media and shuts down SDL
void closef(void);

// Game states
void menu(void);
void game(void);
void options(void);
void ranking(void);
void configuration(void);

// Loads individual image
SDL_Surface* loadSurface(char *path);

// Returns NPC struct with given values
NPC createNPC(int posX, int posY, int stepX, int stepY, SDL_Surface *image);

// Create BLOCK
BLOCK createBLOCK(int posX, int posY, int resist, SDL_Surface *image);

// Create RACKET
RACKET createRACKET(int posX, int posY, int stepX, SDL_Surface *image,
                    int score, int aux_score, int lives, float factor);

// Updates NPC position via stepX and stepY
int moveNPC(NPC *p);

// Updates RACKET position via stepX
void moveRACKET(RACKET *p);

// Draws image on gScreenSurface
int drawOnScreen(SDL_Surface* image,
                int srcX, int srcY,
                int srcW, int srcH,
                int dstX, int dstY);

// Draws text on gScreenSurface
int drawTextOnScreen(char* text, int dstX, int dstY, SDL_Color color);

// Returns the time left until next tick
unsigned time_left(void);

// Collision
void collisionBalls(void);
void collisionBrick(void);
void collisionRacket(void);

// Print errors
void error(int code);

// Helper functions
int absolute(int n);

// New Level
void newLevel(void);

// Random generators
int randneg(void);
int randposi(void);

int Mix_PausedMusic(void);
