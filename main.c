/*
 *    Breakout game in SDL
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

#include "defs.h"
#include "globals.h"

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
        } else if (gameScreen == 2) {
            options();
        } else if (gameScreen == 3) {
            ranking();
        } else if (gameScreen == 4) {
            configuration();
        } else {
            printf("Error: game screen is invalid\n");
            return 1;
        }
    }

    // Frees resources and closes SDL
    closef();

    // Supress warnings from [-Wunused-parameter] flag
    (void)argc;
    (void)args;

    return 0;
}