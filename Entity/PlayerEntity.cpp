#include "PlayerEntity.h"

void PlayerEntity::Update(GLFWwindow* window) {
	Inputs(window);
	camera.Matrix(100.0f, 0.1f, 500.0f, shader, "cameraMatrix");
}
void PlayerEntity::Inputs(GLFWwindow* window) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	double currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float currentYValue = position.y;
	if (lastYValue != position.y) {
		grounded = false;
		lastYValue = currentYValue;
	}
	else {
		grounded = true;
	}
	BlockPos pos = { std::roundf(position.x), std::roundf(position.y), std::roundf(position.z) };
	isInWater = world->GetBlockAt(pos) == 4;
	float currentSpeed = speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		currentSpeed *= 3;
		if (flying) {
			currentSpeed *= 10;
		}
	}
	glm::vec3 nextPos = { 0,0,0 };
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		nextPos += currentSpeed * deltaTime * glm::normalize(glm::vec3(camera.orientation.x, 0.0f, camera.orientation.z));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		nextPos -= currentSpeed * deltaTime * glm::normalize(glm::vec3(camera.orientation.x, 0.0f, camera.orientation.z));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		nextPos += currentSpeed * deltaTime * glm::normalize(glm::cross(camera.orientation, camera.up));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		nextPos -= currentSpeed * deltaTime * glm::normalize(glm::cross(camera.orientation, camera.up));
	}
	if (yVelocity > maxGravity*deltaTime && !grounded && !isInWater) {
		yVelocity += gravity * deltaTime;
	}
	else if (grounded && !isInWater) {
		yVelocity = -0.0001f;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (glfwGetTime() >= nextAvailableJump && grounded) {
			yVelocity = jumpForce;
			nextAvailableJump = glfwGetTime() + 0.1;
		}
	}
	if (isInWater) {
		if (yVelocity < -0.01f) yVelocity = -0.01f;
		if (yVelocity > 0.04f) yVelocity = 0.04f;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			yVelocity += 0.3f * deltaTime;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			yVelocity -= 0.01f * deltaTime;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && currentFrame >= nextFlyingCommand) {
		flying = !flying;
		isNocliping = !isNocliping;
		nextFlyingCommand = currentFrame + 0.2;
	}
	if (flying) {
		yVelocity = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && flying) {
		yVelocity = currentSpeed * deltaTime * 5.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && flying) {
		yVelocity = -currentSpeed * deltaTime * 5.0f;
	}
	Move(nextPos);
	Move(camera.up * yVelocity);
	camera.position = position+camera.up*1.5f;
	camera.Inputs(window);
}

PlayerEntity::PlayerEntity(int w, int h, glm::vec3 position, Shader* shader, World* world) {
	camera.Initialize(w, h, position);
	BoundingBox box;
	box.max = { 0.3f,1.5f,0.3f };
	box.min = { -0.3f,-0.5f,-0.3f };
	InitializeEntity(world, PLAYER);
	SetBoundingBox(box);
	this->position = position;
	this->shader = shader;
}