#include "Entity.h"
// Base class for all entities
void Entity::Update() {
	//std::cout << "Base Entity\n";
}
// Base initialize and assign the starting chunk id to type (Not used)
void Entity::InitializeEntity(World* world, EntityType type) {
	this->world = world;
	ChunkID cid = { floorf(position.x / 16.0f), floorf(position.z / 16.0f) };
	id = cid;
	entityType = type;
}
// Move the entity with checks for collision and update the chunkid to use the according chunks bounding boxes
void Entity::Move(glm::vec3 vector) {
	glm::vec3 dest = position + vector;
	//Collision
	bool isColliding = false;
	// Align the entitys bounding box to its position
	BoundingBox posb;
	posb.min = box.min + dest;
	posb.max = box.max + dest;
	//Check the destinations chunkID
	ChunkID destId = { floorf(dest.x / 16.0f), floorf(dest.z / 16.0f) };
	// Only check for collisions if we are not noclipping
	if (!isNocliping) {
		// Get chunks in a radius of 2 since if we are at a chunk border we do not want to clip through
		std::vector<Chunk*> chunks = world->GetChunksNear(destId, 2);
		//Check each chunks bounding boxes
		for (auto chunk : chunks) {
			std::vector<BoundingBox> chunkCollider = chunk->GetBoxes();
			isColliding = CollisionManager::IsColliding(posb, chunkCollider);
			if (isColliding)break;
		}
	}
	// if we are colliding don't move in the given vector
	if (!isColliding) {
		position = dest;
 	}
}
void Entity::SetBoundingBox(BoundingBox a) {
	box = a;
}