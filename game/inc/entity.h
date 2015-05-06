#ifndef _H_ENTITY
#define _H_ENTITY

#include "SDL.h"
#include "vectormath.h"

class Entity
{

public:
	Entity();
	~Entity();

	vec2 position;
	vec2 velocity;

	int width;		
	int height;

	SDL_Rect playerRect;
		
	int GetId();

	void BindEntity(Entity *e, int Id);
	void UnbindEntity(Entity *e);

	bool is_bound;

private:	
	// Entity Id Table:
	// Player = 0
	// Bullet = 25
	// Nothing = 999
	int entity_id;	

	void SetId(int Id);
};

#endif