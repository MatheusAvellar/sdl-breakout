#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
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

int RACKET_WIDTH = 13 * PROP;
int RACKET_HEIGHT = 2 * PROP;

/* Fixed framerate
 * ---------------
 * Formula: 1000 / (desired fps)
 * TICK_INTERVAL = 16 (~62 fps)
 * TICK_INTERVAL = 17 (~58 fps)
 * TICK_INTERVAL = 33 (~30 fps)
 */
const unsigned int TICK_INTERVAL = 17;

// Speed multiplier
const int BALL_SPEED = 9; //MUDAR AQUI
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
                    // If user pressed the 'ESCAPE' key
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // If the user has clicked with the left mouse button
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        // If the mouse is over the 'New Game' button
                        if(mouseX >= buttonnew_x
                        && mouseX <= buttonnew_x + buttonnew_w
                        && mouseY >= buttonnew_y
                        && mouseY <= buttonnew_y + buttonnew_h) {
                            gameScreen = 4;
                            return;
                        }
                        // If the mouse is over the 'Options' button
                        else if(mouseX >= buttonoptions_x
                        && mouseX <= buttonoptions_x + buttonoptions_w
                        && mouseY >= buttonoptions_y
                        && mouseY <= buttonoptions_y + buttonoptions_h) {
                            gameScreen = 2;
                            return;
                        }
                        // If the mouse is over the 'Ranking' button
                        else if(mouseX >= buttonrankings_x
                        && mouseX <= buttonrankings_x + buttonrankings_w
                        && mouseY >= buttonrankings_y
                        && mouseY <= buttonrankings_y + buttonrankings_h) {
                            gameScreen = 3;
                            return;
                        }
                    }
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }

        // Fill the surface with #99d9ea (light blue)
        SDL_FillRect(gScreenSurface, NULL,
                    SDL_MapRGB(gScreenSurface->format, 0x99, 0xD9, 0xEA));


        // Check if mouse is over buttonnew
        int is_hovering = (mouseX >= buttonnew_x
                        && mouseX <= buttonnew_x + buttonnew_w
                        && mouseY >= buttonnew_y
                        && mouseY <= buttonnew_y + buttonnew_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonnew, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonnew->format, 0x70, 0x92, 0xBE));


        // Check if mouse is over buttonoptions
        is_hovering = (mouseX >= buttonoptions_x
                    && mouseX <= buttonoptions_x + buttonoptions_w
                    && mouseY >= buttonoptions_y
                    && mouseY <= buttonoptions_y + buttonoptions_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonoptions, is_hovering ? SDL_FALSE : SDL_TRUE,
                     SDL_MapRGB(buttonoptions->format, 0x70, 0x92, 0xBE));


        // Check if mouse is over buttonrankings
        is_hovering = (mouseX >= buttonrankings_x
                    && mouseX <= buttonrankings_x + buttonrankings_w
                    && mouseY >= buttonrankings_y
                    && mouseY <= buttonrankings_y + buttonrankings_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonrankings, is_hovering ? SDL_FALSE : SDL_TRUE,
                    SDL_MapRGB(buttonrankings->format, 0x70, 0x92, 0xBE));

        // If there's an error loading any of the buttons
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
                        buttonnew_x, buttonnew_y) < 0) {
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
    // Iteration and control variables
    int i, j, l, m, z,
        _temp_score = 0;

    // Mouse position
    int mouseX, mouseY;

    // Event handler
    SDL_Event e;

    // Time for bonus
    int time_y_bonus;
    int time_max_bonus = (1.2*((LINES*COLUMNS)+(LINES+COLUMNS)))-4;
    int time_min_bonus = 1.5*time_max_bonus;

    // Quit button dimensions
    int buttonquit_x = 36*PROP;
    int buttonquit_y = 31*PROP;
    int buttonquit_w = 11.2*PROP;
    int buttonquit_h = 5*PROP;

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
    player = createRACKET(RACKET_WIDTH * l + z,  // int posX
                             RACKET_HEIGHT * m,  // int posY
                                    BALL_SPEED,  // int stepX
                                gPLAYERSurface,  // SDL_Surface *image
                                             0,  // int score
                                             0,  // int aux_score
                                             3,  // int lives
                                           2.2); // float factor

    // Initialize 'pressing key state' to false
    player._left = false;
    player._right = false;

    // Initialize pause mode to false
    gPause = false;
    // Initialize music and sound to default, true
    gSound = true;
    gMusic = true;

    // Initialize ball positions
    int _posX = player.posX + RACKET_WIDTH/2 - BALL_WIDTH/2;
    int _posY = player.posY - BALL_HEIGHT;

    // 'for' in case of multiball (not implemented yet)
    for (i= 0; i < LEN; i++) {
        ball[i] = createNPC(_posX, _posY, 0, 0, gBall);
    }

    // Power up
    int anim_frame = 0;
    int anim_time = 0;

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
                    // If user pressed the 'ESCAPE' key
                    if(e.key.keysym.sym == SDLK_ESCAPE) quit = true;

                    // If 'control inverter' powerdown is inactive
                    if (!controlInverter) {
                        // Player is pressing left, go left
                        player._left = player._left
                                      || e.key.keysym.sym == SDLK_LEFT
                                      || e.key.keysym.sym == SDLK_a;

                        // Player is pressing right, go right
                        player._right = player._right
                                      || e.key.keysym.sym == SDLK_RIGHT
                                      || e.key.keysym.sym == SDLK_d;
                    }
                    // If 'control inverter' powerdown is active
                    else {
                        // Player is pressing left, go right
                        player._left = player._left
                                      || e.key.keysym.sym == SDLK_RIGHT
                                      || e.key.keysym.sym == SDLK_d;

                        // Player is pressing right, go left
                        player._right = player._right
                                      || e.key.keysym.sym == SDLK_LEFT
                                      || e.key.keysym.sym == SDLK_a;
                    }

                    // If user presses space, the ball is not already in game
                    // and the game is not paused
                    if(e.key.keysym.sym == SDLK_SPACE
                    && !ball_in_game && !gPause) {
                        // For every ball (multiball, not implemented yet)
                        for (i = 0; i < LEN; i++) {
                            // Pick a random direction from 0 to 4
                            float dir = rand() % 4;

                            // If the direction is 0, set it to 1
                            ball[i].stepX = !(int)dir ? 1 : dir;

                            // Flip a coin, if it's 1, then invert ball's
                            // direction. Otherwise, leave it as is
                            if(rand() % 2) ball[i].stepX *= -1;

                            // Ball is moving upwards at first
                            ball[i].stepY = -1;

                            // Ball is now in game
                            ball_in_game = true;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    // If 'control inverter' powerdown is inactive
                    if(!controlInverter) {
                        // Player unpressed left, stop going left
                        if(e.key.keysym.sym == SDLK_a
                        || e.key.keysym.sym == SDLK_LEFT) {
                            player._left = false;
                        }

                        // Player unpressed right, stop going right
                        if (e.key.keysym.sym == SDLK_d
                        || e.key.keysym.sym == SDLK_RIGHT) {
                            player._right = false;
                        }
                    }
                    // If 'control inverter' powerdown is active
                    else {
                        // Player unpressed left, stop going right
                        if(e.key.keysym.sym == SDLK_a
                        || e.key.keysym.sym == SDLK_LEFT) {
                            player._right = false;
                        }

                        // Player unpressed right, stop going left
                        if (e.key.keysym.sym == SDLK_d
                        || e.key.keysym.sym == SDLK_RIGHT) {
                            player._left = false;
                        }
                    }

                    // Player unpressed P, invert pause state
                    if (e.key.keysym.sym == SDLK_p) gPause = !gPause;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // If the user has clicked with the left mouse button
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        // If mouse is over sound button
                        if(mouseX >= ((SCREEN_WIDTH - 200) + 80)
                        && mouseX <= ((SCREEN_WIDTH - 200) + 80) + 50
                        && mouseY >= 440
                        && mouseY <= 440 + 50) {
                            // Invert sound state
                            gSound = !gSound;
                        }
                        // If mouse is over music button
                        else if(mouseX >= ((SCREEN_WIDTH - 200) + 80)
                        && mouseX <= ((SCREEN_WIDTH - 200) + 80) + 50
                        && mouseY >= 510
                        && mouseY <= 510 + 50) {
                            // Invert music state, if it becomes false
                            if(!(gMusic = !gMusic)) {
                                // Mute music
                                Mix_VolumeMusic(0);
                            }
                            // Otherwise, if it becomes true
                            else {
                                // Unmute music
                                Mix_VolumeMusic(MIX_MAX_VOLUME);
                            }
                        }
                        // If mouse is over pause button
                        else if (mouseX >= ((SCREEN_WIDTH - 200) + 80)
                        && mouseX <= ((SCREEN_WIDTH - 200) + 80) + 50
                        && mouseY >= 580
                        && mouseY <= 580 + 50) {
                            // Invert pause state
                            gPause = !gPause;
                        }
                        // If mouse is over quit button
                        else if (gPause
                        && mouseX >= buttonquit_x
                        && mouseX <= buttonquit_x + buttonquit_w
                        && mouseY >= buttonquit_y
                        && mouseY <= buttonquit_y + buttonquit_h) {
                            // Start a new level
                            newLevel();
                            // Reset variables
                            player.score = 0;
                            player.aux_score = 0;
                            player.lives = 3;
                            level = 0;
                            blocklevel = 0;
                            blockscore = 0;
                            blocklives = 0;
                            blockbonus = 0;
                            player._left = false;
                            player._right = false;
                            gameScreen = 0;
                            return;
                        }
                    }
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }

        // Fill the surface with #99d9ea (light blue)
        SDL_FillRect(gScreenSurface, NULL,
                    SDL_MapRGB(gScreenSurface->format, 0x99, 0xD9, 0xEA));

        // If game is not paused
        if(!gPause) {
            // If player should be going left (pressing left or pressing right
            // with 'control inverter' powerdown active)
            if(player._left) {
                // Horizontal speed becomes the absolute value of the
                // horizontal speed, times -1
                player.stepX = -absolute(player.stepX);

                // If position is greater than 0, then position is itself plus
                // horizontal speed. Otherwise, something is wrong, so reset
                // the position to 0
                player.posX = player.posX > 0
                            ? player.posX + player.stepX
                            : 0;
            }
            // If player should be going right (pressing right or pressing
            // left with 'control inverter' powerdown active)
            else if(player._right) {
                // Horizontal speed becomes the absolute value of the
                // horizontal speed
                player.stepX = absolute(player.stepX);

                // If position is lesser than the screen width minus the left
                // border and minus the paddle width, then position is itself
                // plus horizontal speed. Otherwise, something is wrong, so
                // reset the position to the screen width minus the left
                // border and minus the paddde width
                player.posX = player.posX < SCREEN_WIDTH - 200 - RACKET_WIDTH
                            ? player.posX + player.stepX
                            : SCREEN_WIDTH - 200 - RACKET_WIDTH;
            }
        }

        // For every ball (multiball, not implemented yet)
        for (i= 0; i < LEN; i++) {
            // If there are still blocks to be broken
            if (levelClear) {
                // If game is not paused and ball hit bottom of screen
                // (moveNPC returns 1 when ball is lost)
                if(!gPause && moveNPC(&ball[i])) {

                    // Decrease from player life counter
                    if(!_GODMODE) player.lives -= 1;

                    // Reset the position of the ball to the top of the paddle
                    ball[i].posY = player.posY - BALL_HEIGHT;
                    ball[i].posX = player.posX + RACKET_WIDTH/2 - BALL_WIDTH/2;
                    // Reset ball's speed to 0
                    ball[i].stepX = 0;
                    ball[i].stepY = 0;

                    // Ball is no longer in game
                    ball_in_game = false;

                    // If player is out of lives
                    if(player.lives <= 0) {
                        if(_DEBUG) {
                            printf("[Player is out of lives!] %d\n",
                                    player.lives);
                        }
                        // Reset variables
                        blocklevel = 0;
                        blockscore = 0;
                        blocklives = 0;
                        blockbonus = 0;
                        player._left = false;
                        player._right = false;

                        if((unsigned)player.score > gRankedVector[4].score) {
                            if(_DEBUG) printf("[RANKED] New record!\n");
                            gRankedVector[4].score = player.score;
                            // Game screen is now 'name input'
                            gameScreen = 6;
                        } else {
                            // Game screen is now 'game over'
                            gameScreen = 5;
                        }
                        return;
                    }
                }

                // If there's an error loading the ball images
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

        // If there are no more blocks to be broken
        if (!levelClear) {
            // Start a new level
            newLevel();
        }

        // Draw bricks
        int blockX, blockY;

        // For every block
        for (i = 0; i < COLUMNS; i++) {
            for (j = 0; j < LINES; j++) {

                // If gamemode is 'endless'
                if(gGameMode == 2) {
                    // Colorful blocks
                    blockX = j % 2 ? BLOCK_WIDTH : 0;
                    blockY = j == 1 || j == 2 ? 0 : BLOCK_HEIGHT;
                    if(j == 4) blockY *= 2;
                }
                // If gamemode is 'campaign'
                else if (gGameMode == 1) {
                    // Colors reflect each block's resistance
                    switch (brick[i][j].resist) {
                        case 1:
                            blockX = 0;
                            blockY = 0;
                            break;
                        case 2:
                            blockX = BLOCK_WIDTH;
                            blockY = 0;
                            break;
                        case 3:
                            blockX = 0;
                            blockY = BLOCK_HEIGHT;
                            break;
                        case 4:
                            blockX = BLOCK_WIDTH;
                            blockY = BLOCK_HEIGHT;
                            break;
                        case 5:
                            blockX = 0;
                            blockY = 2*BLOCK_HEIGHT;
                            break;
                        default:
                            blockX = BLOCK_WIDTH;
                            blockY = 2*BLOCK_HEIGHT;
                            break;
                    }
                }

                if(brick[i][j].resist
                && drawOnScreen(brick[i][j].image,
                                blockX, blockY,
                                BLOCK_WIDTH, BLOCK_HEIGHT,
                                brick[i][j].posX, brick[i][j].posY) < 0) {
                    error(ERR_BLIT);
                    quit = true;
                }
            }
        }

        // If 'small racket' powerdown is active
        if (smallracket) {
            // Draw small paddle
            player.image = gPLAYERSMALLSurface;
            RACKET_WIDTH = 10.4*PROP;
        }
        // If 'big racket' powerup is active
        else if (bigracket) {
            // Draw big paddle
            player.image = gPLAYERLARGESurface;
            RACKET_WIDTH = 15.6*PROP;
        }
        // If neither 'small' nor 'bit racket' buffers are active
        else {
            // Draw regular paddle
            player.image = gPLAYERSurface;
            RACKET_WIDTH = 13*PROP;
        }

        // If there's an error loading the paddle
        if(drawOnScreen(player.image, 0, 0,
                        RACKET_WIDTH, RACKET_HEIGHT,
                        player.posX, player.posY) < 0) {
            error(ERR_BLIT);
            quit = true;
        }


        // If there's an error loading the side bar
        if(drawOnScreen(side_bar, 0, 0,
                        200, 700,
                        SCREEN_WIDTH - 200, 0) < 0) {
            error(ERR_BLIT);
            quit = true;
        }

        // If there's an error loading any of the side bar icons
        if(drawOnScreen(sound, gSound ? 0 : 50, 0,
                        50, 50,
                        (SCREEN_WIDTH - 200) + 80, 440) < 0
        || drawOnScreen(sound, gMusic ? 0 : 50, 50,
                        50, 50,
                        (SCREEN_WIDTH - 200) + 80, 510) < 0
        || drawOnScreen(sound, gPause ? 0 : 50, 100,
                        50, 50,
                        (SCREEN_WIDTH - 200) + 80, 580) < 0) {
            error(ERR_BLIT);
            quit = true;
        }

        // If there's a powerup/down on screen
        if(gPowerUp) {
            // Increase animation counter
            anim_time++;

            // If 5 game frames have passed
            if(anim_time > 5) {
                // Change animation frame
                anim_frame++;
                // Reset animation counter
                anim_time = 0;
            }

            // If game isn't paused
            if(!gPause) {
                // Lower the powerup's position
                powerup_y += 3;
            }

            // If animation frame is greater than 11
            if(anim_frame > 11) {
                // Reset animation
                anim_frame = 0;
            }

            // If there's an error loading the powerup
            if(drawOnScreen(power_up, 42*anim_frame, 0,
                            42, 42,
                            powerup_x, powerup_y) < 0) {
                error(ERR_BLIT);
                quit = true;
            }

            // If powerup is no longer on screen
            if(powerup_y > SCREEN_HEIGHT) gPowerUp = 0;
        }

        // Current time minus the time the level started
        time_y_bonus = time(0) - gTime;
        bonus = ((time_min_bonus - time_y_bonus) * 1200) / time_max_bonus;
        bonus = bonus > 600 ? 600 : bonus < 0 ? 0 : bonus;

        // If there is more than one ball, check collision between balls
        if(LEN > 1) collisionBalls();

        // Collision between ball and brick
        collisionBrick();

        // Collision between ball and racket
        if(ball_in_game) collisionRacket();

        // Collision between ball and power up
        if (gPowerUp) collisionPowerUp();

        // For testing purposes only
        if(_DEBUG && player.score != _temp_score) {
            printf("[Score: %d]\n", player.score);
            // player.aux_score = player.score;
        }
        // If temp_score is outdated
        if(player.score > _temp_score || player.score == 0) {
            _temp_score = player.score;
            // player.aux_score = player.score;
        }
        if(player.aux_score >= 10000) {
            player.lives += 1;
            player.aux_score -= 10000;
        }

        // If game is paused
        if(gPause) {
            // Check if mouse is over 'quit' button
            if(mouseX >= buttonquit_x
            && mouseX <= buttonquit_x + buttonquit_w
            && mouseY >= buttonquit_y
            && mouseY <= buttonquit_y + buttonquit_h) {
                SDL_SetColorKey(buttonquit, SDL_FALSE,
                            SDL_MapRGB(buttonquit->format, 0x70, 0x92, 0xBE));
            } else {
                SDL_SetColorKey(buttonquit, SDL_TRUE,
                            SDL_MapRGB(buttonquit->format, 0x70, 0x92, 0xBE));
            }

            // If there's an error loading the pause screen or 'quit' button
            if(drawOnScreen(pause, 0, 0,
                            28.8*PROP, 14.2*PROP,
                            30*PROP, 24*PROP) < 0
            || drawOnScreen(buttonquit, 0, 0,
                            buttonquit_w, buttonquit_h,
                            buttonquit_x, buttonquit_y) < 0) {
                error(ERR_BLIT);
                quit = true;
            }
        }

        // R: 0, G: 0, B: 0, alpha: 255
        SDL_Color black = { 0, 0, 0, 255 };

        char strbonus[4];
        // Only update bonus text if it has changed
        if(bonus != contabonus || !blockbonus) {
            sprintf(strbonus, "%d", bonus);
            contabonus = bonus;
            blockbonus = 1;
            if(_DEBUG) printf("TTF bonus\n");
        }
        drawTextOnScreen(strbonus, SCREEN_WIDTH-160, 340, black);

        char strlevel[4];
        // Only update level text if it has changed
        if(level != contalevel || !blocklevel) {
            sprintf(strlevel, "%d", level);
            contalevel = level;
            blocklevel = 1;
            if(_DEBUG) printf("TTF level\n");
        }
        drawTextOnScreen(strlevel, SCREEN_WIDTH-160, 75, black);

        char strscore[10];
        // Only update score text if it has changed
        if(player.score != contascore || !blockscore) {
            sprintf(strscore, "%d", player.score);
            contascore = player.score;
            blockscore = 1;
            if(_DEBUG) printf("TTF score\n");
        }
        drawTextOnScreen(strscore, SCREEN_WIDTH-160, 160, black);

        char strlives[3];
        // Only update lives text if it has changed
        if(player.lives != contalives || !blocklives) {
            sprintf(strlives, "%d", player.lives);
            contalives = player.lives;
            blocklives = 1;
            if(_DEBUG) printf("TTF balls left\n");
        }
        drawTextOnScreen(strlives, SCREEN_WIDTH-160, 250, black);

        // Update the surface
        SDL_UpdateWindowSurface(gWindow);

        // Normalize framerate
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
}

void options(void) {

    // Home button variables
    int buttonhome_x = 5*PROP;
    int buttonhome_y = 5*PROP;
    int buttonhome_w = 11.2*PROP;
    int buttonhome_h = 5*PROP;

    // Option button variables
    int options_x = 0;
    int options_y = 0;
    int options_w = 100*PROP;
    int options_h = 70*PROP;

    // Right arrow variables
    int arrowr_x = 82*PROP;
    int arrowr_y = 60*PROP;
    int arrowr_w = 5.4*PROP;
    int arrowr_h = 5.4*PROP;

    // Left arrow variables
    int arrowl_x = 72*PROP;
    int arrowl_y = 60*PROP;
    int arrowl_w = 5.4*PROP;
    int arrowl_h = 5.4*PROP;

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
                    // If user pressed the 'ESCAPE' key
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // If the user has clicked with the left mouse button
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        // If mouse is over 'home' button
                        if(mouseX >= buttonhome_x
                        && mouseX <= buttonhome_x + buttonhome_w
                        && mouseY >= buttonhome_y
                        && mouseY <= buttonhome_y + buttonhome_h) {
                            // Game screen is 'home'
                            gameScreen = 0;
                            return;
                        }
                        // If mouse is over right arrow
                        else if(mouseX >= arrowr_x
                        && mouseX <= arrowr_x + arrowr_w
                        && mouseY >= arrowr_y
                        && mouseY <= arrowr_y + arrowr_h) {
                            // Change information screen
                            if(gScreen < 1) gScreen++;
                            return;
                        }
                        // If mouse is over left arrow
                        else if(mouseX >= arrowl_x
                        && mouseX <= arrowl_x + arrowl_w
                        && mouseY >= arrowl_y
                        && mouseY <= arrowl_y + arrowl_h) {
                            // Change information screen
                            if (gScreen > 0) gScreen--;
                            return;
                        }
                    }
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }

        // Fill the surface with #99d9ea (light blue)
        SDL_FillRect(gScreenSurface, NULL,
                    SDL_MapRGB(gScreenSurface->format, 0x99, 0xD9, 0xEA));

        // If mouse is over 'home' button
        int is_hovering = (mouseX >= buttonhome_x
                        && mouseX <= buttonhome_x + buttonhome_w
                        && mouseY >= buttonhome_y
                        && mouseY <= buttonhome_y + buttonhome_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonhome, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonhome->format, 0x70, 0x92, 0xBE));

        // Color key is always enabled for arrows
        SDL_SetColorKey(arrow_right, SDL_TRUE,
                        SDL_MapRGB(arrow_right->format, 0xff, 0xAE, 0xC9));
        SDL_SetColorKey(arrow_left, SDL_TRUE,
                        SDL_MapRGB(arrow_left->format, 0xff, 0xAE, 0xC9));

        // If there's an error loading any of the buttons
        if(drawOnScreen(gScreen == 0 ? optionsback : optionsback1, 0, 0,
                        options_w, options_h,
                        options_x, options_y) < 0
        || drawOnScreen(buttonhome, 0, 0, buttonhome_w, buttonhome_h,
                        buttonhome_x, buttonhome_y) < 0) {
              error(ERR_BLIT);
              quit = true;
        }

        if (gScreen) {
          if (drawOnScreen(arrow_left, 0, 0, arrowl_w, arrowl_h,
                          arrowl_x, arrowl_y) < 0) {
                error(ERR_BLIT);
                quit = true;
          }
        }
        else {
          if (drawOnScreen(arrow_right, 0, 0, arrowr_w, arrowr_h,
                          arrowr_x, arrowr_y) < 0) {
                error(ERR_BLIT);
                quit = true;
          }
        }

        // Update the surface
        SDL_UpdateWindowSurface(gWindow);

        // Normalize framerate
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
}

void ranking(void) {

    // Home button variables
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
                    // If user pressed the 'ESCAPE' key
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // If the user has clicked with the left mouse button
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        // If mouse is over 'home' button
                        if(mouseX >= buttonhome_x
                        && mouseX <= buttonhome_x + buttonhome_w
                        && mouseY >= buttonhome_y
                        && mouseY <= buttonhome_y + buttonhome_h) {
                            // Game screen is now 'home'
                            gameScreen = 0;
                            return;
                        }
                    }
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
              }
        }

        // If mouse is over 'home' button
        int is_hovering = (mouseX >= buttonhome_x
                        && mouseX <= buttonhome_x + buttonhome_w
                        && mouseY >= buttonhome_y
                        && mouseY <= buttonhome_y + buttonhome_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonhome, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonhome->format, 0x70, 0x92, 0xBE));

        // Fill the surface with #99d9ea (light blue)
        SDL_FillRect(gScreenSurface, NULL,
                    SDL_MapRGB(gScreenSurface->format, 0x99, 0xD9, 0xEA));

        // R: 0, G: 0, B: 0, alpha: 255
        SDL_Color black = { 0, 0, 0, 255 };

        int r;
        char strrank[13];
        for (r = 0; r < 5; r++) {
            sprintf(strrank, "%s: %u",
                    gRankedVector[r].name,
                    gRankedVector[r].score);
            drawTextOnScreen(strrank,
                            SCREEN_WIDTH/2-250,
                            SCREEN_HEIGHT/4+25*(r+1),
                            black);
        }


        // If there's an error loading the home button
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

    // Home button variables
    int buttonhome_x = 5*PROP;
    int buttonhome_y = 5*PROP;
    int buttonhome_w = 11.2*PROP;
    int buttonhome_h = 5*PROP;

    // Play button variables
    int buttonplay_x = 5*PROP;
    int buttonplay_y = 60*PROP;
    int buttonplay_w = 11.2*PROP;
    int buttonplay_h = 5*PROP;

    // Game Mode button variables
    int gamemode_x = 5*PROP;
    int gamemode_y = 25*PROP;
    int gamemode_w = 35*PROP;
    int gamemode_h = 5*PROP;

    // Game Physics button variables
    int physics_x = 5*PROP;
    int physics_y = 40*PROP;
    int physics_w = 35*PROP;
    int physics_h = 5*PROP;

    // Configuration button variables
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
                    // If user pressed the 'ESCAPE' key
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // If the user has clicked with the left mouse button
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        // If mouse is over 'game mode' button
                        if(mouseX >= gamemode_x
                        && mouseX <= gamemode_x + gamemode_w
                        && mouseY >= gamemode_y
                        && mouseY <= gamemode_y + gamemode_h) {
                            // Change gamemode from 1 to 2, or from 2 to 1
                            gGameMode = gGameMode == 1 ? 2 : 1;
                        }
                        // If mouse is over 'game physics' button
                        else if(mouseX >= physics_x
                        && mouseX <= physics_x + physics_w
                        && mouseY >= physics_y
                        && mouseY <= physics_y + physics_h) {
                            // Change gamemode physics from 1 to 2, or from 2
                            // to 1
                            gPhysics = gPhysics == 1 ? 2 : 1;
                        }
                        // If mouse is over 'play' button
                        else if(mouseX >= buttonplay_x
                        && mouseX <= buttonplay_x + buttonplay_w
                        && mouseY >= buttonplay_y
                        && mouseY <= buttonplay_y + buttonplay_h) {
                            // Game screen is now 'game'
                            gameScreen = 1;
                            // Start a new level
                            newLevel();
                            return;
                        }
                        // If mouse is over 'home' button
                        else if(mouseX >= buttonhome_x
                        && mouseX <= buttonhome_x + buttonhome_w
                        && mouseY >= buttonhome_y
                        && mouseY <= buttonhome_y + buttonhome_h) {
                            // Game screen is now 'home'
                            gameScreen = 0;
                            return;
                        }
                    }
                    break;
                default:
                // Supress warnings from [-Wswitch-default] flag
                break;
            }
        }

        // Fill the surface with #99d9ea (light blue)
        SDL_FillRect(gScreenSurface, NULL,
                    SDL_MapRGB(gScreenSurface->format, 0x99, 0xD9, 0xEA));

        // If mouse is over 'home' button
        is_hovering = (mouseX >= buttonhome_x
                    && mouseX <= buttonhome_x + buttonhome_w
                    && mouseY >= buttonhome_y
                    && mouseY <= buttonhome_y + buttonhome_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonhome, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonhome->format, 0x70, 0x92, 0xBE));

        // If mouse is over 'play' button
        is_hovering = (mouseX >= buttonplay_x
                    && mouseX <= buttonplay_x + buttonplay_w
                    && mouseY >= buttonplay_y
                    && mouseY <= buttonplay_y + buttonplay_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonplay, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonplay->format, 0x70, 0x92, 0xBE));

        // If mouse is over 'gamemode' button
        is_hovering = (mouseX >= gamemode_x
                    && mouseX <= gamemode_x + gamemode_w
                    && mouseY >= gamemode_y
                    && mouseY <= gamemode_y + gamemode_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttoncampaign, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttoncampaign->format, 0x70, 0x92, 0xBE));
        SDL_SetColorKey(buttonendless, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonendless->format, 0x70, 0x92, 0xBE));

        // If mouse is over 'game physics' button
        is_hovering = (mouseX >= physics_x
                    && mouseX <= physics_x + physics_w
                    && mouseY >= physics_y
                    && mouseY <= physics_y + physics_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonclassic, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonclassic->format, 0x70, 0x92, 0xBE));
        SDL_SetColorKey(buttonalternate, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonalternate->format, 0x70, 0x92,0xBE));

        // If there's an error loading any of the buttons
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

void end_game(void) {

    // Home button variables
    int buttonhome_x = 5*PROP;
    int buttonhome_y = 5*PROP;
    int buttonhome_w = 11.2*PROP;
    int buttonhome_h = 5*PROP;

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
                    // If user pressed the 'ESCAPE' key
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // If the user has clicked with the left mouse button
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        // If mouse is over 'home' button
                        if(mouseX >= buttonhome_x
                        && mouseX <= buttonhome_x + buttonhome_w
                        && mouseY >= buttonhome_y
                        && mouseY <= buttonhome_y + buttonhome_h) {
                            // Game screen is now 'menu'
                            gameScreen = 0;
                            // Reset variables
                            blocklevel = 0;
                            blockscore = 0;
                            player.score = 0;
                            player.aux_score = 0;
                            player.lives = 3;
                            level = 0;
                            return;
                        }
                    }
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }

        // Fill the surface with #99d9ea (light blue)
        SDL_FillRect(gScreenSurface, NULL,
                    SDL_MapRGB(gScreenSurface->format, 0x99, 0xD9, 0xEA));

        // If mouse is over 'home' button
        is_hovering = (mouseX >= buttonhome_x
                    && mouseX <= buttonhome_x + buttonhome_w
                    && mouseY >= buttonhome_y
                    && mouseY <= buttonhome_y + buttonhome_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonhome, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonhome->format, 0x70, 0x92, 0xBE));

        // If there's an error loading the 'home' button
        if (drawOnScreen(buttonhome, 0, 0,
                         buttonhome_w, buttonhome_h,
                         buttonhome_x, buttonhome_y) < 0) {
            error(ERR_BLIT);
            quit = true;
        }

        // R: 180, G: 180, B: 0, alpha: 255
        SDL_Color yellow = { 180, 180, 0, 255 };

        char strlevel[10];
        // Only update level text if it has changed
        if(level != contalevel || !blocklevel) {
            sprintf(strlevel, "Level: %d", level);
            contalevel = level;
            blocklevel = 1;
            if(_DEBUG) printf("TTF end_level\n");
        }
        drawTextOnScreen(strlevel, SCREEN_WIDTH-260, 75, yellow);

        char strscore[18];
        // Only update score text if it has changed
        if((player.score != contascore) || (!blockscore)) {
            sprintf(strscore, "Score: %d", player.score);
            contascore = player.score;
            blockscore = 1;
            if(_DEBUG) printf("TTF end_score\n");
        }
        drawTextOnScreen(strscore, SCREEN_WIDTH-260, 160, yellow);

        // Update the surface
        SDL_UpdateWindowSurface(gWindow);

        // Normalize framerate
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
}

void name_input(void) {

    // Home button variables
    int buttonhome_x = 5*PROP;
    int buttonhome_y = 5*PROP;
    int buttonhome_w = 11.2*PROP;
    int buttonhome_h = 5*PROP;

    // Mouse position
    int mouseX, mouseY;

    int is_hovering;

    // Cursor for writing name
    int cursor;

    // Username letters
    char letters[] = { 'Z', 'Z', 'Z', '\0' };

    FILE *pFile;

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
                    // If user pressed the 'ESCAPE' key
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // If the user has clicked with the left mouse button
                    if(e.button.button == SDL_BUTTON_LEFT) {
                        // If mouse is over 'home' button
                        if(mouseX >= buttonhome_x
                        && mouseX <= buttonhome_x + buttonhome_w
                        && mouseY >= buttonhome_y
                        && mouseY <= buttonhome_y + buttonhome_h) {
                            // Game screen is now 'menu'
                            gameScreen = 0;
                            // Reset variables
                            blocklevel = 0;
                            blockscore = 0;
                            player.score = 0;
                            player.aux_score = 0;
                            player.lives = 3;
                            level = 0;
                            return;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    switch(e.key.keysym.sym) {
                        case SDLK_a: letters[cursor++] = 'A'; break;
                        case SDLK_b: letters[cursor++] = 'B'; break;
                        case SDLK_c: letters[cursor++] = 'C'; break;
                        case SDLK_d: letters[cursor++] = 'D'; break;
                        case SDLK_e: letters[cursor++] = 'E'; break;
                        case SDLK_f: letters[cursor++] = 'F'; break;
                        case SDLK_g: letters[cursor++] = 'G'; break;
                        case SDLK_h: letters[cursor++] = 'H'; break;
                        case SDLK_i: letters[cursor++] = 'I'; break;
                        case SDLK_j: letters[cursor++] = 'J'; break;
                        case SDLK_k: letters[cursor++] = 'K'; break;
                        case SDLK_l: letters[cursor++] = 'L'; break;
                        case SDLK_m: letters[cursor++] = 'M'; break;
                        case SDLK_n: letters[cursor++] = 'N'; break;
                        case SDLK_o: letters[cursor++] = 'O'; break;
                        case SDLK_p: letters[cursor++] = 'P'; break;
                        case SDLK_q: letters[cursor++] = 'Q'; break;
                        case SDLK_r: letters[cursor++] = 'R'; break;
                        case SDLK_s: letters[cursor++] = 'S'; break;
                        case SDLK_t: letters[cursor++] = 'T'; break;
                        case SDLK_u: letters[cursor++] = 'U'; break;
                        case SDLK_v: letters[cursor++] = 'V'; break;
                        case SDLK_w: letters[cursor++] = 'W'; break;
                        case SDLK_x: letters[cursor++] = 'X'; break;
                        case SDLK_y: letters[cursor++] = 'Y'; break;
                        case SDLK_z: letters[cursor++] = 'Z'; break;
                        case SDLK_KP_0: case SDLK_KP_00: case SDLK_KP_000:
                        case SDLK_0: letters[cursor++] = '0'; break;
                        case SDLK_KP_1:
                        case SDLK_1: letters[cursor++] = '1'; break;
                        case SDLK_KP_2:
                        case SDLK_2: letters[cursor++] = '2'; break;
                        case SDLK_KP_3:
                        case SDLK_3: letters[cursor++] = '3'; break;
                        case SDLK_KP_4:
                        case SDLK_4: letters[cursor++] = '4'; break;
                        case SDLK_KP_5:
                        case SDLK_5: letters[cursor++] = '5'; break;
                        case SDLK_KP_6:
                        case SDLK_6: letters[cursor++] = '6'; break;
                        case SDLK_KP_7:
                        case SDLK_7: letters[cursor++] = '7'; break;
                        case SDLK_KP_8:
                        case SDLK_8: letters[cursor++] = '8'; break;
                        case SDLK_KP_9:
                        case SDLK_9: letters[cursor++] = '9'; break;
                        case SDLK_PERIOD: case SDLK_KP_PERIOD:
                                     letters[cursor++] = '.'; break;
                        case SDLK_COMMA: case SDLK_KP_COMMA:
                                     letters[cursor++] = ','; break;
                        case SDLK_F12: error(ERR_EST_EGG);    break;
                        case SDLK_LEFT:
                            cursor = --cursor < 0 ? 2 : cursor;
                            break;
                        case SDLK_RIGHT:
                            cursor = ++cursor > 2 ? 0 : cursor;
                            break;

                        // If user presses [ENTER] key
                        case SDLK_RETURN:case SDLK_RETURN2:case SDLK_KP_ENTER:
                            if(_DEBUG) {
                                printf("[Inserted name %s with score %u]\n",
                                    letters, gRankedVector[4].score);
                            }
                            // Set the username to input
                            sprintf(gRankedVector[4].name, letters);

                            // Sort ranked players
                            unsigned _s = sizeof(RANKED);
                            qsort(gRankedVector, 5, _s, compare_score);
                            pFile = fopen("./ranking","wb");
                            if (!pFile) {
                                printf("Unable to open file!");
                                gameScreen = 0;
                            } else {
                                fwrite(gRankedVector, sizeof(RANKED), 5, pFile);
                                fclose(pFile);
                                gameScreen = 3;
                            }
                            return;
                        default: break;
                    }
                    break;
                default:
                    // Supress warnings from [-Wswitch-default] flag
                    break;
            }
        }

        // Fill the surface with #99d9ea (light blue)
        SDL_FillRect(gScreenSurface, NULL,
                    SDL_MapRGB(gScreenSurface->format, 0x99, 0xD9, 0xEA));

        // If mouse is over 'home' button
        is_hovering = (mouseX >= buttonhome_x
                    && mouseX <= buttonhome_x + buttonhome_w
                    && mouseY >= buttonhome_y
                    && mouseY <= buttonhome_y + buttonhome_h);

        // If mouse is over button, color key is disabled; Otherwise, enabled
        SDL_SetColorKey(buttonhome, is_hovering ? SDL_FALSE : SDL_TRUE,
                        SDL_MapRGB(buttonhome->format, 0x70, 0x92, 0xBE));

        // If there's an error loading the 'home' button
        if (drawOnScreen(buttonhome, 0, 0,
                         buttonhome_w, buttonhome_h,
                         buttonhome_x, buttonhome_y) < 0) {
            error(ERR_BLIT);
            quit = true;
        }

        // R: 0, G: 0, B: 0, alpha: 255
        SDL_Color black = { 0, 0, 0, 255 };

        cursor = cursor > 2 ? 0 : cursor < 0 ? 2 : cursor;
        drawTextOnScreen(letters, SCREEN_WIDTH/2, 160, black);
        drawTextOnScreen("^", SCREEN_WIDTH/2 + cursor*15, 180, black);


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

    // For every ball (multiball, not implemented yet)
    for (i= 0; i < LEN; i++) {
        for (j = i + 1; j < LEN; j++) {
            // Distance X
            int deltaX = ball[i].posX - ball[j].posX;
            // Distance Y
            int deltaY = ball[i].posY - ball[j].posY;

            // If dx² + dy² < width², balls are colliding
            if(deltaX * deltaX + deltaY * deltaY < BALL_WIDTH * BALL_WIDTH) {
                // Swap balls' horizontal speeds
                temp = ball[i].stepX;
                ball[i].stepX = ball[j].stepX;
                ball[j].stepX = temp;
                // Swap balls' vertical speeds
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

    // For every ball (multiball, not implemented yet)
    for (i = 0; i < LEN; i++) {
        // For every brick
        for (j = 0; j < COLUMNS; j++) {
            for (k = 0; k < LINES; k++) {
                BLOCK current = brick[j][k];

                int over = ball[i].posY + BALL_HEIGHT >= current.posY;
                int under = ball[i].posY <= current.posY + BLOCK_HEIGHT;
                int left = ball[i].posX <= current.posX + BLOCK_WIDTH;
                int right = ball[i].posX + BALL_WIDTH >= current.posX;

                // If ball is colliding with a brick with resistance different
                // from 0 (greater or lesser than 0)
                if(current.resist && over && under && left && right) {
                    // If resistance is lesser than 0
                    if(current.resist < 0) {
                        brick[j][k].resist = 0;
                    }
                    // Resistance must be positive
                    else {
                        // You can't ever be too sure, right?
                        if(current.resist > 0 && current.resist < 6) {
                            player.score += 100;
                            player.aux_score += 100;
                            levelClear--;
                        }

                        // If resist minus 1 is lesser than 0, then resist
                        // becomes 0; Otherwise, it becomes itself minus one
                        if (current.resist < 6) {
                            brick[j][k].resist = current.resist-1 < 0
                                        ? 0
                                        : current.resist-1;
                        }

                        // If block was broken and sounds are on
                        if(!brick[j][k].resist && gSound) {
                            // Play brick breaking sound
                            Mix_PlayChannel(-1, gBrickWAV, 0);
                        }

                        // If there are no powerups
                        if(!gPowerUp) {
                            // 11% chance of a powerup
                            gPowerUp = rand() % 100 < 11 ? 1 : 0;
                            // If player should get a powerup
                            if(gPowerUp) {
                                // Reset powerups
                                controlInverter = 0;
                                bigracket = 0;
                                smallracket = 0;
                            }
                            // Update powerup position
                            powerup_x = current.posX + 29;
                            powerup_y = current.posY;
                        }

                        // Collision side checking
                        // Adapted from:
                        // https://gamedev.stackexchange.com/a/29796
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

                        if(abs_dist_x <= avg_width
                        && abs_dist_y <= avg_height) {
                            if(cross_width > cross_height) {
                                if(cross_width > -cross_height) {
                                    // Ball hit the bottom of the brick
                                    ball[i].stepY = absolute(ball[i].stepY);
                                } else {
                                    // Ball hit the left side of the brick
                                    ball[i].stepX = -absolute(ball[i].stepX);
                                }
                            } else {
                                if(cross_width > -cross_height) {
                                    // Ball hit the right side of the brick
                                    ball[i].stepX = absolute(ball[i].stepX);
                                } else {
                                    // Ball hit the top of the brick
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

    // For every ball (multiball, not implemented yet)
    for (i = 0; i < LEN; i++) {
        // If 'classic' physics
        if (gPhysics == 1) {
            int top_limit = ball[i].posY + BALL_HEIGHT >= player.posY;
            // 2 pixels of tolerance
            int bottom_limit = ball[i].posY + BALL_HEIGHT <= player.posY + 2;
            int left_limit = ball[i].posX + BALL_WIDTH >= player.posX;
            int right_limit = ball[i].posX <= player.posX + RACKET_WIDTH;

            if(top_limit && bottom_limit && left_limit && right_limit) {
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
        // If 'alternate' physics
        else if(gPhysics == 2) {
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
                if(gSound) {
                    Mix_PlayChannel(-1, gBrickWAV, 0);
                    Mix_PlayChannel(-1, gRacketWAV, 0);
                }
            }
        }
    }
}

void collisionPowerUp(void) {
    int bottom_limit = powerup_y + 42 >= player.posY + 2;
    int left_limit = powerup_x + 42 >= player.posX;
    int right_limit = powerup_x <= player.posX + RACKET_WIDTH;

    if(bottom_limit && left_limit && right_limit) {
        gPowerUp = 0;

        switch((rand())%4) {
            case 0:
                player.lives++;
                if (_DEBUG) printf("VIDA\n");
                break;
            case 1:
                controlInverter = 1;
                if (_DEBUG) printf("INVERTER\n");
                break;
            case 2:
                bigracket = 1;
                if (_DEBUG) printf("BIG\n");
                break;
            case 3:
                smallracket = 1;
                if (_DEBUG) printf("SMALL\n");
                break;
            default:
                break;
        }
    }
}


void newLevel(void) {
    // Iteration variables
    int i, j, k;

    player.score += 1000;
    player.score += bonus;
    player.aux_score += 1000;
    player.aux_score += bonus;
    ball_in_game = false;

    for (k = 0; k < LEN; k++) {
        ball[k].posY = player.posY - BALL_HEIGHT;
        ball[k].posX = player.posX + RACKET_WIDTH / 2 - BALL_WIDTH / 2;
        ball[k].stepX = 0;
        ball[k].stepY = 0;
    }

    levelClear = 0;
    level++;
    gTime = time(0);


    int resist_array[COLUMNS * LINES];
    FILE *pFile;
    char level_string[15];

    if(level > 0 && level < 10) {
        sprintf(level_string, "./levels/%d.lvl", level-1);
    } else {
        sprintf(level_string, "./levels/1.lvl");
    }

    pFile = fopen(level_string,"rb");

    /* gGameMode == 1 -> campaign
     * gGameMode == 2 -> endless */
    // If there is an error loading the file, or if the gamemode is 'endless'
    if (!pFile || gGameMode == 2) {
        if(_DEBUG && gGameMode == 1) printf("Unable to open file!");
        else if(!!pFile) fclose(pFile);

        for (i = 0; i < COLUMNS; i++) {
            for (j = 0; j < LINES; j++) {
                // Load all blocks with resist 1
                brick[i][j].resist = 1;
            }
        }
        levelClear = COLUMNS * LINES;
        if(_DEBUG) printf("Level clear set to %d\n", levelClear);
    } else {
        fread(resist_array, sizeof(int), COLUMNS * LINES, pFile);
        if(_DEBUG) printf("Reading level %d from file\n", level-1);
        for (i = 0; i < COLUMNS; i++) {
            for (j = 0; j < LINES; j++) {
                brick[i][j].resist = resist_array[i*LINES + j];
                if (brick[i][j].resist < 6)
                  levelClear += resist_array[i*LINES + j];
            }
        }
        if(_DEBUG) printf("Level clear set to %d\n", levelClear);
        fclose(pFile);
    }

    // Reset power up
    gPowerUp = 0;
    controlInverter = 0;
    bigracket = 0;
    smallracket = 0;
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

    if(TTF_Init() == -1) {
        error(ERR_INIT);
        return false;
    }

    gFont = TTF_OpenFont("./fonts/PressStart2P.ttf", 17);

    // Get window surface
    gScreenSurface = SDL_GetWindowSurface(gWindow);

    FILE *pFile;

    pFile = fopen("./ranking","rb");
    if(!pFile) {
        error(ERR_RANK);
        return false;
    } else {
        fread(gRankedVector, sizeof(RANKED), 5, pFile);
        fclose(pFile);
    }

    return true;
}

int loadMedia(void) {
    // Load PNG surfaces
    if((gBall = loadSurface("./images/circle.png")) == NULL
    || (gBRICKSurface = loadSurface("./images/brick.png")) == NULL
    || (gPLAYERSurface = loadSurface("./images/racket.png")) == NULL
    || (gPLAYERSMALLSurface = loadSurface("./images/racketsmall.png")) == NULL
    || (gPLAYERLARGESurface = loadSurface("./images/racketlarge.png")) == NULL
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
    || (buttonplay = loadSurface("./images/playbutton.png")) == NULL
    || (pause = loadSurface("./images/pause.png")) == NULL
    || (buttonquit = loadSurface("./images/quitbutton.png")) == NULL
    || (optionsback = loadSurface("./images/optionsback.png")) == NULL
    || (optionsback1 = loadSurface("./images/optionsback1.png")) == NULL
    || (power_up = loadSurface("./images/powerup.png")) == NULL
    || (arrow_right = loadSurface("./images/arrow_right.png")) == NULL
    || (arrow_left = loadSurface("./images/arrow_left.png")) == NULL) {
        error(ERR_IMG_LOAD);
        return false;
    }

    // Color key
    SDL_SetColorKey(gBall, SDL_TRUE,
                    SDL_MapRGB(gBall->format, 0xff, 0xAE, 0xC9));
    SDL_SetColorKey(gPLAYERSurface, SDL_TRUE,
                    SDL_MapRGB(gPLAYERSurface->format, 0xff, 0xAE, 0xC9));
    SDL_SetColorKey(gPLAYERSMALLSurface, SDL_TRUE,
                    SDL_MapRGB(gPLAYERSMALLSurface->format, 0xff, 0xAE, 0xC9));
    SDL_SetColorKey(gPLAYERLARGESurface, SDL_TRUE,
                    SDL_MapRGB(gPLAYERLARGESurface->format, 0xff, 0xAE, 0xC9));
    SDL_SetColorKey(sound, SDL_TRUE,
                    SDL_MapRGB(sound->format, 0xff, 0xAE, 0xC9));
    SDL_SetColorKey(power_up, SDL_TRUE,
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

    gMusicWAV = Mix_LoadMUS("./sounds/music.wav");
    if(!gMusicWAV) {
        error(ERR_WAV_LOAD);
        return false;
    }

    // Play music
    if(can_music_play && Mix_PlayMusic(gMusicWAV, -1)==-1) {
        // There is no music, but the game is still playable
        printf("Failed to load music!\n%s\n", Mix_GetError());
    }
    can_music_play = 0;

    return true;
}

void closef() {
    // Free loaded image
    SDL_FreeSurface(gBall), SDL_FreeSurface(gBRICKSurface),
    SDL_FreeSurface(gPLAYERSurface), SDL_FreeSurface(gPLAYERSMALLSurface),
    SDL_FreeSurface(gPLAYERLARGESurface), SDL_FreeSurface(buttonnew),
    SDL_FreeSurface(buttonoptions), SDL_FreeSurface(buttonrankings),
    SDL_FreeSurface(side_bar);
    gBall = gBRICKSurface = gPLAYERSurface = gPLAYERSMALLSurface = gPLAYERLARGESurface = NULL;
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

int drawTextOnScreen(char* text, int dstX, int dstY, SDL_Color color) {

    SDL_Surface* text_surface;
    SDL_Rect dstRect;

    dstRect.x = dstX;
    dstRect.y = dstY;

    if(!gFont) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return false;
    }

    if(!(text_surface = TTF_RenderText_Solid(gFont, text, color))) {
        error(0);
        return false;
    }

    int _r = SDL_BlitSurface(text_surface, NULL, gScreenSurface, &dstRect);
    SDL_FreeSurface(text_surface);

    return _r;
}

// Time left until next tick
unsigned time_left(void) {
    unsigned now = SDL_GetTicks();
    return (next_time <= now) ? 0 : next_time - now;
}

void error(int code) {
    _0 _=0;
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
            break;
        case ERR_WAV_LOAD:
            printf("Unable to load WAV!");
            break;
        case ERR_FONT_LOAD:
            printf("Unable to load TTF!");
            break;
        case ERR_EST_EGG:
            _=42&15;
            printf("         ___%c      .-*)) `*-.%c     /*  ((*   *'.%c    |   *))  *   *\\%c",_,_,_,_);
            printf("    | *  ((*   *  /%c     \\  *))  *  .'%c      '-.((*_.-'%c",_,_,_);
            break;
        default:
            printf("Unspecified error!");
            break;
    }
    if(!_) printf(" SDL Error: %s\n", SDL_GetError());
}

int absolute(int n) {
    return n < 0 ? -n : n;
}

int compare_score(const void *a, const void *b) {
    RANKED *x = (RANKED *) a;
    RANKED *y = (RANKED *) b;
    if(x->score > y->score) return-1;
    if(x->score < y->score) return 1;
    return 0;
}