#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H
#include"Collision.h"
#include"../World/World.h"
#include"../World/Chunk.h"
class CollisionManager {
public:
	// Check collisions with a given boundingbox and compare towards to a vector of boundingbox
	static bool IsColliding(BoundingBox boxToCheck, std::vector<BoundingBox> boxes) {
		for (int i = 0; i < boxes.size(); i++) {
			BoundingBox face = boxes[i];
			if (Collision::IsColliding(face,boxToCheck)) {
				return true;
			}
		}
		return false;
	}
	// Not used
	static bool IsCollidingPoint(glm::vec3 a, Chunk* c) {
		std::vector<BoundingBox> boxes = c->GetBoxes();
		glm::vec3 chunkPos = { c->GetID().x * 16.0f, 0.0f, c->GetID().y * 16.0f };
		for (int i = 0; i < boxes.size(); i++) {
			BoundingBox face = boxes.at(i);
			face.min += chunkPos;
			face.max += chunkPos;
			if (Collision::IsCollidingPoint(a, face)) {
				return true;
			}
		}
		return false;
	}
	// not used
	static bool Raycast(glm::vec3 startPos, glm::vec3 direction, float dist, float spacing, Chunk* c) {
		
		return false;
	}
};
#endif