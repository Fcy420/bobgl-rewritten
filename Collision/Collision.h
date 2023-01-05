#ifndef COLLISION_H
#define COLLISION_H
#include <glm/glm.hpp>

static bool operator >(glm::vec3& a, glm::vec3& b) {
	return a.x > b.x && a.y > b.y && a.z > b.z;
}
static bool operator <(glm::vec3& a, glm::vec3& b) {
	return a.x < b.x && a.y < b.y && a.z < b.z;
}
static bool operator >=(glm::vec3& a, glm::vec3& b) {
	return a.x >= b.x && a.y >= b.y && a.z >= b.z;
}
static bool operator <=(glm::vec3& a, glm::vec3& b) {
	return a.x <= b.x && a.y <= b.y && a.z <= b.z;
}

typedef struct {
	glm::vec3 min;
	glm::vec3 max;
} BoundingBox;
class Collision {
public:
	// Danke https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
	static bool IsColliding(BoundingBox a, BoundingBox b) {
		return a.min <= b.max && a.max >= b.min;
	}
	static bool IsCollidingPoint(glm::vec3 a, BoundingBox b) {
		return a >= b.min && a <= b.max;
	}
};

#endif