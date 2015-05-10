#ifndef _H_MAIN
#define _H_MAIN

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MAX_VEL 10

#define TILE_WIDTH  64
#define TILE_HEIGHT 64
#define WORLD_WIDTH 64
#define WORLD_HEIGHT 64
#define MAX_TILES (WORLD_WIDTH * WORLD_HEIGHT)

#define MAX_ENTITIES 1024

unsigned char *worldmap;

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

//Game controller stuff
int gNumGamepads;

//Mouse position stuff
int gMouseX, gMouseY;

//Keyboard stuff
unsigned char *gKeyState;

bool init();
void shutdown();



#endif