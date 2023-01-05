#ifndef FISHENTITY_H
#define FISHENTITY_H
#include"../Entity.h"
#include"../../BufferObjects/EntityMesh.h"
#include <iostream>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/glm.hpp>

class FishEntity : public Entity {
public:
	FishEntity(Material* mat, glm::vec3 position, World* world);
	virtual void Update();
private:
	int randomStartTime = rand() % 10000;
	float testRot = 0.0f;
	double moveFrame = 0;
	EntityMesh mesh;
};

#endif
