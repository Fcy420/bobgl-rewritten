#include "PlayerEntity.h"
// Update the camera and inputs to the main window 
void PlayerEntity::Update(GLFWwindow* window) {
	Inputs(window);
	camera.Matrix(100.0f, 0.1f, 500.0f, shader, "cameraMatrix");
}
// Main movement system for the player
void PlayerEntity::Inputs(GLFWwindow* window) {
	//Hide the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	// Compare current frame to the last to get the delta
	double currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	//If we are moving in the y direction we arent grounded
	float currentYValue = position.y;
	if (lastYValue != position.y) {
		grounded = false;
		lastYValue = currentYValue;
	}
	else {
		grounded = true;
	}
	//Rounded position for block alignment
	BlockPos pos = { std::roundf(position.x), std::roundf(position.y), std::roundf(position.z) };
	isInWater = world->GetBlockAt(pos) == 4;
	float currentSpeed = speed;
	// Multiply speed with 10 if we are sprinting
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		currentSpeed *= 3;
		if (flying) {
			currentSpeed *= 10;
		}
	}
	// Calculate the move direction depending on our input and orientation
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
	// If we arent in water or grounded add gravity to our calculated Y velocity and make sure we dont go over the max gravity
	if (yVelocity > maxGravity*deltaTime && !grounded && !isInWater) {
		yVelocity += gravity * deltaTime;
	}
	// We are grounded and not in water set the gravity a very small number to not stop in the air
	else if (grounded && !isInWater) {
		yVelocity = -0.0001f;
	}
	// Set Y velocity to a high number to jump if we are grounded and the jump cooldown has been reset
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (glfwGetTime() >= nextAvailableJump && grounded) {
			yVelocity = jumpForce;
			nextAvailableJump = glfwGetTime() + 0.1;
		}
	}
	// If we are in water make sure we can swim up and down and sink slowly
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
	// Toggle noclipping
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && currentFrame >= nextFlyingCommand) {
		flying = !flying;
		isNocliping = !isNocliping;
		nextFlyingCommand = currentFrame + 0.2;
	}
	// No gravity if we are flying
	if (flying) {
		yVelocity = 0;
	}
	// Add velocity in direction depending on key press
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && flying) {
		yVelocity = currentSpeed * deltaTime * 5.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && flying) {
		yVelocity = -currentSpeed * deltaTime * 5.0f;
	}
	// Move in the X and Z seperate to not get stuck in walls
	Move(nextPos);
	Move(camera.up * yVelocity);
	// align the camera a little bit up to simulate head
	camera.position = position+camera.up*1.5f;
	//Give the camera inputs
	camera.Inputs(window);
}
// initialize the player class with its shader and world and the width, height of screen for the camera
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