#ifndef ENTITY_H
#define ENTITY_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"../Collision/Collision.h"
#include"../Collision/CollisionManager.h"
#include <glm/glm.hpp>

// All entity types
enum EntityType {
	FISH,
	PLAYER
};
// Base class for a entity
class Entity {
public:
	virtual void Update();
	glm::vec3 GetPosition() {
		return position;
	}
	EntityType GetType() {
		return entityType;
	}
protected:
	virtual void InitializeEntity(World* world, EntityType type);
	virtual void Move(glm::vec3 vector);
	virtual void SetBoundingBox(BoundingBox box);
	glm::vec3 position;
	BoundingBox box;
	ChunkID id;
	Chunk* c;
	World* world;
	EntityType entityType;
	bool isNocliping = false;
};



#endif