#ifndef _H_MAIN
#define _H_MAIN

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define MAX_VEL 10

#define TILE_WIDTH  32
#define TILE_HEIGHT 32
#define WORLD_WIDTH 64
#define WORLD_HEIGHT 64
#define MAX_TILES (WORLD_WIDTH * WORLD_HEIGHT)

#define MAX_ENTITIES 65536

#define CAM_SPEED 25

#define BULLET_RATE 36

unsigned char *worldmap;

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

SDL_Joystick *gJoystick;

//Game controller stuff
int gNumGamepads;

//Mouse position stuff
static int gMouseX, gMouseY;

//Keyboard stuff
static unsigned char *gKeyState;

bool init();
void shutdown();



#endif