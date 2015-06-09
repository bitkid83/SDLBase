#ifndef _H_ENTITY
#define _H_ENTITY

#include <iostream>
#include "SDL.h"
#include "timer.h"
#include "vectormath.h"

enum EntityType {
	ENT_PLAYER = 0,
	ENT_BULLET = 1,
	ENT_ENEMY = 2,
	ENT_NONE = -1
};

class Entity
{

public:
	Entity();
	~Entity();

	int width;
	int height;

	vec2 position;
	vec2 velocity;

	int spawnTime;
	int fadeTime;

	SDL_Rect player_rect;
		
	EntityType GetType();
	void SetType(EntityType t);

	void BindEntity(Entity *e, EntityType t, int fade);
	void UnbindEntity(Entity *e);

	void Update(double px, double py, double vx, double vy, int w, int h);
	void Render();

private:
	EntityType eType;	
};

#endif