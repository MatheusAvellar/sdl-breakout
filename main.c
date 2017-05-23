/*
 *  Simple Breakout game in SDL
 *
 *    by the great minds of
 *       Matheus Avellar
 *         Pedro Dias
 *        Pedro Possato
 *
 *         (C) 2017.1
 */

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*
 * Type definitions
 */

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
    int lives;
} RACKET;


/*
 * Constants
 */

// Constants of proportion
// Default PROP = 10
// In order to utilize 640 x 480 screen, set PROP to 8
#define PROP 10

// Screen dimension constants
const int SCREEN_WIDTH = 80 * PROP;
const int SCREEN_HEIGHT = 60 * PROP;

const int false = 0;
const int true = 1;

const int BALL_WIDTH = 3 * PROP;
const int BALL_HEIGHT = 3 * PROP;

const int BLOCK_WIDTH = 10 * PROP;
const int BLOCK_HEIGHT = 5 * PROP;

const int RACKET_WIDTH = 18 * PROP;
const int RACKET_HEIGHT = 2 * PROP;

/* Fixed framerate
 * ---------------
 * Formula: 1000 / (desired fps)
 * TICK_INTERVAL = 16 (~62 fps)
 * TICK_INTERVAL = 17 (~58 fps)
 * TICK_INTERVAL = 33 (~30 fps)
 */
const unsigned int TICK_INTERVAL = 17;

// Speed multiplier
const int BALL_SPEED = 2;

// Amount of balls on screen
#define LEN 1

// Amount of brick on screen
#define COLUMNS 8
#define LINES 1

// Error codes
#define ERR_INIT 0
#define ERR_WINDOW_CREATE 1
#define ERR_BLIT 2
#define ERR_OPTIMIZE 3
#define ERR_IMG_LOAD 4

/*
 * Global Variables
 */

//Define game screen
int gameScreen = 1;

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

// Control variable for optimal FPS handling
static Uint32 next_time;


/*
 * Function Prototypes
 */

// Starts up SDL and creates window
int init(void);

// Loads media
int loadMedia(void);

// Frees media and shuts down SDL
void close(void);

// Game states
void menu(void);
void game(void);

// Loads individual image
SDL_Surface* loadSurface(char *path);

// Returns NPC struct with given values
NPC createNPC(int posX, int posY, int stepX, int stepY, SDL_Surface *image);

// Create BLOCK
BLOCK createBLOCK(int posX, int posY, int resist, SDL_Surface *image);

// Create RACKET
RACKET createRACKET(int posX, int posY, int stepX, SDL_Surface *image,
                    int score, int lives);

// Updates NPC position via stepX and stepY
void moveNPC(NPC *p);

// Updates RACKET position via stepX
void moveRACKET(RACKET *p);

// Draws image on gScreenSurface
int drawOnScreen(SDL_Surface* image,
                int srcX, int srcY,
                int srcW, int srcH,
                int dstX, int dstY);

// Returns the time left until next tick
unsigned time_left(void);

// Collision
void collisionBalls(void);

// Check Level Clearance
int levelClear;

// Print errors
void error(int code);


int main(int argc, char* args[]) {


    // Start up SDL and create window
    if(!init() || !loadMedia()) {
        printf("Failed to initialize!\n");
        return 1;
    }

    if (gameScreen == 0) {
        menu();
    } else if (gameScreen == 1) {
        game();
    } else {
        printf("Error: game screen is invalid\n");
        return 1;
    }

    // Frees resources and closes SDL
    close();

    // Supress warnings from [-Wunused-parameter] flag
    (void)argc;
    (void)args;

    return 0;
}

void menu(void) {
    // Main loop flag
    int quit = false;

    // Event handler
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                        quit = true;
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }
        // Fill the surface with #000000 (black)
        SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format,
                                                    0x00, 0x00, 0x00));

        // Update the surface
        SDL_UpdateWindowSurface(gWindow);

        // Normalize framerate
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
}

void game(void) {
    // Iteration variables
    int i, j, k, l, m;

    // Event handler
    SDL_Event e;

    // Main loop flag
    int quit = false;

    // Create NPC
    int _posX = SCREEN_WIDTH / 2 - BALL_WIDTH / 2;
    int _posY = SCREEN_HEIGHT / 2 - BALL_HEIGHT / 2;

    ball[0] = createNPC(_posX, _posY,
                /* TODO: Ball should start over pad and only go when
                 * user presses the space key
                 */
                rand() % 2 ? -1 : 1,
                rand() % 2 ? -1 : 1,
                gBall);

    //Create BRICKS
    for (i = 0; i < COLUMNS; i++) {
        for (j = 0; j < LINES; j++) {
            brick[i][j] = createBLOCK(BLOCK_WIDTH*i, BLOCK_HEIGHT*j, 1,
                                    gBRICKSurface);
        }
    }

    // Create RACKET
    l = 2;
    m = 28;
    player = createRACKET(RACKET_WIDTH * l - 58,
                        // 58 = 42 + 4² -> magic number
                        RACKET_HEIGHT * m, 5, gPLAYERSurface, 0, 3);
    player._left = false;
    player._right = false;

    // While application is running
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;

                    player._left = player._left
                                || e.key.keysym.sym == SDLK_LEFT;

                    player._right = player._right
                                || e.key.keysym.sym == SDLK_RIGHT;
                    break;
                case SDL_KEYUP:
                    if (e.key.keysym.sym == SDLK_LEFT) player._left = false;
                    if (e.key.keysym.sym == SDLK_RIGHT) player._right = false;
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }

        // Fill the surface with #99d9ea (light blue)
        SDL_FillRect(gScreenSurface, NULL,
                  SDL_MapRGB(gScreenSurface->format,
                              0x99, 0xD9, 0xEA));

        moveNPC(&ball[0]);

        if (drawOnScreen(ball[0].image, 0, 0,
                    BALL_WIDTH,
                    BALL_HEIGHT,
                    ball[0].posX,
                    ball[0].posY) < 0) {
            error(ERR_BLIT);
            quit = true;
        }

        // Draw bricks
        for (i= 0; i < COLUMNS; i++) {
            for (j = 0; j < LINES; j++) {
                if (brick[i][j].resist
                 && drawOnScreen(brick[i][j].image, 0, 0,
                               BLOCK_WIDTH, BLOCK_HEIGHT,
                               brick[i][j].posX, brick[i][j].posY) < 0) {
                    error(ERR_BLIT);
                    quit = true;
                }
            }
        }

        if (player._left) {
            if (player.stepX > 0) player.stepX *= -1;

            player.posX = player.posX > 0
                        ? player.posX + player.stepX
                        : 0;
        } else if (player._right) {
            if (player.stepX < 0) player.stepX *= -1;

            player.posX = player.posX < SCREEN_WIDTH - RACKET_WIDTH
                        ? player.posX + player.stepX
                        : SCREEN_WIDTH - RACKET_WIDTH;
        }

        // Draw pad
        if (drawOnScreen(player.image, 0, 0,
                        RACKET_WIDTH, RACKET_HEIGHT,
                        player.posX, player.posY) < 0) {
            error(ERR_BLIT);
            quit = true;
        }

        // Collision between balls
        collisionBalls();

        // Collision between ball and brick
        for (j = 0; j < COLUMNS; j++) {
            for (k = 0; k < LINES; k++) {
                BLOCK current = brick[j][k];

                int over = ball[0].posY + BALL_HEIGHT >= current.posY;
                int under = ball[0].posY <= current.posY + BLOCK_HEIGHT;
                int left = ball[0].posX <= current.posX + BLOCK_WIDTH;
                int right = ball[0].posX + BALL_WIDTH >= current.posX;

                if (current.resist && over && under && left && right) {
                    player.score += 100;
                    ball[0].stepY *= -1;
                    brick[j][k].resist--;

                    /* TODO: Add stepX *= -1
                     * if ball hits brick on the side
                     */
                }
            }
        }

        levelClear = true;

        for (j = 0; j < COLUMNS; j++) {
            for (k = 0; k < LINES; k++) {
                if (brick[j][k].resist) {
                    levelClear = false;
                }
            }
        }

        if (levelClear == true) {
            player.score += 1000;
            levelClear = false;
        }

        //Testing purposes only
        //printf("%d", player.score);

        // Update the surface
        SDL_UpdateWindowSurface(gWindow);

        // Normalize framerate
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
}

void collisionBalls(void) {
    int i, j;

    // Temporary variable for 2 way value swapping
    int temp;

    for (i= 0; i < LEN; i++) {
        for (j = i + 1; j < LEN; j++) {
            int deltaX = ball[i].posX - ball[j].posX;
            int deltaY = ball[i].posY - ball[j].posY;

            if (deltaX * deltaX + deltaY * deltaY < BALL_WIDTH * BALL_WIDTH) {
                temp = ball[i].stepX;
                ball[i].stepX = ball[j].stepX;
                ball[j].stepX = temp;
                temp = ball[i].stepY;
                ball[i].stepY = ball[j].stepY;
                ball[j].stepY = temp;
            }
        }
    }
}

void moveNPC(NPC *p) {
    p->posX += p->stepX * BALL_SPEED;
    p->posY += p->stepY * BALL_SPEED;

    // If the image is out of bounds on the X axis,
    // invert the direction and reset the position to a valid one
    if (p->posX + BALL_WIDTH > SCREEN_WIDTH) {
        p->stepX *= -1;
        p->posX = SCREEN_WIDTH - BALL_WIDTH;
    } else if (p->posX < 0) {
        p->stepX *= -1;
        p->posX = 0;
    }

    // If the image is out of bounds on the Y axis,
    // invert the direction and reset the position to a valid one
    if (p->posY + BALL_HEIGHT > SCREEN_HEIGHT) {
        p->stepY *= -1;
        p->posY = SCREEN_HEIGHT - BALL_HEIGHT;
    } else if (p->posY < 0) {
        p->stepY *= -1;
        p->posY = 0;
    }
}

// Create NPC
NPC createNPC(int posX, int posY, int stepX, int stepY, SDL_Surface *image) {
    NPC p;

    p.posX = posX;
    p.posY = posY;
    p.stepX = stepX;
    p.stepY = stepY;
    p.image = image;
    return p;
}

// Create BLOCK
BLOCK createBLOCK(int posX, int posY, int resist, SDL_Surface *image) {
    BLOCK p;

    p.posX = posX;
    p.posY = posY;
    p.resist = resist;
    p.image = image;
    return p;
}

// Create RACKET
RACKET createRACKET(int posX, int posY, int stepX, SDL_Surface *image,
                    int score, int lives) {
    RACKET p;

    p.posX = posX;
    p.posY = posY;
    p.stepX = stepX;
    p.image = image;
    p.score = score;
    p.lives = lives;
    return p;
}

int init(void) {
    // Generate unique seed for the random function
    srand((unsigned)time(NULL));

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error(ERR_INIT);
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow("SDL Breakout",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH,
                            SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        error(ERR_WINDOW_CREATE);
        return false;
    }

    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        error(ERR_INIT);
        return false;
    }

    // Get window surface
    gScreenSurface = SDL_GetWindowSurface(gWindow);

    return true;
}

int loadMedia(void) {

    // Load PNG surface
    if((gBall = loadSurface("./images/circle.png")) == NULL) {
        error(ERR_IMG_LOAD);
        return false;
    }

    if((gBRICKSurface = loadSurface("./images/brick.png")) == NULL) {
        error(ERR_IMG_LOAD);
        return false;
    }

    if((gPLAYERSurface = loadSurface("./images/racket.png")) == NULL) {
        error(ERR_IMG_LOAD);
        return false;
    }

    // Color key
    SDL_SetColorKey(gBall, SDL_TRUE,
                    SDL_MapRGB(gBall->format, 0xff, 0xAE, 0xC9));
    SDL_SetColorKey(gPLAYERSurface, SDL_TRUE,
                    SDL_MapRGB(gPLAYERSurface->format, 0xff, 0xAE, 0xC9));

    return true;
}

void close() {
    // Free loaded image
    SDL_FreeSurface(gBall);
    gBall = NULL;
    SDL_FreeSurface(gBRICKSurface);
    gBRICKSurface = NULL;

    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Surface* loadSurface(char *path) {
    // The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    // Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path);

    if (loadedSurface == NULL) {
        error(ERR_IMG_LOAD);
        return NULL;
    }

    // Convert surface to screen format
    optimizedSurface = SDL_ConvertSurface(loadedSurface,
                                        gScreenSurface->format, 0);

    if (optimizedSurface == NULL) {
        error(ERR_OPTIMIZE);
        return NULL;
    }

    // Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
    return optimizedSurface;
}

// Draws to gScreenSurface
int drawOnScreen(SDL_Surface* image,
                int srcX, int srcY,
                int srcW, int srcH,
                int dstX, int dstY) {

    SDL_Rect srcRect, dstRect;

    srcRect.x = srcX;
    srcRect.y = srcY;
    srcRect.w = srcW;
    srcRect.h = srcH;
    dstRect.x = dstX;
    dstRect.y = dstY;

    return SDL_BlitSurface(image, &srcRect, gScreenSurface, &dstRect);
}

// Time left until next tick
unsigned time_left(void) {
    unsigned now = SDL_GetTicks();
    return (next_time <= now) ? 0 : next_time - now;
}

void error(int code) {
  switch(code) {
    case ERR_INIT:
        printf("SDL could not initialize!");
    case ERR_WINDOW_CREATE:
        printf("Window could not be created!");
        break;
    case ERR_BLIT:
        printf("SDL could not blit!");
        break;
    case ERR_OPTIMIZE:
        printf("Unable to optimize image %s!");
    case ERR_IMG_LOAD:
        printf("Unable to load image %s!");
    default:
        printf("Unspecified error!");
        break;
  }
  printf(" SDL Error: %s\n", SDL_GetError());
}
