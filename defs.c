#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
/* TODO: #include <SDL2/SDL_ttf.h>*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "globals.h"


/*
 * Constants
 */

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


void menu(void) {

    int buttonnew_x = 59.2*PROP;
    int buttonnew_y = 1.2*PROP;
    int buttonnew_w = 40*PROP;
    int buttonnew_h = 5*PROP;

    int buttonoptions_x = 64.2*PROP;
    int buttonoptions_y = 6.6*PROP;
    int buttonoptions_w = 35*PROP;
    int buttonoptions_h = 5*PROP;

    int buttonrankings_x = 69.2*PROP;
    int buttonrankings_y = 12.1*PROP;
    int buttonrankings_w = 30*PROP;
    int buttonrankings_h = 5*PROP;

    int breakout_x = 0;
    int breakout_y = 0;
    int breakout_w = 100*PROP;
    int breakout_h = 70*PROP;

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

        // Check if mouse is over buttonnew
        if (mouseX >= buttonnew_x
        && mouseX <= buttonnew_x + buttonnew_w
        && mouseY >= buttonnew_y
        && mouseY <= buttonnew_y + buttonnew_h) {

            SDL_SetColorKey(buttonnew, SDL_FALSE,
                            SDL_MapRGB(buttonnew->format, 0x70, 0x92, 0xBE));

            // Check if buttonnew is pressed
            if (SDL_GetMouseState(NULL, NULL)
                && SDL_BUTTON(SDL_BUTTON_LEFT)) {
                gameScreen = 4;
                return;
            }
        } else {
            SDL_SetColorKey(buttonnew, SDL_TRUE,
                            SDL_MapRGB(buttonnew->format, 0x70, 0x92, 0xBE));
        }

        // Check if mouse is over buttonoptions
        if (mouseX >= buttonoptions_x
        && mouseX <= buttonoptions_x + buttonoptions_w
        && mouseY >= buttonoptions_y
        && mouseY <= buttonoptions_y + buttonoptions_h) {

            SDL_SetColorKey(buttonoptions, SDL_FALSE,
                         SDL_MapRGB(buttonoptions->format, 0x70, 0x92, 0xBE));

            // Check if buttonoptions is pressed
            if(SDL_GetMouseState(NULL, NULL)
              && SDL_BUTTON(SDL_BUTTON_LEFT)) {
              gameScreen = 2;
              return;
            }
        } else {
            SDL_SetColorKey(buttonoptions, SDL_TRUE,
                         SDL_MapRGB(buttonoptions->format, 0x70, 0x92, 0xBE));
        }

        // Check if mouse is over buttonrankings
        if (mouseX >= buttonrankings_x
        && mouseX <= buttonrankings_x + buttonrankings_w
        && mouseY >= buttonrankings_y
        && mouseY <= buttonrankings_y + buttonrankings_h) {

            SDL_SetColorKey(buttonrankings, SDL_FALSE,
                        SDL_MapRGB(buttonrankings->format, 0x70, 0x92, 0xBE));

            // Check if buttonrankings is pressed
            if(SDL_GetMouseState(NULL, NULL)
              && SDL_BUTTON(SDL_BUTTON_LEFT)) {
              gameScreen = 3;
              return;
            }
        } else {
            SDL_SetColorKey(buttonrankings, SDL_TRUE,
                        SDL_MapRGB(buttonrankings->format, 0x70, 0x92, 0xBE));
        }

        // Fill the surface with #000000 (black)
        SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format,
                                                    0x00, 0x00, 0x00));

        if(drawOnScreen(breakout, 0, 0,
                         breakout_w, breakout_h,
                         breakout_x, breakout_y) < 0
        || drawOnScreen(buttonoptions, 0, 0,
                         buttonoptions_w, buttonoptions_h,
                         buttonoptions_x, buttonoptions_y) < 0
        || drawOnScreen(buttonrankings, 0, 0,
                         buttonrankings_w, buttonrankings_h,
                         buttonrankings_x, buttonrankings_y) < 0
        || drawOnScreen(buttonnew, 0, 0,
                        buttonnew_w, buttonnew_h,
                        buttonnew_x, buttonnew_y) < 0){
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

    // Mouse position
    int mouseX, mouseY;

    // Event handler
    SDL_Event e;

    ball_in_game = false;

    // Create BRICKS
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
    player = createRACKET (RACKET_WIDTH * l + z,  // int posX
                              RACKET_HEIGHT * m,  // int posY
                                   (BALL_SPEED),  // int stepX
                                 gPLAYERSurface,  // SDL_Surface *image
                                              0,  // int score
                                              0,  // int aux_score
                                              3,  // int lives
                                           2.2);  // float factor
    player._left = false;
    player._right = false;
    gPause = false;
    gSound = true;
    gMusic = true;

    // Create NPC
    int _posX = player.posX + RACKET_WIDTH/2 - BALL_WIDTH/2;
    int _posY = player.posY - BALL_HEIGHT;

    for (i= 0; i < LEN; i++) {
        ball[i] = createNPC(_posX, _posY, 0, 0, gBall);
    }

    // While application is running
    while (!quit) {
        // Get current mouse position
        SDL_GetMouseState(&mouseX, &mouseY);

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
                            float r = rand() % 4;
                            ball[i].stepX = !r ? 1 : r;
                            if(rand() % 2) ball[i].stepX *= -1;

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

                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        if (mouseX >= ((SCREEN_WIDTH - 200) + 40)
                        && mouseX <= ((SCREEN_WIDTH - 200) + 40) + 50
                        && mouseY >= 510
                        && mouseY <= 510 + 50) {
                            gSound = !gSound;
                        } else if (mouseX >= ((SCREEN_WIDTH - 200) + 120)
                        && mouseX <= ((SCREEN_WIDTH - 200) + 120) + 50
                        && mouseY >= 510
                        && mouseY <= 510 + 50)
                            gMusic = !gMusic;
                            if(!gMusic) Mix_VolumeMusic(0);
                            else Mix_VolumeMusic(MIX_MAX_VOLUME);
                    }

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
                            can_music_play = 1;
                            player.score = 0;
                            player.aux_score = 0;
                            player.lives = 3;
                            player._left = false;
                            player._right = false;
                            menu();
                            return;
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
                blockX = j % 2 ? BLOCK_WIDTH : 0;
                blockY = j % 2 ? 0 : BLOCK_HEIGHT;
                if(j == 4) blockY *= 2;

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

        // Draw sound icons
        if(drawOnScreen(sound, gSound ? 0 : 50, 0,
                        50, 50,
                        (SCREEN_WIDTH - 200) + 40, 510) < 0
        || drawOnScreen(sound, gMusic ? 0 : 50, 50,
                        50, 50,
                        (SCREEN_WIDTH - 200) + 120, 510) < 0) {
            error(ERR_BLIT);
            quit = true;
        }

        // Play music
        if(player.lives > 0 && can_music_play) {
            if(Mix_PlayMusic(gMusicWAV, -1)==-1) {
                printf("Failed to load music!\n%s\n", Mix_GetError());
                // There is no music, but the game is still playable
            }
            can_music_play = 0;
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

void options(void) {
    int buttonhome_x = 5*PROP;
    int buttonhome_y = 5*PROP;
    int buttonhome_w = 11.2*PROP;
    int buttonhome_h = 5*PROP;

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

        // Check if mouse is over button home
        if (mouseX >= buttonhome_x
        && mouseX <= buttonhome_x + buttonhome_w
        && mouseY >= buttonhome_y
        && mouseY <= buttonhome_y + buttonhome_h) {
            SDL_SetColorKey(buttonhome, SDL_FALSE,
                            SDL_MapRGB(buttonhome->format, 0x70, 0x92, 0xBE));

            // Check if button home is pressed
            if (SDL_GetMouseState(NULL, NULL)
                && SDL_BUTTON(SDL_BUTTON_LEFT)) {
                gameScreen = 0;
                return;
            }
        } else {
            SDL_SetColorKey(buttonhome, SDL_TRUE,
                            SDL_MapRGB(buttonhome->format, 0x70, 0x92, 0xBE));
        }

        // Fill the surface with #000000 (black)
        SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format,
                                                      0x00, 0x00, 0x00));

        if(drawOnScreen(buttonhome, 0, 0,
              buttonhome_w, buttonhome_h,
              buttonhome_x, buttonhome_y) < 0){
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

void ranking(void) {
    /* TODO: struct all the things */
    int buttonhome_x = 5*PROP;
    int buttonhome_y = 5*PROP;
    int buttonhome_w = 11.2*PROP;
    int buttonhome_h = 5*PROP;

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

        // Check if mouse is over button home
        int is_hovering = (mouseX >= buttonhome_x
                        && mouseX <= buttonhome_x + buttonhome_w
                        && mouseY >= buttonhome_y
                        && mouseY <= buttonhome_y + buttonhome_h);

        SDL_SetColorKey(buttonhome, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonhome->format, 0x70, 0x92, 0xBE));

        // Check if button home is pressed
        if(is_hovering
        && SDL_GetMouseState(NULL, NULL)
        && SDL_BUTTON(SDL_BUTTON_LEFT)) {
            gameScreen = 0;
            return;
        }

        // Fill the surface with #000000 (black)
        SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format,
                                                      0x00, 0x00, 0x00));

        if(drawOnScreen(buttonhome, 0, 0,
          buttonhome_w, buttonhome_h,
          buttonhome_x, buttonhome_y) < 0) {
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

void configuration(void) {
    /* TODO: struct all the things */
    int buttonhome_x = 5*PROP;
    int buttonhome_y = 5*PROP;
    int buttonhome_w = 11.2*PROP;
    int buttonhome_h = 5*PROP;

    int buttonplay_x = 5*PROP;
    int buttonplay_y = 60*PROP;
    int buttonplay_w = 11.2*PROP;
    int buttonplay_h = 5*PROP;

    int gamemode_x = 5*PROP;
    int gamemode_y = 25*PROP;
    int gamemode_w = 35*PROP;
    int gamemode_h = 5*PROP;

    int physics_x = 5*PROP;
    int physics_y = 40*PROP;
    int physics_w = 35*PROP;
    int physics_h = 5*PROP;

    int config_x = 0;
    int config_y = 0;
    int config_w = 100*PROP;
    int config_h = 70*PROP;

    // Mouse position
    int mouseX, mouseY;

    int is_hovering;

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
                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button == SDL_BUTTON_LEFT
                    && mouseX >= gamemode_x
                    && mouseX <= gamemode_x + gamemode_w
                    && mouseY >= gamemode_y
                    && mouseY <= gamemode_y + gamemode_h) {
                        gGameMode = gGameMode == 1 ? 2 : 1;
                    }
                    if(e.button.button == SDL_BUTTON_LEFT
                    && mouseX >= physics_x
                    && mouseX <= physics_x + physics_w
                    && mouseY >= physics_y
                    && mouseY <= physics_y + physics_h) {
                        gPhysics = gPhysics == 1 ? 2 : 1;
                    }
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }

        // Check if mouse is over button home
        is_hovering = (mouseX >= buttonhome_x
                    && mouseX <= buttonhome_x + buttonhome_w
                    && mouseY >= buttonhome_y
                    && mouseY <= buttonhome_y + buttonhome_h);

        SDL_SetColorKey(buttonhome, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonhome->format, 0x70, 0x92, 0xBE));

        // Check if button home is pressed
        if(is_hovering
        && SDL_GetMouseState(NULL, NULL)
        && SDL_BUTTON(SDL_BUTTON_LEFT)) {
            gameScreen = 0;
            return;
        }

        // Check if mouse is over button play
        is_hovering = (mouseX >= buttonplay_x
                    && mouseX <= buttonplay_x + buttonplay_w
                    && mouseY >= buttonplay_y
                    && mouseY <= buttonplay_y + buttonplay_h);

        SDL_SetColorKey(buttonplay, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonplay->format, 0x70, 0x92, 0xBE));

        // Check if button home is pressed
        if(is_hovering
        && SDL_GetMouseState(NULL, NULL)
        && SDL_BUTTON(SDL_BUTTON_LEFT)) {
            gameScreen = 1;
            return;
        }

        // Check if mouse is over button campaign/endless
        is_hovering = (mouseX >= gamemode_x
                    && mouseX <= gamemode_x + gamemode_w
                    && mouseY >= gamemode_y
                    && mouseY <= gamemode_y + gamemode_h);

        SDL_SetColorKey(buttoncampaign, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttoncampaign->format, 0x70, 0x92, 0xBE));

        SDL_SetColorKey(buttonendless, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonendless->format, 0x70,
                                                          0x92,
                                                          0xBE));

        // Check if mouse is over button campaign/endless
        is_hovering = (mouseX >= physics_x
                    && mouseX <= physics_x + physics_w
                    && mouseY >= physics_y
                    && mouseY <= physics_y + physics_h);

        SDL_SetColorKey(buttonclassic, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonclassic->format, 0x70, 0x92, 0xBE));
        SDL_SetColorKey(buttonalternate, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonalternate->format, 0x70, 0x92,0xBE));

        // Fill the surface with #000000 (black)
        SDL_FillRect(gScreenSurface, NULL,
                        SDL_MapRGB(gScreenSurface->format, 0x00, 0x00, 0x00));

        if(drawOnScreen(config, 0, 0,
                        config_w, config_h,
                        config_x, config_y) < 0
        || drawOnScreen(buttonhome, 0, 0,
                        buttonhome_w, buttonhome_h,
                        buttonhome_x, buttonhome_y) < 0
        || drawOnScreen(buttonplay, 0, 0,
                        buttonplay_w, buttonplay_h,
                        buttonplay_x, buttonplay_y) < 0
        || drawOnScreen(gGameMode == 1 ? buttoncampaign : buttonendless, 0, 0,
                        gamemode_w, gamemode_h,
                        gamemode_x, gamemode_y) < 0
        || drawOnScreen(gPhysics == 1 ? buttonclassic : buttonalternate, 0, 0,
                        physics_w, physics_h,
                        physics_x, physics_y) < 0) {
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
                        if(!brick[j][k].resist && gSound) {
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
    if (gPhysics == 1) {
        // Iteration variable
        int i;

        for (i = 0; i < LEN; i++) {
            int top_limit = ball[i].posY + BALL_HEIGHT >= player.posY;
            // 2 pixels of tolerance
            int bottom_limit = ball[i].posY + BALL_HEIGHT <= player.posY + 2;
            int left_limit = ball[i].posX + BALL_WIDTH >= player.posX;
            int right_limit = ball[i].posX <= player.posX + RACKET_WIDTH;
            //int right_side = ball[i].posX >= player.posX + RACKET_WIDTH/2;

            if (top_limit && bottom_limit && left_limit && right_limit) {
                ball[i].stepY = -absolute(ball[i].stepY);

                float left_edge = player.posX;
                float right_edge = left_edge + RACKET_WIDTH;

                float step_max = BALL_MAX_SPEED - 1;
                int POS = ball[i].posX + BALL_WIDTH/2;

                ball[i].stepX = (2*POS - left_edge - right_edge) * step_max;
                ball[i].stepX /= (float)RACKET_WIDTH;

                if(_DEBUG) {
                    printf("[  %f  ]\n", ball[i].stepX / player.factor);
                    printf("stepX == %f\n", ball[i].stepX);
                }

                if(gSound) Mix_PlayChannel(-1, gRacketWAV, 0);

                if(ball[i].stepX < -BALL_MAX_SPEED) {
                    ball[i].stepX = -BALL_MAX_SPEED;
                } else if (ball[i].stepX > BALL_MAX_SPEED) {
                    ball[i].stepX = BALL_MAX_SPEED;
                }

                if(_DEBUG) printf("[FINAL] stepX = %d\n", ball[i].stepX);
            }
        }
    } else if(gPhysics == 2) {
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

                if(ball[i].stepX < -BALL_MAX_SPEED) {
                    ball[i].stepX = -BALL_MAX_SPEED;
                } else if (ball[i].stepX > BALL_MAX_SPEED) {
                    ball[i].stepX = BALL_MAX_SPEED;
                }
                if(_DEBUG) printf("[FINAL] stepX = %d\n", ball[i].stepX);
                if(gSound) Mix_PlayChannel(-1, gBrickWAV, 0);
            }
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

    Uint16 pixels[16*16] = {  // ...or with raw pixel data:
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0aab, 0x0789, 0x0bcc, 0x0eee, 0x09aa, 0x099a, 0x0ddd,
        0x0fff, 0x0eee, 0x0899, 0x0fff, 0x0fff, 0x1fff, 0x0dde, 0x0dee,
        0x0fff, 0xabbc, 0xf779, 0x8cdd, 0x3fff, 0x9bbc, 0xaaab, 0x6fff,
        0x0fff, 0x3fff, 0xbaab, 0x0fff, 0x0fff, 0x6689, 0x6fff, 0x0dee,
        0xe678, 0xf134, 0x8abb, 0xf235, 0xf678, 0xf013, 0xf568, 0xf001,
        0xd889, 0x7abc, 0xf001, 0x0fff, 0x0fff, 0x0bcc, 0x9124, 0x5fff,
        0xf124, 0xf356, 0x3eee, 0x0fff, 0x7bbc, 0xf124, 0x0789, 0x2fff,
        0xf002, 0xd789, 0xf024, 0x0fff, 0x0fff, 0x0002, 0x0134, 0xd79a,
        0x1fff, 0xf023, 0xf000, 0xf124, 0xc99a, 0xf024, 0x0567, 0x0fff,
        0xf002, 0xe678, 0xf013, 0x0fff, 0x0ddd, 0x0fff, 0x0fff, 0xb689,
        0x8abb, 0x0fff, 0x0fff, 0xf001, 0xf235, 0xf013, 0x0fff, 0xd789,
        0xf002, 0x9899, 0xf001, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0xe789,
        0xf023, 0xf000, 0xf001, 0xe456, 0x8bcc, 0xf013, 0xf002, 0xf012,
        0x1767, 0x5aaa, 0xf013, 0xf001, 0xf000, 0x0fff, 0x7fff, 0xf124,
        0x0fff, 0x089a, 0x0578, 0x0fff, 0x089a, 0x0013, 0x0245, 0x0eff,
        0x0223, 0x0dde, 0x0135, 0x0789, 0x0ddd, 0xbbbc, 0xf346, 0x0467,
        0x0fff, 0x4eee, 0x3ddd, 0x0edd, 0x0dee, 0x0fff, 0x0fff, 0x0dee,
        0x0def, 0x08ab, 0x0fff, 0x7fff, 0xfabc, 0xf356, 0x0457, 0x0467,
        0x0fff, 0x0bcd, 0x4bde, 0x9bcc, 0x8dee, 0x8eff, 0x8fff, 0x9fff,
        0xadee, 0xeccd, 0xf689, 0xc357, 0x2356, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0ccd, 0x0bdd, 0x0cdd, 0x0aaa, 0x2234, 0x4135, 0x4346,
        0x5356, 0x2246, 0x0346, 0x0356, 0x0467, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff
    };
    gIcon = SDL_CreateRGBSurfaceFrom(pixels,
                                    16,16,
                                    16,16*2,
                                    0x0f00,0x00f0,
                                    0x000f,0xf000);

    // The icon is attached to the window pointer
    SDL_SetWindowIcon(gWindow, gIcon);

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
    || (buttonnew = loadSurface("./images/newgamebutton.png")) == NULL
    || (buttonoptions = loadSurface("./images/optionsbutton.png")) == NULL
    || (buttonrankings = loadSurface("./images/rankingbutton.png")) == NULL
    || (side_bar = loadSurface("./images/side_bar.png")) == NULL
    || (breakout = loadSurface("./images/homeback.png")) == NULL
    || (config = loadSurface("./images/configback.png")) == NULL
    || (sound = loadSurface("./images/sound.png")) == NULL
    || (buttonhome = loadSurface("./images/homebutton.png")) == NULL
    || (buttonendless = loadSurface("./images/endless.png")) == NULL
    || (buttoncampaign = loadSurface("./images/campaign.png")) == NULL
    || (buttonclassic = loadSurface("./images/classic.png")) == NULL
    || (buttonalternate = loadSurface("./images/alternate.png")) == NULL
    || (buttonplay = loadSurface("./images/playbutton.png")) == NULL) {
        error(ERR_IMG_LOAD);
        return false;
    }

    // Color key
    SDL_SetColorKey(gBall, SDL_TRUE,
                    SDL_MapRGB(gBall->format, 0xff, 0xAE, 0xC9));
    SDL_SetColorKey(gPLAYERSurface, SDL_TRUE,
                    SDL_MapRGB(gPLAYERSurface->format, 0xff, 0xAE, 0xC9));
    SDL_SetColorKey(sound, SDL_TRUE,
                    SDL_MapRGB(sound->format, 0xff, 0xAE, 0xC9));

    // Load audios
    gBrickWAV = Mix_LoadWAV("./sounds/brick.wav");
    if(!gBrickWAV) {
        error(ERR_WAV_LOAD);
        return false;
    }
    gRacketWAV = Mix_LoadWAV("./sounds/racket.wav");
    if(!gRacketWAV) {
        error(ERR_WAV_LOAD);
        return false;
    }

    gMusicWAV = Mix_LoadMUS("./sounds/music.mp3");
    if(!gMusicWAV) {
        error(ERR_MP3_LOAD);
        return false;
    }

    return true;
}

void closef() {
    // Free loaded image
    SDL_FreeSurface(gBall), SDL_FreeSurface(gBRICKSurface),
    SDL_FreeSurface(gPLAYERSurface), SDL_FreeSurface(buttonnew),
    SDL_FreeSurface(buttonoptions), SDL_FreeSurface(buttonrankings),
    SDL_FreeSurface(side_bar);
    gBall = gBRICKSurface = gPLAYERSurface = NULL;
    buttonnew = buttonoptions = buttonrankings = side_bar = NULL;

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
        case ERR_MP3_LOAD:
            printf("Unable to load MP3!");
        default:
            printf("Unspecified error!");
            break;
    }
    printf(" SDL Error: %s\n", SDL_GetError());
}

int absolute(int n) {
    return n < 0 ? -n : n;
}
