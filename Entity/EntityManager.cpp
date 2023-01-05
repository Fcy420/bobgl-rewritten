#include "EntityManager.h"

EntityManager::EntityManager(World* world, Material* mat) {
	this->world = world;
	this->mat = mat;
}
void EntityManager::Update() {
	for(int i = 0; i < 300; i++) {
		if (entities.size() >= 50) {
			break;
		}
		BlockPos pos = { rand() % 500 - 25, rand() % 60, rand() % 500 - 25 };
		if (world->GetBlockAt(pos) == 4) {
			glm::vec3 spawnPos = { pos.x + .5f,pos.y,pos.z + .5f };
			FishEntity* fish = new FishEntity(mat, spawnPos, world);
			entities.emplace_back(fish);
		}
	}
	for (int i = 0; i < entities.size(); i++) {
		entities[i]->Update();
	}
}