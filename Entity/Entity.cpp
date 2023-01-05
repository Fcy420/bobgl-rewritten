#include "Entity.h"
void Entity::Update() {
	//std::cout << "Base Entity\n";
}
void Entity::InitializeEntity(World* world, EntityType type) {
	this->world = world;
	ChunkID cid = { floorf(position.x / 16.0f), floorf(position.z / 16.0f) };
	id = cid;
	entityType = type;
}
void Entity::Move(glm::vec3 vector) {
	glm::vec3 dest = position + vector;
	//Collision
	bool isColliding = false;
	BoundingBox posb;
	posb.min = box.min + dest;
	posb.max = box.max + dest;
	ChunkID destId = { floorf(dest.x / 16.0f), floorf(dest.z / 16.0f) };
	if (!isNocliping) {
		std::vector<Chunk*> chunks = world->GetChunksNear(destId, 2);
		for (auto chunk : chunks) {
			std::vector<BoundingBox> chunkCollider = chunk->GetBoxes();
			isColliding = CollisionManager::IsColliding(posb, chunkCollider);
			if (isColliding)break;
		}
	}
	if (!isColliding) {
		position = dest;
 	}
}
void Entity::SetBoundingBox(BoundingBox a) {
	box = a;
}