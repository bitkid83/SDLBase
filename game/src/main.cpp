#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "SDL_image.h"

#include "main.h"
#include "contentpipeline.h"
#include "entity.h"
#include "timer.h"

Texture *texDirt;
Texture *texGrass;
Texture *texWater;

// Entity dictionary
Entity *edict;
int last_entity;

// Frame timing stuff
const float maxfps = 100;
const float dt = 1 / maxfps;
float accumulator = 0;

vec2 gCamera;


void GenTiles()
{
	worldmap = (unsigned char*)malloc(MAX_TILES);
	
	for (int i = 0; i < MAX_TILES; i++) {
		worldmap[i] = rand() % 3;
	}		
}

void NewGen()
{
	for (int i = 0; i < MAX_TILES; i++) {
		worldmap[i] = rand() % 3;
	}
	
	for (int gen = 0; gen < MAX_TILES; gen++) {
		if (worldmap[gen] == 1) {
			for (int g = 0; g < 50; g++) {
				srand(SDL_GetTicks());
				worldmap[gen] = rand() % 3;
			}
		}
	}
}

//SDL_Rect TileIndex(int x, int y)
Texture *TileIndex(int x, int y)
{
	int index;
	//SDL_Rect temp;

	Texture *t = NULL;

	//temp.w = TILE_WIDTH;
	//temp.h = TILE_HEIGHT;

	index = x + (y * TILE_WIDTH);

	// Tile Types (0, 1, 2)
	if (worldmap[index] == 0) {
		t = texGrass;
		//SDL_SetRenderDrawColor(gRenderer, 0x0F, 0x3B, 0x58, 0xFF);
	}
	else if (worldmap[index] == 1) {
		t = texDirt;
		//SDL_SetRenderDrawColor(gRenderer, 0x00, 0x92, 0xBF, 0xFF);
	}
	else if (worldmap[index] == 2) {
		t = texWater;
		//SDL_SetRenderDrawColor(gRenderer, 0x1F, 0x0B, 0x36, 0xFF);
	}
	
	// todo: get tile position updating out of this function
	//temp.x = x * TILE_WIDTH - gCamera.x;
	//temp.y = y * TILE_HEIGHT - gCamera.y;

	t->posx = x * TILE_WIDTH - gCamera.x;
	t->posy = y * TILE_HEIGHT - gCamera.y;

	return t;
}

void UpdateTiles()
{
	// todo: allocation all the memory for map's assets up front
	Texture *tileTexture;

	//SDL_Rect *temprect;
	//temprect = (SDL_Rect*)malloc(sizeof(SDL_Rect));

	for (int y = 0; y < WORLD_WIDTH; y++) {
		for (int x = 0; x < WORLD_HEIGHT; x++) {			
			//*temprect = TileIndex(x, y);		
			//SDL_RenderFillRect(gRenderer, temprect);
			tileTexture = TileIndex(x, y);
			tileTexture->RenderTexture(tileTexture->posx, tileTexture->posy);
		}
	}

	//free(temprect);
}

void HandlePlayerInput(float frametime)
{
	//SDL_PumpEvents();
	if (gKeyState[SDL_SCANCODE_W]) {
		//edict[0].velocity.y += -1;
		gCamera.y += -CAM_SPEED * frametime;
	}
	if (gKeyState[SDL_SCANCODE_S]) {
		//edict[0].velocity.y += 1;
		gCamera.y += CAM_SPEED * frametime;
	}
	if (gKeyState[SDL_SCANCODE_A]) {
		//edict[0].velocity.x += -1;
		gCamera.x += -CAM_SPEED * frametime;
	}
	if (gKeyState[SDL_SCANCODE_D]) {
		//edict[0].velocity.x += 1;
		gCamera.x += CAM_SPEED * frametime;
	}

	// add velocities to player position
	//edict[0].position.x += edict[0].velocity.x * frametime;
	//edict[0].position.y += edict[0].velocity.y * frametime;
	//edict[0].velocity.x *= 0.005;
	//edict[0].velocity.y *= 0.005;

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

		if (IMG_Init(IMG_INIT_PNG) == 0) {
			printf("Error initializing SDL Image: %s\n", IMG_GetError());
			success = false;
		}

		success = true;
	}

	// init Camera
	gCamera.x = 0;
	gCamera.y = 0;

	// allocate memory for all possible entities
	edict = (Entity *)malloc(sizeof(Entity) * MAX_ENTITIES);
	memset(edict, 0, sizeof(Entity) * MAX_ENTITIES);

	for (int lst = 0; lst < MAX_ENTITIES; lst++) {
		edict[lst].SetType(ENT_NONE);
	}

	last_entity = 0;

	// initialize player	
	edict[0].BindEntity(&edict[0], ENT_PLAYER, 0);
	edict[0].Update(	gCamera.x + ((SCREEN_WIDTH / 2) - edict[0].width), 
						gCamera.y + ((SCREEN_HEIGHT / 2) - edict[0].height), 
						0, 0, 
						32, 32	);


	last_entity++;
	
	// set up SDL Game Controllers - not working yet
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

	Uint32 frameStart = 0;
	Uint32 frameEnd = 0;

	bool shooting = false;
	int start = 0;
	
	Timer bullet_timer;
	
	// Game loop
	while (!done) {
		frameStart = SDL_GetTicks();	

		// SDL Event loop
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {

			case SDL_QUIT:
				done = true;
				break;

			case SDL_MOUSEBUTTONDOWN:
				// Left Mouse Button
				if (evt.button.button == SDL_BUTTON_LEFT) {
					shooting = true;
					bullet_timer.Start();
				}
				
				// Right Mouse Button
				else if (evt.button.button == SDL_BUTTON_RIGHT) {					
					NewGen();
				}
				break;

			case SDL_MOUSEBUTTONUP:
				shooting = false;
				bullet_timer.Stop();
				break;

			case SDL_MOUSEMOTION:
				break;

			case SDL_KEYUP:
				// ESC key quits
				if (evt.key.keysym.sym == SDLK_ESCAPE) {
					done = true;
				}
				break;
			}
		}

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

		// make bullets
		if (shooting && (bullet_timer.GetTicks() % BULLET_RATE == 0)) {
			// set an Entity to BULLET type
			if (last_entity != -1) {
				edict[last_entity].BindEntity(&edict[last_entity], ENT_BULLET, 1000);
			}

			// get vector from PLAYER to mouse position
			mousePlayerVector = {
				gMouseX - edict[0].position.x,
				gMouseY - edict[0].position.y
			};

			// normalize PLAYER-to-mouse vector
			mousePlayerVector = Unit(mousePlayerVector);

			// initialize bullet starting position
			edict[last_entity].Update(edict[0].position.x + (edict[0].width / 2),
				edict[0].position.y + (edict[0].height / 2),
				mousePlayerVector.x,
				mousePlayerVector.y,
				10, 10);
		}

		// entity updates
		HandlePlayerInput(accumulator);
			
		// loop from the first Entity slot, til the last bound Entity
		for (int idx = 0; idx <= last_entity; idx++) {
			switch (edict[idx].GetType()) {

			case ENT_PLAYER:
				// PLAYER is always Entity 0
				edict[0].Update(edict[0].position.x, edict[0].position.y, edict[0].velocity.x, edict[0].velocity.y, edict[0].width, edict[0].height);
				edict[0].player_rect = { (int)edict[0].position.x, (int)edict[0].position.y, edict[0].width, edict[0].height };
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderFillRect(gRenderer, &edict[0].player_rect);
				break;

			case ENT_BULLET:
				edict[idx].position.x += edict[idx].velocity.x; //*accumulator;
				edict[idx].position.y += edict[idx].velocity.y; //*accumulator;
				edict[idx].Update(edict[idx].position.x, edict[idx].position.y, edict[idx].velocity.x, edict[idx].velocity.y, edict[idx].width, edict[idx].height);

				edict[idx].player_rect = {
					(int)edict[idx].position.x, 
					(int)edict[idx].position.y, 
					edict[idx].width, 
					edict[idx].height
				};
					
				// todo: move rendering related calls into entity member function
				SDL_SetRenderDrawColor(gRenderer, 0xBB, 0xAA, 0x00, 0xFF);
				SDL_RenderFillRect(gRenderer, &edict[idx].player_rect);
				break;
			}
		}

		// entity slot check loop
		if (last_entity == -1) {
			last_entity = 1;
		}
		else {
			start = last_entity;
		}
		
		for(;;) {
			if (last_entity == MAX_ENTITIES) { // want this inside incrementing loop
				last_entity = 1;
			}

			if (edict[last_entity].GetType() == ENT_NONE) {
				break;  // break from loop, we found a free slot
			}

			// slot was used, go to next one
			last_entity++;

			// have we gone full circle from where last_entity started ?
			if (last_entity == start) {
				last_entity = -1;       // signifies no free slots
				break;
			}
		}

		// aiming laser
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_RenderDrawLine(gRenderer, edict[0].position.x + (edict[0].width / 2), edict[0].position.y + (edict[0].height / 2), gMouseX, gMouseY);
		
		// frame timing & rate limiting
		frameEnd = SDL_GetTicks();
		accumulator = (frameEnd - frameStart) / 1000.f;

		while (accumulator < dt) {
			accumulator += dt;
		}		

		// blit to screen
		SDL_RenderPresent(gRenderer);
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
		texDirt = new Texture(gRenderer);
		texWater = new Texture(gRenderer);
		texGrass = new Texture(gRenderer);
		texDirt->LoadTexture("dirt.png");
		texWater->LoadTexture("water.png");
		texGrass->LoadTexture("grass.png");

		refresh();
	}

	delete texDirt; delete texWater; delete texGrass;

	shutdown();

	return 0;
}