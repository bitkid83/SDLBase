#ifndef _H_ENTITY
#define _H_ENTITY

#include <iostream>
#include "SDL.h"
#include "vectormath.h"

enum EntityType {
	PLAYER = 0,
	BULLET = 1,
	ENEMY = 2,
	NONE = 999
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
	
	SDL_Rect player_rect;
		
	EntityType GetType();
	void SetType(EntityType t);

	void BindEntity(Entity *e, EntityType t);
	void UnbindEntity(Entity *e);

	void Update(double px, double py, double vx, double vy, int w, int h);
	void Render();

	bool is_bound;

private:
	EntityType eType;	
};

#endif