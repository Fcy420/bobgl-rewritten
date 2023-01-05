#include "FishEntity.h"

FishEntity::FishEntity(Material* mat, glm::vec3 position, World* world) {
	/*float width = 0.1f;
	float length = 0.75f;
	float height = 0.25f;
	float finWidth = 0.01f;
	float finLength = 0.03f;
	std::vector<float> vertices = {
			0,0,0, 0,99, 4, // RIGHT
			length,0,0, 1,99, 4, // RIGHT
			0,height,0, 0,100, 4, // RIGHT
			length,height+0.2f,0, 1,100, 4, // RIGHT
			0,0,width, 0,99, 4, // LEFT
			length,0,width, 1,99, 4, // LEFT
			0,height,width, 0,100, 4, // LEFT
			length,height+0.2f,width, 1,100, 4, // LEFT
			0,height,0, 1,99, 4, // TOP
			0,height,width, 2,99, 4, // TOP
			length,height+0.2f,0, 1,100, 4, // TOP
			length,height+0.2f,width, 2,100, 4, // TOP
			length,0,0, 1,99, 4, // FORWARD
			length,0,width, 2,99, 4, // FORWARD
			length,height+0.2f,0, 1,100, 4, // FORWARD
			length,height+0.2f,width, 2,100, 4, // FORWARD
			0,0,0, 1,99, 4, // BACKWARD
			0,0,width, 2,99, 4, // BACKWARD
			0,height,0, 1,100, 4, // BACKWARD
			0,height,width, 2,100, 4, // BACKWARD
			0,0,0, 1,99, 4, // DOWN
			0,0,width, 2,99, 4, // DOWN
			length,0,0, 1,100, 4, // DOWN
			length,0,width, 2,100, 4, // DOWN
			
	};
	std::vector<unsigned int> triangles = {
		2,1,0,1,2,3,4,5,6,7,6,5,8,9,10,11,10,9,14,13,12,13,14,15,16,17,18,19,18,17,22,21,20,21,22,23,
	};*/
	mesh = EntityMesh::LoadOBJ("Models/fish.obj", mat);
	InitializeEntity(world, FISH);
	BoundingBox box;
	box.min = { -0.1f,-0.1f,-0.1f };
	box.max = { 0.1f,0.1f,0.1f };
	SetBoundingBox(box);
	this->position = position;
}

void FishEntity::Update() {
	double currentFrame = glfwGetTime();
	if (currentFrame >= moveFrame) {
		glm::vec3 randomDir = { 0, sin(currentFrame+randomStartTime)*0.001f , 0};
		Move(randomDir);
	}
	testRot += 0.01f;
	mesh.Draw(position, glm::mat4(1), {1,1,1,1});
}