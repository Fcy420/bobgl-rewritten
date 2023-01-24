#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H
#include"Entity.h"
#include<glad/glad.h>
#include"../Camera.h"
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include"../BufferObjects/Shader.h"
#include"../Collision/Collision.h"
// Derive from the entity class
class PlayerEntity : public Entity {
public:
	//Update the object
	void Update(GLFWwindow* window);
	//Init
	PlayerEntity(int w, int h, glm::vec3 position, Shader* shader, World* world);
	//Raycast into the world in the cameras orientation in small steps and check if we crossed a block if the given block is solid stop (Can be optimized with DDA algorithm)
	BlockPos RaycastFromCamera(float maxDist, float stepSize) {
		BlockPos prevPos = { floorf(camera.position.x), floorf(camera.position.y), floorf(camera.position.z) };
		float t = 0.0f;
		while (t < maxDist) {
			glm::vec3 pos = camera.position + camera.orientation * t;
			BlockPos bPos = { floorf(pos.x), floorf(pos.y), floorf(pos.z) };
			t += stepSize;
			if (prevPos.x == bPos.x && prevPos.y == bPos.y && prevPos.z == bPos.z) {
				continue;
			}
			prevPos = bPos;
			int blockType = world->GetBlockAt(bPos);
			if (blockType > 0 && blockType != 4) {
				return bPos;
			}
		}
		return prevPos;
	}
	glm::vec3 GetRotation() {
		return camera.orientation;
	}
	glm::mat4 GetViewRotation() {
		return camera.currentViewRotation;
	}
private:
	double lastFrame = glfwGetTime();
	double nextAvailableJump = 0.0;
	double nextFlyingCommand = 0.0;
	float yVelocity = 0.0f;
	float gravity = -0.1f;
	float maxGravity = -13.0f;
	float jumpForce = 0.05f;
	float speed = 3.0f;
	float lastYValue;
	bool grounded = false;
	bool isInWater = false;
	bool flying = false;
	void Inputs(GLFWwindow* window);
	Camera camera;
	Shader* shader;
};

#endif