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
Entity *eDict;
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

	t->posx = (x * TILE_WIDTH - gCamera.x);
	t->posy = (y * TILE_HEIGHT - gCamera.y);

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
		//eDict[0].velocity.y += -1;
		eDict[0].frame_advance = true;
		gCamera.y += -CAM_SPEED * frametime;
	}
	if (gKeyState[SDL_SCANCODE_S]) {
		//eDict[0].velocity.y += 1;
		eDict[0].frame_advance = true;
		gCamera.y += CAM_SPEED * frametime;
	}
	if (gKeyState[SDL_SCANCODE_A]) {
		//eDict[0].velocity.x += -1;		
		eDict[0].frame_advance = true;
		eDict[0].flip_direction = SDL_FLIP_HORIZONTAL;
		gCamera.x += -CAM_SPEED * frametime;
	}
	if (gKeyState[SDL_SCANCODE_D]) {
		//eDict[0].velocity.x += 1;	
		eDict[0].frame_advance = true;
		eDict[0].flip_direction = SDL_FLIP_NONE;
		gCamera.x += CAM_SPEED * frametime;		
	}
	// add velocities to player position
	//eDict[0].position.x += eDict[0].velocity.x * frametime;
	//eDict[0].position.y += eDict[0].velocity.y * frametime;
	//eDict[0].velocity.x *= 0.005;
	//eDict[0].velocity.y *= 0.005;

	//if (fabs(eDict[0].velocity.x) < 0.00001) eDict[0].velocity.x = 0;
	//if (fabs(eDict[0].velocity.y) < 0.00001) eDict[0].velocity.y = 0;

	//printf("(X, Y): %f, %f\n", eDict[0].position.x, eDict[0].position.y);
}

bool init()
{
	bool success = false;

	gWindow = NULL;
	gRenderer = NULL;

	// initialize SDL2
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
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
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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

	// allocate memory for all possible Entities
	eDict = (Entity *)malloc(sizeof(Entity) * MAX_ENTITIES);
	// zero out all Entity list entries
	memset(eDict, 0, sizeof(Entity) * MAX_ENTITIES);

	// set all Entities in the list to type ENT_NONE
	for (int lst = 0; lst < MAX_ENTITIES; lst++) {
		eDict[lst].SetRenderer(gRenderer);
		eDict[lst].SetType(ENT_NONE);
	}

	last_entity = 0;

	// initialize player	
	eDict[0].BindEntity(&eDict[0], ENT_PLAYER, 0);
	eDict[0].Update(	gCamera.x + ((SCREEN_WIDTH / 2) - eDict[0].width), 
						gCamera.y + ((SCREEN_HEIGHT / 2) - eDict[0].height), 
						0, 0, 
						64, 64, 0.f, SDL_FLIP_NONE, false);


	last_entity++;
	
	// set up SDL Game Controllers - not working yet
	gNumGamepads = 0;
	gNumGamepads = SDL_NumJoysticks();

	if (gNumGamepads > 0) {
		printf("Gamepads connected: %d\n", gNumGamepads);
		for (int joy_index = 0; joy_index < gNumGamepads; joy_index++) {
			printf("%s\n", SDL_JoystickNameForIndex(joy_index));
		}

		gJoystick = SDL_JoystickOpen(0);
		if (gJoystick) {
			printf("Buttons: %d\n", SDL_JoystickNumButtons(gJoystick));
		} else {
			printf("Failed to open gamepad 0\n");
		}
	} else {
		printf("No gamepads found! %s\n", SDL_GetError());
	}
	
	// retrieve keyboard state snapshot
	gKeyState = (unsigned char *)SDL_GetKeyboardState(NULL);

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
					// NewGen();
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
		
		frameStart = SDL_GetTicks();

		// clear screen to black
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(gRenderer);
		
		// render the tilemap
		UpdateTiles();

		// mouse crosshair
		// aiming laser
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
		SDL_RenderDrawLine(gRenderer, eDict[0].position.x + (eDict[0].width / 2), eDict[0].position.y + (eDict[0].height / 2), gMouseX, gMouseY);

		SDL_GetMouseState(&gMouseX, &gMouseY);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0xAA, 0xCA, 0xFF);
		SDL_RenderDrawLine(gRenderer, gMouseX - 5, gMouseY, gMouseX + 5, gMouseY);
		SDL_RenderDrawLine(gRenderer, gMouseX, gMouseY - 5, gMouseX, gMouseY + 5);

		// make bullets
		if (shooting && (bullet_timer.GetTicks() % BULLET_RATE == 0)) {
			// set an Entity to BULLET type
			if (last_entity != -1) {
				eDict[last_entity].BindEntity(&eDict[last_entity], ENT_BULLET, 1000);
			}

			// get vector from PLAYER to mouse position
			mousePlayerVector = {
				gMouseX - eDict[0].position.x,
				gMouseY - eDict[0].position.y
			};

			// normalize PLAYER-to-mouse vector
			mousePlayerVector = Unit(mousePlayerVector);

			// initialize bullet starting position
			eDict[last_entity].Update(	eDict[0].position.x + (eDict[0].width / 2),
										eDict[0].position.y + (eDict[0].height / 2),
										mousePlayerVector.x, mousePlayerVector.y,
										10, 10, 
										0.f, 
										SDL_FLIP_NONE, false);
		}

		// entity updates
		HandlePlayerInput(accumulator);		
			
		// loop from the first Entity slot, til the last bound Entity
		for (int idx = 0; idx <= last_entity; idx++) {
			switch (eDict[idx].GetType()) {

			case ENT_PLAYER:
				// PLAYER is always Entity 0
				eDict[0].Update(	eDict[0].position.x, eDict[0].position.y, 
									eDict[0].velocity.x, eDict[0].velocity.y, 
									eDict[0].width, eDict[0].height, 
									eDict[0].rotation_angle, 
									eDict[0].flip_direction, eDict[0].frame_advance);

				eDict[0].Render();
				eDict[0].frame_advance = false;
				//eDict[0].entity_rect = { (int)eDict[0].position.x, (int)eDict[0].position.y, eDict[0].width, eDict[0].height };
				//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				//SDL_RenderFillRect(gRenderer, &eDict[0].entity_rect);
				break;

			case ENT_BULLET:
				//printf("%f, %f\n", eDict[idx].velocity.x, eDict[idx].velocity.y);
				eDict[idx].position.x += eDict[idx].velocity.x * accumulator * 30;
				eDict[idx].position.y += eDict[idx].velocity.y * accumulator * 30;
				eDict[idx].Update(	eDict[idx].position.x, eDict[idx].position.y, 
									eDict[idx].velocity.x, eDict[idx].velocity.y, 
									eDict[idx].width, eDict[idx].height, 
									eDict[idx].rotation_angle, 
									eDict[idx].flip_direction, false);

				eDict[idx].entity_rect = {
					(int)eDict[idx].position.x, 
					(int)eDict[idx].position.y, 
					eDict[idx].width, 
					eDict[idx].height
				};
					
				// todo: move rendering related calls into entity member function
				SDL_SetRenderDrawColor(gRenderer, 0xBB, 0xAA, 0x00, 0xFF);
				SDL_RenderFillRect(gRenderer, &eDict[idx].entity_rect);
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

			if (eDict[last_entity].GetType() == ENT_NONE) {
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

		
		
		// frame timing & rate limiting
		frameEnd = SDL_GetTicks();
		// timing between start of frame and end of frame in milliseconds
		accumulator = (float)(frameEnd - frameStart) / 1000.f;	
		// increment the accumulator if frames are rendering faster than 1 / MAX_FPS
		while (accumulator < dt) {
			accumulator += 0.0001;
		}		
		//printf("frame time: %fms\n", accumulator);	

		// blit to screen
		SDL_RenderPresent(gRenderer);
	}
}

void shutdown()
{
	free((unsigned char *)worldmap);
	worldmap = NULL;

	free((void *)eDict);
	eDict = NULL;

	SDL_JoystickClose(gJoystick);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gJoystick = NULL;
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
		texDirt = new Texture(gRenderer);
		texWater = new Texture(gRenderer);
		texGrass = new Texture(gRenderer);
		texDirt->LoadTexture("dirt.png");
		texWater->LoadTexture("stone.png");
		texGrass->LoadTexture("grass.png");

		refresh();
	}

	delete texDirt; delete texWater; delete texGrass;

	shutdown();

	return 0;
}