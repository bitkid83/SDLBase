#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "main.h"
#include "vectormath.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define MAX_VEL 10

#define TILE_WIDTH  64
#define TILE_HEIGHT 60
#define WORLD_WIDTH 128
#define WORLD_HEIGHT 128
#define MAX_TILES (WORLD_WIDTH * WORLD_HEIGHT)

unsigned char *worldmap;

bool done = false; //quit when done is true

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

// TODO: 
// 1) Entities need their own source files
// 2) Create a doubly-linked list of Entities for #3?
// 3) Entities should be able to communicate to/from one another.
struct Entity
{
	vec2 position;
	vec2 velocity;
	vec2 speed;
	int width;
	int height;
	SDL_Rect playerRect;
};

//Frame timing stuff
int gStartTime = 0;
int gLastTime = 0;
double gFrameTime = 0.0;

//Keyboard stuff
unsigned char *gKeyState;

//Game controller stuff
int gNumGamepads = 0;

//Mouse position stuff
int gMouseX, gMouseY;

//Global entities
Entity gPlayer;
Entity gProjectile;

void GenTiles()
{
	worldmap = (unsigned char*)malloc(MAX_TILES);

	for (int i = 0; i < MAX_TILES; i++) {
		worldmap[i] = rand() % 3;
		
		//printf("Tile #%d = %d\n", i, worldmap[i]);
	}
}

SDL_Rect TileIndex(int x, int y)
{
	int index;
	SDL_Rect temp;
	temp.w = TILE_WIDTH;
	temp.h = TILE_HEIGHT;

	index = x + y * WORLD_WIDTH;
	if (worldmap[index] == 0) {
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	}
	else if (worldmap[index] == 1) {
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
	}
	else if (worldmap[index] == 2) {
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
	}
	temp.x = x * TILE_WIDTH;
	temp.y = y * TILE_HEIGHT;

	return temp;
}

void RenderTiles()
{
    SDL_Rect *temprect;
    temprect = (SDL_Rect*)malloc(sizeof(SDL_Rect));

    for (int y = 0; y < SCREEN_HEIGHT / TILE_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH / TILE_WIDTH; x++) {
            *temprect = TileIndex(x - (int)gPlayer.position.x / TILE_WIDTH, y - (int)gPlayer.position.y / TILE_HEIGHT);
            SDL_RenderFillRect(gRenderer, temprect);
            //SDL_RenderFillRect(gRenderer, &TileIndex(x - (int)gPlayer.position.x / TILE_WIDTH, y - (int)gPlayer.position.y / TILE_HEIGHT ));
			//SDL_RenderFillRect(gRenderer, &TileIndex(x & ((int)gPlayer.position.x / TILE_WIDTH) - 1, y & ((int)gPlayer.position.y / TILE_HEIGHT) - 1));
			//SDL_RenderFillRect(gRenderer, &TileIndex(x, y));			
		}
	}
    
    free(temprect);
}

void HandlePlayerInput()
{
	//SDL_PumpEvents();
	if (gKeyState[SDL_SCANCODE_W]) {
		gPlayer.velocity.y += -1;
	}
	if (gKeyState[SDL_SCANCODE_S]) {
		gPlayer.velocity.y += 1;
	}
	if (gKeyState[SDL_SCANCODE_A]) {
		gPlayer.velocity.x += -1;
	}
	if (gKeyState[SDL_SCANCODE_D]) {
		gPlayer.velocity.x += 1;
	}

	//Add velocities to player position
	gPlayer.position.x += (gPlayer.velocity.x * gFrameTime);
	gPlayer.position.y += (gPlayer.velocity.y * gFrameTime);

	gPlayer.velocity.x *= 0.05;
	gPlayer.velocity.y *= 0.05;

	//if (fabs(gPlayer.velocity.x) < 0.00001) gPlayer.velocity.x = 0;
	//if (fabs(gPlayer.velocity.y) < 0.00001) gPlayer.velocity.y = 0;

	//printf("(X, Y): %f, %f\n", gPlayer.velocity.x, gPlayer.velocity.y);
}

bool init()
{
	bool success = false;

	gWindow = NULL;
	gRenderer = NULL;

	//Initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Error initializing SDL: %s\n", SDL_GetError());
		success = false;
	}
	//Create SDL window
	else {
		gWindow = SDL_CreateWindow(	"SDL Base", 
									 SDL_WINDOWPOS_CENTERED, 
									 SDL_WINDOWPOS_CENTERED, 
									 SCREEN_WIDTH, 
									 SCREEN_HEIGHT, 
									 SDL_WINDOW_SHOWN);

		if (gWindow == NULL) {
			printf("Error creating SDL Window: %s\n", SDL_GetError());
			success = false;
		}
		//create 2D rendering context
		else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

			if (gRenderer == NULL) {
				printf("Error creating SDL Renderer: %s\n", SDL_GetError());
				success = false;
			}
		}

		success = true;
	}

	//Initialize player
	gPlayer.width = 16;
	gPlayer.height = 16;
	gPlayer.speed.x = 0;
	gPlayer.speed.y = 0;
	gPlayer.position.x = (SCREEN_WIDTH / 2) - gPlayer.width;
	gPlayer.position.y = (SCREEN_HEIGHT / 2) - gPlayer.height;
	//gPlayer.position.x = 0;
	//gPlayer.position.y = 0;
	gPlayer.velocity.x = 0;
	gPlayer.velocity.y = 0;
	gPlayer.playerRect = { (int)gPlayer.position.x, 
						   (int)gPlayer.position.y, 
								gPlayer.width, 
	  						    gPlayer.height };

	//Initialize Projectile
	gProjectile.width = 5;
	gProjectile.height = 5;
	gProjectile.speed.x = 0;
	gProjectile.speed.y = 0;
	gProjectile.position.x = 0;
	gProjectile.position.y = 0;
	gProjectile.velocity.x = 0;
	gProjectile.velocity.y = 0;
	gProjectile.playerRect = {  (int)gProjectile.position.x, 
								(int)gProjectile.position.y, 
									gProjectile.width, 
									gProjectile.height };

	//Set up SDL Game Controllers	
	gNumGamepads = SDL_NumJoysticks();
	if (gNumGamepads > 0) {
		printf("Gamepads connected: %d\n", gNumGamepads);
	}
	else {
		printf("No gamepads found! %s\n", SDL_GetError());
	}

	//Retrieve keyboard state snapshot
	gKeyState = (unsigned char*)SDL_GetKeyboardState(NULL);

	//Disable mouse cursor
	SDL_ShowCursor(SDL_DISABLE);

	//Get initial ticks for frame timer
	gStartTime = SDL_GetTicks();

	SDL_RenderClear(gRenderer);

	GenTiles();
	return success;
}

void refresh()
{
	SDL_Texture *tmptex;
	tmptex = (SDL_Texture*)malloc(sizeof(tmptex));

	SDL_Event evt;	
	vec2 mouse_relative_to_player = { 0, 0 };

	//Game loop
	while (!done) {		
		//SDL Event loop

		//SDL_PumpEvents(); //not needed, SDL_PollEvent already pumps events
		
		// ++ FRAME START ++
		gLastTime = SDL_GetTicks() - gStartTime;
		
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
				case SDL_QUIT:
					done = true;
					break;
				case SDL_MOUSEBUTTONDOWN:
					gProjectile.position.x = gPlayer.position.x;
					gProjectile.position.y = gPlayer.position.y;

					//Left Mouse Button
					if (evt.button.button == SDL_BUTTON_LEFT) {	
						mouse_relative_to_player = { evt.button.x - gPlayer.position.x, evt.button.y - gPlayer.position.y };
						mouse_relative_to_player = Vec2Normalize(mouse_relative_to_player);
						//printf("%f %f\n", mouse_relative_to_player.x, mouse_relative_to_player.y);
					}
					//Right Mouse Button
					else if (evt.button.button == SDL_BUTTON_RIGHT) {
					}
					break;
				case SDL_KEYUP:
					if (evt.key.keysym.sym == SDLK_ESCAPE) {
						done = true;
					}
					break;
					
			}			
		}

		//Clear the screen to black
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);

		RenderTiles();

		//Render the player (red square)
		HandlePlayerInput();
		
		//Constrain player to the screen boundaries
		//if (gPlayer.position.x > SCREEN_WIDTH - gPlayer.width) { gPlayer.position.x = (SCREEN_WIDTH - gPlayer.width) - 1; }
		//if (gPlayer.position.x < 0) { gPlayer.position.x = 0; }
		//if (gPlayer.position.y > SCREEN_HEIGHT - gPlayer.height) { gPlayer.position.y = (SCREEN_HEIGHT - gPlayer.height) - 1; }
		//if (gPlayer.position.y < 0) { gPlayer.position.y = 0; }

		gPlayer.playerRect = { (int)gPlayer.position.x, (int)gPlayer.position.y, gPlayer.width, gPlayer.height };		
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderFillRect(gRenderer, &gPlayer.playerRect);

		//Mouse crosshair
		SDL_GetMouseState(&gMouseX, &gMouseY);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xAA, 0xCA, 0xFF);
		SDL_RenderDrawLine(gRenderer, gMouseX - 5, gMouseY, gMouseX + 5, gMouseY);
		SDL_RenderDrawLine(gRenderer, gMouseX, gMouseY - 5, gMouseX, gMouseY + 5);

		//Aiming laser
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_RenderDrawLine(gRenderer, gPlayer.position.x + (gPlayer.width / 2), gPlayer.position.y + (gPlayer.height / 2), gMouseX, gMouseY);

		//Projectile
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		gProjectile.position.x += (mouse_relative_to_player.x * gFrameTime);
		gProjectile.position.y += (mouse_relative_to_player.y * gFrameTime);
		gProjectile.playerRect = { (int)gProjectile.position.x, (int)gProjectile.position.y, gProjectile.width, gProjectile.height };
		SDL_RenderFillRect(gRenderer, &gProjectile.playerRect);
		
		//Draw everything on the screen
		SDL_RenderPresent(gRenderer);

		// ++ FRAME END ++
		gFrameTime = (SDL_GetTicks() - gLastTime) / 1000.f;
	}
}

void shutdown()
{
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);	
	gRenderer = NULL;
	gWindow = NULL;
	
	SDL_Quit();
}

int main(int argc, char *argv[])
{	
	if (!init())
	{
		printf("Error launching game!\n");
		return 0;
	}
	else
	{	
		refresh();
	}

	shutdown();

	return 0;
}