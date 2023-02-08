#include"Camera.h"

void Camera::Initialize(int width, int height, glm::vec3 position) {
	this->width = width;
	this->height = height;
	this->position = position;
}
// Camera view inspired from https://github.com/VictorGordan/opengl-tutorials/blob/main/YoutubeOpenGL%208%20-%20Camera/Camera.cpp
void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader* shader, const char* uniform) {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	view = glm::lookAt(position, position + orientation, up);
	proj = glm::perspective(glm::radians(FOVdeg), (float)(width/height), nearPlane, farPlane);
	//Set the current view matrix so it can be used on the CPU side
	currentViewRotation = view * proj;
	glUniformMatrix4fv(glGetUniformLocation(shader->id, uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
	glUniform3fv(glGetUniformLocation(shader->id, "eyePosition"), 1, glm::value_ptr(position));
}
#define TAU (M_PI * 2.0)
void Camera::Inputs(GLFWwindow* window) {
	double mouseX;
	double mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	//Get the delta of mouse movements and use magic to make it to rotation and look at point
	float rotationX = sense * (float)(mouseY - (height / 2)) / height;
	float rotationY = sense * (float)(mouseX - (width / 2)) / width;
	glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotationX), glm::normalize(glm::cross(orientation, up)));
	if (!((glm::angle(newOrientation, up) <= glm::radians(10.0f) or glm::angle(newOrientation, -up) <= glm::radians(10.0f)))) {
		orientation = newOrientation;
		orientation = glm::rotate(orientation, glm::radians(-rotationY), up);
	}
	//Reset the cursor to the center of the screen
	glfwSetCursorPos(window, (width / 2), (height / 2));
}