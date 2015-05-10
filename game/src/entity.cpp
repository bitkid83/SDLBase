#include <stdio.h>
#include "entity.h"

Entity::Entity()
{
	
};

Entity::~Entity()
{

};

void Entity::SetType(EntityType t)
{
	this->eType = t;
};

EntityType Entity::GetType()
{
	return this->eType;
}

void Entity::BindEntity(Entity *e, EntityType t)
{
	if (!e->is_bound) {
		e->SetType(t);
		e->is_bound = true;
	}
	else {
		char *typestr = "";
		switch (e->GetType())
		{
			case PLAYER:
				typestr = "PLAYER";
				break;
			case BULLET:
				typestr = "BULLET";
				break;
			case ENEMY:
				typestr = "ENEMY";
				break;
			case NONE:
				typestr = "NONE";
				break;
		}
		printf("Error: entity already bound to type: %s\n", typestr);
		return;
	}
};

void Entity::UnbindEntity(Entity *e)
{
	if (e != NULL) {
		if (e->is_bound == true) {
			e->SetType(NONE);
			e->is_bound = false;
		}
		else {
			printf("Error: entity not actually bound!\n");
			return;
		}
	}
	else {
		printf("Error: entity is null!\n");
		return;
	}
};

void Entity::Update(double px, double py, double vx, double vy, int w, int h)
{
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