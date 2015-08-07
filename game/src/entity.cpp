#include <stdio.h>
#include "entity.h"

Entity::Entity() { };

Entity::~Entity() { };

void Entity::SetType(EntityType t)
{
	this->eType = t;
};

EntityType Entity::GetType()
{
	return this->eType;
}

void Entity::BindEntity(Entity *e, EntityType t, int fade)
{
	if (e->GetType() == ENT_NONE) {
		e->SetType(t);

		this->fadeTime = fade;
		this->spawnTime = SDL_GetTicks();
	} 
	else {
		char *typestr = "";
		switch (e->GetType())
		{
			case ENT_PLAYER:
				typestr = "PLAYER";
				break;
			case ENT_BULLET:
				typestr = "BULLET";
				break;
			case ENT_ENEMY:
				typestr = "ENEMY";
				break;
		}

		printf("Error: entity already bound to type: %s\n", typestr);
		return;
	}
};

void Entity::UnbindEntity(Entity *e)
{
	if (e->GetType() == ENT_NONE) {
		printf("Entity already null!\n");
	}
	else {
		e->SetType(ENT_NONE);
	}
};

void Entity::Update(double px, double py, double vx, double vy, int w, int h)
{
	if (this->fadeTime != 0) {
		if (SDL_GetTicks() - this->spawnTime >= this->fadeTime) {
			this->UnbindEntity(this);
		}
	}
	
	if (w != 0 && h != 0) {
		this->width = w;
		this->height = h;
	}

	this->position.x = px;
	this->position.y = py;

	this->velocity.x = vx;
	this->velocity.y = vy;

	this->player_rect.w = this->width;
	this->player_rect.h = this->height;

	this->player_rect.x = (int)this->position.x;
	this->player_rect.y = (int)this->position.y;
};

void Entity::Render()
{

};