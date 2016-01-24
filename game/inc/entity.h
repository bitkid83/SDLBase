#ifndef _H_ENTITY
#define _H_ENTITY

#include <iostream>
#include "SDL.h"
#include "contentpipeline.h"
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

	double rotation_angle;

	SDL_RendererFlip flip_direction;

	bool frame_advance;

	int spawnTime;
	int fadeTime;

	SDL_Rect entity_rect;	
		
	EntityType GetType();
	void SetRenderer(SDL_Renderer *ren);
	void SetType(EntityType t);

	void BindEntity(Entity *e, EntityType t, int fade);
	void UnbindEntity(Entity *e);
		
	void Update(double px, double py, double vx, double vy, int w, int h, double rotation, SDL_RendererFlip flip, bool frame_step); // re-factor to use vec2 instead of doubles
	void Render();

protected:
	SDL_Renderer *eRenderer;
	SpriteSheet *entSprite;

private:
	EntityType eType;

};

#endif