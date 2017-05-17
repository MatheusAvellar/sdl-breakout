#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED //prevents SDL's main() function conflict
#endif
 
//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*
 * Constants
 */

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int false = 0;
const int true = 1;

const int IMAGE_WIDTH = 81;
const int IMAGE_HEIGHT = 81;

typedef struct _NPC 
{
    int posX;
    int posY;
    int stepX;
    int stepY;
    SDL_Surface* image;
    int imgW;
    int imgH;
} NPC;

int temp;

/*
 * Global Variables
 */

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The imagem character
#define LEN 2
int i, j;
NPC ball[LEN];
  
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Current displayed PNG image
SDL_Surface* gJPGSurface = NULL;

/*
 * function prototypes
 */

//Starts up SDL and creates window
int init();

//Loads media
int loadMedia();

//Frees media and shuts down SDL
void closing();

//Loads individual image
SDL_Surface* loadSurface( char *path );

//Create NPC
NPC createNPC( int posX, int posY, int stepX, int stepY, SDL_Surface *image);

//Move NPC
void moveNPC(NPC *p);


int main( int argc, char* args[] ) {
    SDL_Rect srcRect, dstRect;
    
    //Start up SDL and create window
    if( !init() ) {
        printf( "Failed to initialize!\n" );
    }
    else {
        //Load media
        if( !loadMedia() ) {
            printf( "Failed to load media!\n" );
        }
        else {   
            //Create NPC
			for (i= 0; i < LEN; i++) {
				ball[i] = createNPC((rand() % (SCREEN_WIDTH/LEN - IMAGE_WIDTH)) + SCREEN_WIDTH*i/LEN, 
                             rand() % (SCREEN_HEIGHT - IMAGE_HEIGHT), 
                             rand() % 2 ? -1-(i%2): 1+(i%2), 
                             rand() % 2 ? -1-(i%2): 1+(i%2), 
                             gJPGSurface);
			}
			
            //Main loop flag
            int quit = false;

            //Event handler
            SDL_Event e;

            //While application is running
            while( !quit ) {
                while( SDL_PollEvent( &e ) != 0 ) {
                    switch (e.type) {
                        case SDL_QUIT:
                            quit = true;
                            break;
                        case SDL_KEYDOWN:
                            if (e.key.keysym.sym == SDLK_ESCAPE) {
                                quit = true;
                            }
                        break;
                    }
                }
				
            
                //Fill the surface white
                SDL_FillRect( gScreenSurface, NULL, 
                              SDL_MapRGB( gScreenSurface->format, 
                              0x00, 0xFF, 0x00 ) );
							  
				for (i= 0; i < LEN; i++) {
					moveNPC(&ball[i]);
				}

                srcRect.x = 0; srcRect.y = 0;
                srcRect.w = IMAGE_WIDTH;
                srcRect.h = IMAGE_HEIGHT;
				
				for (i= 0; i < LEN; i++) {
					dstRect.x = ball[i].posX;
					dstRect.y = ball[i].posY;
            
					if( SDL_BlitSurface( ball[i].image, &srcRect, 
										 gScreenSurface, &dstRect ) < 0 ) {
						printf( "SDL could not blit! SDL Error: %s\n", SDL_GetError() );
						quit = true;
					}
				}
				
				//Collision between balls
				for (i= 0; i < LEN; i++) {
					for (j = i + 1; j < LEN; j++) {
						if ((ball[i].posX - ball[j].posX)*(ball[i].posX - ball[j].posX) + (ball[i].posY - ball[j].posY)*(ball[i].posY - ball[j].posY) < IMAGE_WIDTH*IMAGE_WIDTH) {
							temp = ball[i].stepX; ball[i].stepX = ball[j].stepX; ball[j].stepX = temp;
							temp = ball[i].stepY; ball[i].stepY = ball[j].stepY; ball[j].stepY = temp;
						}
					}
				}
            
                //Update the surface
                SDL_UpdateWindowSurface( gWindow );
                
                // Not so good solution, depends on your computer
                SDL_Delay(5);
            }
        }
    }

    //Free resources and closing SDL
    closing();

    return 0;
}


void moveNPC(NPC *p) {
    p->posX += p->stepX;
    p->posY += p->stepY;
    
    if ( (p->posX + IMAGE_WIDTH > SCREEN_WIDTH) ||
         (p->posX < 0) ) {
        p->stepX = -p->stepX;
        p->posX += p->stepX; 
    }
    if ( (p->posY + IMAGE_HEIGHT > SCREEN_HEIGHT) ||
         (p->posY < 0) ) {
        p->stepY = -p->stepY;
        p->posY += p->stepY;
    }   
}


//Create NPC
NPC createNPC( int posX, int posY, int stepX, int stepY, 
               SDL_Surface *image) {
    NPC p;
    
    p.posX = posX;
    p.posY = posY;
    p.stepX = stepX;
    p.stepY = stepY;
    p.image = image;
    return p;
}

int init() {
    //Initialization flag
    int success = true;

    srand(time(NULL));

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else {
        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial: 06_moving_images", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL ) {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Initialize JPG and PNG loading 
            int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
            if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                success = false;
            }
            else {
                //Get window surface
                gScreenSurface = SDL_GetWindowSurface( gWindow );
            }
        }
    }

   

    return success;
}

int loadMedia() {
    //Loading success flag
    int success = true;
    //uint32_t colorKey;
    
    //Load PNG surface
    gJPGSurface = loadSurface( "./redCircle.png" );
    if( gJPGSurface == NULL ) {
        printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
        success = false;
    } 
		
	//Color key
	SDL_SetColorKey(gJPGSurface, SDL_TRUE, SDL_MapRGB(gJPGSurface->format, 0x00, 0x00, 0x00));
	
    return success;
}

void closing() {
    //Free loaded image
    SDL_FreeSurface( gJPGSurface );
    gJPGSurface = NULL;

    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Surface* loadSurface( char *path ) {
    //The final optimized image
    SDL_Surface* optimizedSurface = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path );
    if( loadedSurface == NULL ) {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
	}
    else {
        //Convert surface to screen format
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
        if( optimizedSurface == NULL ) {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );	
    }
    return optimizedSurface;
}

