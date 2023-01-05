#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "Entity.h"
#include"EntityTypes/FishEntity.h"
#include"../World/World.h"
#include <memory>

class EntityManager {
public:
	EntityManager(World* world, Material* mat);
	void Update();
private:
	std::vector<std::unique_ptr<Entity>> entities;
	World* world;
	Material* mat;
};
#endif