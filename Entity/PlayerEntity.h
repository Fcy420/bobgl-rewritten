#ifndef PLAYERENTITY_H
#define PLAYERENTITY_H
#include"Entity.h"
#include<glad/glad.h>
#include"../Camera.h"
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include"../BufferObjects/Shader.h"
#include"../Collision/Collision.h"

class PlayerEntity : public Entity {
public:
	void Update(GLFWwindow* window);
	PlayerEntity(int w, int h, glm::vec3 position, Shader* shader, World* world);
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