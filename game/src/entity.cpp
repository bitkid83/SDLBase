#include <stdio.h>
#include "entity.h"

Entity::Entity()
{
	
};

Entity::~Entity()
{

};

void Entity::SetId(int Id)
{
	this->entity_id = Id;
};

int Entity::GetId()
{
	return this->entity_id;
}

void Entity::BindEntity(Entity *e, int Id)
{
	if (!e->is_bound) {
		e->SetId(Id);
		e->is_bound = true;
	}
	else {
		printf("Error: entity already bound with Id: %d\n", e->GetId());
		return;
	}
};

void Entity::UnbindEntity(Entity *e)
{
	if (e != NULL) {
		if (e->is_bound == true) {
			e->SetId(999);
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