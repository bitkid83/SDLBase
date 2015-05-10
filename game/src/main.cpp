#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "SDL_image.h"
#include "main.h"
#include "entity.h"


// Entity dictionary
Entity *edict;
int last_entity;

// Frame timing stuff
const float maxfps = 60;
const float dt = 1 / maxfps;
float accumulator = 0;

vec2 gCamera;

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

	index = x + (y * TILE_WIDTH);

	// Tile Types (0, 1, 2)
	if (worldmap[index] == 0) {
		SDL_SetRenderDrawColor(gRenderer, 0x0F, 0x3B, 0x58, 0xFF);
	}
	else if (worldmap[index] == 1) {
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x92, 0xBF, 0xFF);
	}
	else if (worldmap[index] == 2) {
		SDL_SetRenderDrawColor(gRenderer, 0x1F, 0x0B, 0x36, 0xFF);
	}
	
	temp.x = x * TILE_WIDTH - gCamera.x;
	temp.y = y * TILE_HEIGHT - gCamera.y;

	return temp;
}

void UpdateTiles()
{
	SDL_Rect *temprect;
	temprect = (SDL_Rect*)malloc(sizeof(SDL_Rect));

	for (int y = 0; y < WORLD_WIDTH; y++) {
		for (int x = 0; x < WORLD_HEIGHT; x++) {			
			*temprect = TileIndex(x, y);		
			SDL_RenderFillRect(gRenderer, temprect);
		}
	}

	free(temprect);
}

void HandlePlayerInput(float frametime)
{
	//SDL_PumpEvents();
	if (gKeyState[SDL_SCANCODE_W]) {
		edict[0].velocity.y += -1;
		gCamera.y += -5;
	}
	if (gKeyState[SDL_SCANCODE_S]) {
		edict[0].velocity.y += 1;
		gCamera.y += 5;
	}
	if (gKeyState[SDL_SCANCODE_A]) {
		edict[0].velocity.x += -1;
		gCamera.x += -5;
	}
	if (gKeyState[SDL_SCANCODE_D]) {
		edict[0].velocity.x += 1;
		gCamera.x += 5;
	}

	// add velocities to player position
	edict[0].position.x += edict[0].velocity.x * frametime;
	edict[0].position.y += edict[0].velocity.y * frametime;
	edict[0].velocity.x *= 0.05;
	edict[0].velocity.y *= 0.05;

	//if (fabs(edict[0].velocity.x) < 0.00001) edict[0].velocity.x = 0;
	//if (fabs(edict[0].velocity.y) < 0.00001) edict[0].velocity.y = 0;

	//printf("(X, Y): %f, %f\n", edict[0].position.x, edict[0].position.y);
}

bool init()
{
	bool success = false;

	gWindow = NULL;
	gRenderer = NULL;

	// initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Error initializing SDL: %s\n", SDL_GetError());
		success = false;
	}
	// create SDL window
	else {
		gWindow = SDL_CreateWindow("SDL Base",
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

	// init Camera
	gCamera.x = 0;
	gCamera.y = 0;

	// allocate memory for all possible entities
	edict = (Entity *)malloc(sizeof(Entity) * MAX_ENTITIES);
	memset(edict, 0, sizeof(Entity) * MAX_ENTITIES);

	last_entity = 0;

	// initialize player	
	edict[0].BindEntity(&edict[0], PLAYER);
	edict[0].Update(	gCamera.x + ((SCREEN_WIDTH / 2) - edict[0].width), 
						gCamera.y + ((SCREEN_HEIGHT / 2) - edict[0].height), 
						0, 0, 
						32, 32	);

	last_entity++;

	// set up SDL Game Controllers
	gNumGamepads = 0;
	gNumGamepads = SDL_NumJoysticks();
	if (gNumGamepads > 0) {
		printf("Gamepads connected: %d\n", gNumGamepads);
	}
	else {
		printf("No gamepads found! %s\n", SDL_GetError());
	}
	
	// retrieve keyboard state snapshot
	gKeyState = (unsigned char*)SDL_GetKeyboardState(NULL);

	// disable mouse cursor
	SDL_ShowCursor(SDL_DISABLE);

	SDL_RenderClear(gRenderer);

	GenTiles();

	return success;
}

void refresh()
{
	bool done = false;	

	SDL_Event evt;
	vec2 mousePlayerVector = { 0, 0 };

	float frameStart = SDL_GetTicks();

	// Game loop
	while (!done) {
		const float currentTime = SDL_GetTicks();
		// store time elapsed since the last frame began
		accumulator += currentTime - frameStart;
		// record the starting of this frame
		frameStart = currentTime;

		if (accumulator > 0.2f) {
			accumulator = 0.2f;
		}
		// SDL_PumpEvents(); //not needed, SDL_PollEvent already pumps events
			
		// SDL Event loop
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {

			case SDL_QUIT:
				done = true;
				break;

			case SDL_MOUSEBUTTONDOWN:
				// Left Mouse Button
				if (evt.button.button == SDL_BUTTON_LEFT) {					
					// set an Entity to BULLET type
					edict[last_entity].BindEntity(&edict[last_entity], BULLET);

					// get vector from PLAYER to mouse position
					mousePlayerVector = { 
						evt.button.x - edict[0].position.x,
						evt.button.y - edict[0].position.y 
					};
					// normalize PLAYER-to-mouse vector
					mousePlayerVector = Vec2Normalize(mousePlayerVector);

					// initialize bullet starting position
					edict[last_entity].Update(	edict[0].position.x + (edict[0].width / 2), 
												edict[0].position.y + (edict[0].height / 2), 
												mousePlayerVector.x, 
												mousePlayerVector.y, 
												10, 10);
				}
				// Right Mouse Button
				else if (evt.button.button == SDL_BUTTON_RIGHT) {
					//nothing here yet!
				}
				break;

			case SDL_KEYUP:
				// ESC key quits
				if (evt.key.keysym.sym == SDLK_ESCAPE) {
					done = true;
				}
				break;

			}
		}

		while (accumulator > dt) {
			accumulator -= dt;

			// clear screen to black
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(gRenderer);
		
			// render the tilemap
			UpdateTiles();

			// mouse crosshair
			SDL_GetMouseState(&gMouseX, &gMouseY);
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0xAA, 0xCA, 0xFF);
			SDL_RenderDrawLine(gRenderer, gMouseX - 5, gMouseY, gMouseX + 5, gMouseY);
			SDL_RenderDrawLine(gRenderer, gMouseX, gMouseY - 5, gMouseX, gMouseY + 5);

			// aiming laser
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
			SDL_RenderDrawLine(gRenderer, edict[0].position.x + (edict[0].width / 2), edict[0].position.y + (edict[0].height / 2), gMouseX, gMouseY);

			// entity updates
			HandlePlayerInput(dt);
			
			// loop from the first Entity slot, til the last bound Entity
			for (int idx = 1; idx <= last_entity; idx++) {
				switch (edict[idx].GetType()) {

				case PLAYER:
					// PLAYER is always Entity 0
					// edict[0].Update(edict[0].position.x, edict[0].position.y, edict[0].velocity.x, edict[0].velocity.y, edict[0].width, edict[0].height);
					//edict[0].player_rect = { (int)edict[0].position.x, (int)edict[0].position.y, edict[0].width, edict[0].height };
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderFillRect(gRenderer, &edict[0].player_rect);
					break;

				case BULLET:
					edict[idx].position.x += edict[idx].velocity.x * 2; // *gFrameTime
					edict[idx].position.y += edict[idx].velocity.y * 2; // *gFrameTime
					
					edict[idx].player_rect = {
						(int)edict[idx].position.x, 
						(int)edict[idx].position.y, 
						edict[idx].width, 
						edict[idx].height
					};
					
					// todo: move rendering related calls into entity member function
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xAA, 0x00, 0xFF);
					SDL_RenderFillRect(gRenderer, &edict[idx].player_rect);
					break;
				}
			}

			// if Entity is bound, increment the array count
			if (last_entity == MAX_ENTITIES) {
				last_entity = 1;
			}
			else {
				while (edict[last_entity].is_bound == true) {
					last_entity++;
				}
			}			
			
			SDL_RenderPresent(gRenderer);			
		}
		
		// ++ FRAME END ++
		//gFrameTime = (SDL_GetTicks() - gLastTime) / 1000.f;		
	}
}

void shutdown()
{
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;
	
	free((void *)edict);
	edict = NULL;

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