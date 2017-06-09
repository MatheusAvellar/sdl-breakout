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

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
/* TODO: #include <SDL2/SDL_ttf.h>*/
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
    int aux_score;
    int lives;
    float factor;
} RACKET;


/*
 * Constants
 */

// Debug mode
#define _DEBUG 1

// Constants of proportion
// Default PROP = 10
// In order to utilize 640 x 480 screen, set PROP to 8
#define PROP 10

// Screen dimension constants
const int SCREEN_WIDTH = 100 * PROP;
const int SCREEN_HEIGHT = 70 * PROP;

const int false = 0;
const int true = 1;

const int BALL_WIDTH = 3 * PROP;
const int BALL_HEIGHT = 3 * PROP;

const int BLOCK_WIDTH = 10 * PROP;
const int BLOCK_HEIGHT = 5 * PROP;

const int RACKET_WIDTH = 13 * PROP;
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
const int BALL_SPEED = 9;
const int BALL_MAX_SPEED = 11;

// Amount of balls on screen
#define LEN 1

// Amount of brick on screen
#define COLUMNS 8
#define LINES 5

// Error codes
#define ERR_INIT 0
#define ERR_WINDOW_CREATE 1
#define ERR_BLIT 2
#define ERR_OPTIMIZE 3
#define ERR_IMG_LOAD 4
#define ERR_INIT_AUDIO 5
#define ERR_WAV_LOAD 6

/*
 * Global Variables
 */

// Pause variable
int gPause;

// Define game screen
int gameScreen = 0;

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

// Image for buttons
SDL_Surface* buttonplay = NULL;
SDL_Surface* buttonoptions = NULL;
SDL_Surface* buttonrankings = NULL;
SDL_Surface* breakout = NULL;

// Image for side_bar
SDL_Surface* side_bar = NULL;

// Control variable for optimal FPS handling
static Uint32 next_time;

// Check Level Clearance
int levelClear = COLUMNS*LINES;
int level = 1;

// Check if ball is in game
int ball_in_game;

// Brick breaking sound
Mix_Chunk* gBrickWAV;

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
//void options(void);

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

// Main loop flag
int quit = 0;


int main(int argc, char* args[]) {

    // Start up SDL and create window
    if(!init() || !loadMedia()) {
        printf("Failed to initialize!\n");
        return 1;
    }

    while (!quit) {
      if (gameScreen == 0) {
          menu();
      } else if (gameScreen == 1) {
          game();
      //} else if (gameScreen == 2) {
        //  options();
      } else {
          printf("Error: game screen is invalid\n");
          return 1;
      }
    }

    // Frees resources and closes SDL
    close();

    // Supress warnings from [-Wunused-parameter] flag
    (void)argc;
    (void)args;

    return 0;
}

void menu(void) {

    int buttonplay_x = 300;
    int buttonplay_y = 255;
    int buttonplay_w = 400;
    int buttonplay_h = 50;

    int buttonoptions_x = 325;
    int buttonoptions_y = 375;
    int buttonoptions_w = 350;
    int buttonoptions_h = 50;

    int buttonrankings_x = 350;
    int buttonrankings_y = 495;
    int buttonrankings_w = 300;
    int buttonrankings_h = 50;

    int breakout_x = 100;
    int breakout_y = 20;
    int breakout_w = 800;
    int breakout_h = 160;

    // Mouse position
    int mouseX, mouseY;

    // Event handler
    SDL_Event e;

    while (!quit) {
        // Get current mouse position
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }

        // Check if mouse is over buttonplay
        if (mouseX >= buttonplay_x
        && mouseX <= buttonplay_x + buttonplay_w
        && mouseY >= buttonplay_y
        && mouseY <= buttonplay_y + buttonplay_h) {

            SDL_SetColorKey(buttonplay, SDL_FALSE,
                            SDL_MapRGB(buttonplay->format, 0x99, 0xD9, 0xEA));

            // Check if buttonplay is pressed
            if (SDL_GetMouseState(NULL, NULL)
                && SDL_BUTTON(SDL_BUTTON_LEFT)) {
                gameScreen = 1;
                return;
            }
        } else {
            SDL_SetColorKey(buttonplay, SDL_TRUE,
                            SDL_MapRGB(buttonplay->format, 0x99, 0xD9, 0xEA));
        }

        // Check if mouse is over buttonoptions
        if (mouseX >= buttonoptions_x
        && mouseX <= buttonoptions_x + buttonoptions_w
        && mouseY >= buttonoptions_y
        && mouseY <= buttonoptions_y + buttonoptions_h) {

            SDL_SetColorKey(buttonoptions, SDL_FALSE,
                         SDL_MapRGB(buttonoptions->format, 0x99, 0xD9, 0xEA));

            // Check if buttonoptions is pressed
            if(SDL_GetMouseState(NULL, NULL)
            && SDL_BUTTON(SDL_BUTTON_LEFT)) {
                quit = true;
            }
        } else {
            SDL_SetColorKey(buttonoptions, SDL_TRUE,
                         SDL_MapRGB(buttonoptions->format, 0x99, 0xD9, 0xEA));
        }

        // Check if mouse is over buttonrankings
        if (mouseX >= buttonrankings_x
        && mouseX <= buttonrankings_x + buttonrankings_w
        && mouseY >= buttonrankings_y
        && mouseY <= buttonrankings_y + buttonrankings_h) {

            SDL_SetColorKey(buttonrankings, SDL_FALSE,
                        SDL_MapRGB(buttonrankings->format, 0x99, 0xD9, 0xEA));

            //C heck if buttonrankings is pressed
            if(SDL_GetMouseState(NULL, NULL)
            && SDL_BUTTON(SDL_BUTTON_LEFT)) {
                quit = true;
            }
        } else {
            SDL_SetColorKey(buttonrankings, SDL_TRUE,
                        SDL_MapRGB(buttonrankings->format, 0x99, 0xD9, 0xEA));
        }

        // Fill the surface with #000000 (black)
        SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format,
                                                    0x00, 0x00, 0x00));

        if(drawOnScreen(buttonplay, 0, 0,
                         buttonplay_w, buttonplay_h,
                         buttonplay_x, buttonplay_y) < 0
        || drawOnScreen(buttonoptions, 0, 0,
                         buttonoptions_w, buttonoptions_h,
                         buttonoptions_x, buttonoptions_y) < 0
        || drawOnScreen(buttonrankings, 0, 0,
                         buttonrankings_w, buttonrankings_h,
                         buttonrankings_x, buttonrankings_y) < 0
        || drawOnScreen(breakout, 0, 0,
                         breakout_w, breakout_h,
                         breakout_x, breakout_y) < 0) {
            /* TODO: Clean this if condition */
            error(ERR_BLIT);
            quit = true;
        }

        // Update the surface
        SDL_UpdateWindowSurface(gWindow);

        // Normalize framerate
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
}

void game(void) {
    // Iteration variables
    int i, j, l, m, z;

    int _temp_score = 0;

    // Event handler
    SDL_Event e;

    ball_in_game = false;

    //Create BRICKS
    for (i = 0; i < COLUMNS; i++) {
        for (j = 0; j < LINES; j++) {
            brick[i][j] = createBLOCK(BLOCK_WIDTH*i, BLOCK_HEIGHT*j, 1,
                                    gBRICKSurface);
        }
    }

    // Create RACKET
    l = 2;
    m = 33;
    z = 75; // = 42 + 42 - (4-(4/2²))² (magic number)
    player = createRACKET(RACKET_WIDTH * l + z,  // int posX
                             RACKET_HEIGHT * m,  // int posY
                                (BALL_SPEED-1),  // int stepX
                                gPLAYERSurface,  // SDL_Surface *image
                                             0,  // int score
                                             0,  // int aux_score
                                             3,  // int lives
                                          2.2);  // float factor
    player._left = false;
    player._right = false;
    gPause = false;

    // Create NPC
    int _posX = player.posX + RACKET_WIDTH/2 - BALL_WIDTH/2;
    int _posY = player.posY - BALL_HEIGHT;

    for (i= 0; i < LEN; i++) {
        ball[i] = createNPC(_posX, _posY, 0, 0, gBall);
    }

    // While application is running
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_ESCAPE) quit = true;

                    player._left = player._left
                                || e.key.keysym.sym == SDLK_LEFT
                                || e.key.keysym.sym == SDLK_a;

                    player._right = player._right
                                || e.key.keysym.sym == SDLK_RIGHT
                                || e.key.keysym.sym == SDLK_d;

                    if(e.key.keysym.sym == SDLK_SPACE
                    && !ball_in_game && !gPause) {
                        for (i = 0; i < LEN; i++) {
                            ball[i].stepX = rand() % 2 ? -1 : 1;
                            ball[i].stepY = -1;
                            ball_in_game = true;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if(e.key.keysym.sym == SDLK_a
                    || e.key.keysym.sym == SDLK_LEFT) player._left = false;

                    if (e.key.keysym.sym == SDLK_d
                    || e.key.keysym.sym == SDLK_RIGHT) player._right = false;

                    if (e.key.keysym.sym == SDLK_p) gPause = !gPause;
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }

        // Fill the surface with #99d9ea (light blue)
        SDL_FillRect(gScreenSurface, NULL,
                    SDL_MapRGB(gScreenSurface->format, 0x99, 0xD9, 0xEA));

        if(!gPause) {
            if(player._left) {
                player.stepX = -absolute(player.stepX);

                player.posX = player.posX > 0
                            ? player.posX + player.stepX
                            : 0;
            } else if(player._right) {
                player.stepX = absolute(player.stepX);

                player.posX = player.posX < (SCREEN_WIDTH - 200) - RACKET_WIDTH
                            ? player.posX + player.stepX
                            : (SCREEN_WIDTH - 200) - RACKET_WIDTH;
            }
        }

        for (i= 0; i < LEN; i++) {
            if (levelClear) {
                // Ball hit bottom of screen
                if(!gPause && moveNPC(&ball[i])) {
                    player.lives -= 1;
                    if(_DEBUG) printf("[Lives] %d\n", player.lives);

                    ball[i].posY = player.posY - BALL_HEIGHT;
                    ball[i].posX = player.posX + RACKET_WIDTH/2 - BALL_WIDTH/2;
                    ball[i].stepX = 0;
                    ball[i].stepY = 0;

                    ball_in_game = false;

                    if(player.lives <= 0) {
                        /* TODO: Player is out of lives -- Game over */
                        if(_DEBUG) {
                            printf("[Player is out of lives!] %d\n", player.lives);
                            newLevel();
                            player.score = 0;
                            player.aux_score = 0;
                            player.lives = 3;
                            player._left = false;
                            player._right = false;
                            menu();
                        }
                    }

                }

                if(drawOnScreen(ball[i].image, 0, 0,
                            BALL_WIDTH,
                            BALL_HEIGHT,
                            ball[i].posX,
                            ball[i].posY) < 0) {
                    error(ERR_BLIT);
                    quit = true;
                }
            }
        }

        // Draw bricks
        int blockX, blockY;

        for (i = 0; i < COLUMNS; i++) {
            for (j = 0; j < LINES; j++) {
                if (j == 0) {
                    blockX = 0;
                    blockY = BLOCK_HEIGHT;
                } else if (j == 1) {
                    blockX = BLOCK_WIDTH;
                    blockY = 0;
                } else if (j == 2) {
                    blockX = 0;
                    blockY = 0;
                } else if (j == 3) {
                    blockX = BLOCK_WIDTH;
                    blockY = BLOCK_HEIGHT;
                }

                if(brick[i][j].resist
                && drawOnScreen(brick[i][j].image,
                                blockX, blockY,
                                BLOCK_WIDTH, BLOCK_HEIGHT,
                                brick[i][j].posX, brick[i][j].posY) < 0) {
                    error(ERR_BLIT);
                    quit = true;
                }

                if (!levelClear) newLevel();
            }
        }

        // Draw pad
        if(drawOnScreen(player.image, 0, 0,
                        RACKET_WIDTH, RACKET_HEIGHT,
                        player.posX, player.posY) < 0) {
            error(ERR_BLIT);
            quit = true;
        }


        // Draw side_bar
        if(drawOnScreen(side_bar, 0, 0,
                        200, 700,
                        SCREEN_WIDTH - 200, 0) < 0) {
            error(ERR_BLIT);
            quit = true;
        }

        // Collision between balls
        if(LEN > 1) collisionBalls();

        // Collision between ball and brick
        collisionBrick();

        // Collision between ball and racket
        if(ball_in_game) collisionRacket();

        // For testing purposes only
        if(_DEBUG && player.score != _temp_score) {
            printf("[Score: %d]\n", player.score);
            //player.aux_score = player.score;
        }
        if(player.score > _temp_score || player.score == 0) {
            _temp_score = player.score;
            //player.aux_score = player.score;
        }
        if(player.aux_score >= 10000) {
            player.lives += 1;
            player.aux_score -= 10000;
        }

        // Update the surface
        SDL_UpdateWindowSurface(gWindow);

        // Normalize framerate
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
}

void collisionBalls(void) {
    // Iteraion variables
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

void collisionBrick(void) {
    // Iteraion variables
    int i, j, k;

    for (i = 0; i < LEN; i++) {
        for (j = 0; j < COLUMNS; j++) {
            for (k = 0; k < LINES; k++) {
                BLOCK current = brick[j][k];

                int over = ball[i].posY + BALL_HEIGHT >= current.posY;
                int under = ball[i].posY <= current.posY + BLOCK_HEIGHT;
                int left = ball[i].posX <= current.posX + BLOCK_WIDTH;
                int right = ball[i].posX + BALL_WIDTH >= current.posX;

                if(current.resist && over && under && left && right) {
                    if(current.resist < 0) {
                        brick[j][k].resist = 0;
                    } else {
                        if(current.resist > 0){
                            player.score += 100;
                            player.aux_score += 100;
                            levelClear--;
                        }
                        brick[j][k].resist = current.resist-1 < 0
                                    ? 0
                                    : current.resist-1;
                        if(!brick[j][k].resist) {
                            Mix_PlayChannel(-1, gBrickWAV, 0);
                        }

                        // Check collision side
                        int ball_half_x = ball[i].posX + BALL_WIDTH / 2;
                        int ball_half_y = ball[i].posY + BLOCK_HEIGHT / 2;
                        int brick_half_x = current.posX + BLOCK_WIDTH / 2;
                        int brick_half_y = current.posY + BLOCK_HEIGHT / 2;

                        int center_dist_x = ball_half_x - brick_half_x;
                        int abs_dist_x = absolute(center_dist_x);

                        int center_dist_y = ball_half_y - brick_half_y;
                        int abs_dist_y = absolute(center_dist_y);

                        int avg_width = (BALL_WIDTH + BLOCK_WIDTH) / 2;
                        int avg_height = (BLOCK_HEIGHT + BLOCK_HEIGHT) / 2;

                        int cross_width = avg_width * center_dist_y;
                        int cross_height = avg_height * center_dist_x;

                        if (abs_dist_x <= avg_width
                         && abs_dist_y <= avg_height) {
                            if (cross_width > cross_height) {
                                if (cross_width > -cross_height) {
                                    // BOTTOM
                                    ball[i].stepY = absolute(ball[i].stepY);
                                } else {
                                    // LEFT
                                    ball[i].stepX = -absolute(ball[i].stepX);
                                }
                            } else {
                                if(cross_width > -cross_height) {
                                    // RIGHT
                                    ball[i].stepX = absolute(ball[i].stepX);
                                } else {
                                    // TOP
                                    ball[i].stepY = -absolute(ball[i].stepY);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void collisionRacket(void) {
    // Iteration variable
    int i;

    for (i = 0; i < LEN; i++) {
        int top_limit = ball[i].posY + BALL_HEIGHT >= player.posY;
        // 2 pixels of tolerance
        int bottom_limit = ball[i].posY + BALL_HEIGHT <= player.posY + 2;
        int left_limit = ball[i].posX + BALL_WIDTH >= player.posX;
        int right_limit = ball[i].posX <= player.posX + RACKET_WIDTH;

        int right_side = ball[i].posX >= player.posX + RACKET_WIDTH/2;

        if (top_limit && bottom_limit && left_limit && right_limit) {
            ball[i].stepY = -absolute(ball[i].stepY);

            if (right_side) {
                if (ball[i].stepX < 0) {
                    ball[i].stepX = (int)(ball[i].stepX / player.factor);
                    if(_DEBUG) {
                        printf("[  %g  ]\n", ball[i].stepX / player.factor);
                        printf("RIGHT < 0 :: stepX == %d\n", ball[i].stepX);
                    }
                } else if (ball[i].stepX > 0) {
                    ball[i].stepX = (int)(ball[i].stepX * player.factor);
                    if(_DEBUG) {
                        printf("[  %g  ]\n", ball[i].stepX * player.factor);
                        printf("RIGHT > 0 :: stepX == %d\n", ball[i].stepX);
                    }
                } else {
                    ball[i].stepX = (int)(player.factor);
                    if(_DEBUG) {
                        printf("[  %g  ]\n", player.factor);
                        printf("RIGHT = 0 :: stepX == %d\n", ball[i].stepX);
                    }
                }
            } else {
                if (ball[i].stepX < 0) {
                    ball[i].stepX = (int)(ball[i].stepX * player.factor);
                    if(_DEBUG) {
                        printf("[  %g  ]\n", ball[i].stepX * player.factor);
                        printf("LEFT < 0 :: stepX == %d\n", ball[i].stepX);
                    }
                } else if (ball[i].stepX > 0) {
                    ball[i].stepX = (int)(ball[i].stepX / player.factor);
                    if(_DEBUG) {
                        printf("[  %g  ]\n", ball[i].stepX / player.factor);
                        printf("LEFT > 0 :: stepX == %d\n", ball[i].stepX);
                    }
                } else {
                    ball[i].stepX = -(int)(player.factor);
                    if(_DEBUG) {
                        printf("[  %g  ]\n", player.factor);
                        printf("LEFT = 0 :: stepX == %d\n", ball[i].stepX);
                    }
                }
            }

            if (ball[i].stepX < -BALL_MAX_SPEED) {
                ball[i].stepX = -BALL_MAX_SPEED;
            } else if (ball[i].stepX > BALL_MAX_SPEED) {
                ball[i].stepX = BALL_MAX_SPEED;
            }
            if(_DEBUG) printf("[FINAL] stepX = %d\n", ball[i].stepX);
        }
    }
}

void newLevel(void) {
    // Iteration variables
    int i, j, k;

    for (k = 0; k < LEN; k++) {
        player.score += 1000;
        player.aux_score += 1000;

        ball[k].posY = player.posY - BALL_HEIGHT;
        ball[k].posX = player.posX + RACKET_WIDTH / 2 - BALL_WIDTH / 2;
        ball[k].stepX = 0;
        ball[k].stepY = 0;
        ball_in_game = false;

        for (i = 0; i < COLUMNS; i++) {
            for (j = 0; j < LINES; j++) {
                brick[i][j].resist = 1;
            }
        }
        levelClear = COLUMNS * LINES;
        level++;
    }
}

int moveNPC(NPC *p) {
    p->posX += p->stepX/* * BALL_SPEED*/;
    p->posY += p->stepY * BALL_SPEED;

    // If the image is out of bounds on the X axis,
    // invert the direction and reset the position to a valid one
    if (p->posX + BALL_WIDTH > (SCREEN_WIDTH - 200)) {
        p->stepX = -absolute(p->stepX);
        p->posX = (SCREEN_WIDTH - 200) - BALL_WIDTH;
    } else if (p->posX < 0) {
        p->stepX = absolute(p->stepX);
        p->posX = 0;
    }

    // If the image is out of bounds on the Y axis,
    // invert the direction and reset the position to a valid one
    if (p->posY + BALL_HEIGHT > SCREEN_HEIGHT) {
        return 1;
        /*
         * p->stepY = -absolute(p->stepY);
         * p->posY = SCREEN_HEIGHT - BALL_HEIGHT;
         */
    } else if (p->posY < 0) {
        p->stepY = absolute(p->stepY);
        p->posY = 0;
    }

    if(!ball_in_game) p->posX = player.posX + RACKET_WIDTH/2 - BALL_WIDTH/2;
    return 0;
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
                    int score, int aux_score, int lives, float factor) {
    RACKET p;

    p.posX = posX;
    p.posY = posY;
    p.stepX = stepX;
    p.image = image;
    p.score = score;
    p.aux_score = aux_score;
    p.lives = lives;
    p.factor = factor;
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

    /* Open 44.1KHz, signed 16bit, system byte order,
            stereo audio, using 1024 byte chunks     */
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        error(ERR_INIT_AUDIO);
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow("SDL Breakout",
                            200,
                            70,
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

    /* TODO: Fix this
     * main.exe - Entry Point Not Found
     * The procedure entry point InterlockedCompareExchange@12 could not
     * be located in the dynamic link library ...\SDL2_ttf.dll
     *
     * if(TTF_Init() == -1) {
     *     error(ERR_INIT);
     *     return false;
     * }
     */

    // Get window surface
    gScreenSurface = SDL_GetWindowSurface(gWindow);

    return true;
}

int loadMedia(void) {

    // Load PNG surfaces
    if((gBall = loadSurface("./images/circle.png")) == NULL
    || (gBRICKSurface = loadSurface("./images/brick.png")) == NULL
    || (gPLAYERSurface = loadSurface("./images/racket.png")) == NULL
    || (buttonplay = loadSurface("./images/newgamebutton.png")) == NULL
    || (buttonoptions = loadSurface("./images/optionsbutton.png")) == NULL
    || (buttonrankings = loadSurface("./images/rankingbutton.png")) == NULL
    || (side_bar = loadSurface("./images/side_bar.png")) == NULL
    || (breakout = loadSurface("./images/breakout.png")) == NULL) {
        error(ERR_IMG_LOAD);
        return false;
    }

    // Color key
    SDL_SetColorKey(gBall, SDL_TRUE,
                    SDL_MapRGB(gBall->format, 0xff, 0xAE, 0xC9));
    SDL_SetColorKey(gPLAYERSurface, SDL_TRUE,
                    SDL_MapRGB(gPLAYERSurface->format, 0xff, 0xAE, 0xC9));

    // Load audios
    gBrickWAV = Mix_LoadWAV("./sounds/brick.wav");
    if(!gBrickWAV) {
        error(ERR_WAV_LOAD);
        return false;
    }

    return true;
}

void close() {
    // Free loaded image
    SDL_FreeSurface(gBall), SDL_FreeSurface(gBRICKSurface),
    SDL_FreeSurface(gPLAYERSurface), SDL_FreeSurface(buttonplay),
    SDL_FreeSurface(buttonoptions), SDL_FreeSurface(buttonrankings),
    SDL_FreeSurface(side_bar);
    gBall = gBRICKSurface = gPLAYERSurface = NULL;
    buttonplay = buttonoptions = buttonrankings = side_bar = NULL;

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
            printf("Unable to optimize image!");
            break;
        case ERR_IMG_LOAD:
            printf("Unable to load image!");
            break;
        case ERR_INIT_AUDIO:
            printf("Audio could not initialize!");
        case ERR_WAV_LOAD:
            printf("Unable to load WAV!");
        default:
            printf("Unspecified error!");
            break;
    }
    printf(" SDL Error: %s\n", SDL_GetError());
}

int absolute(int n) {
    return n < 0 ? -n : n;
}
