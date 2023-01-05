#include"Camera.h"

void Camera::Initialize(int w, int h, glm::vec3 position) {
	this->w = w;
	this->h = h;
	this->position = position;
}
void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader* shader, const char* uniform) {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	view = glm::lookAt(position, position + orientation, up);
	proj = glm::perspective(glm::radians(FOVdeg), (float)(w/h), nearPlane, farPlane);
	currentViewRotation = view * proj;
	glUniformMatrix4fv(glGetUniformLocation(shader->id, uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
	glUniform3fv(glGetUniformLocation(shader->id, "eyePosition"), 1, glm::value_ptr(position));
}
#define TAU (M_PI * 2.0)
void Camera::Inputs(GLFWwindow* window) {
	double mouseX;
	double mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	float rotx = sense * (float)(mouseY - (h / 2)) / h;
	float roty = sense * (float)(mouseX - (w / 2)) / w;
	glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotx), glm::normalize(glm::cross(orientation, up)));
	if (!((glm::angle(newOrientation, up) <= glm::radians(10.0f) or glm::angle(newOrientation, -up) <= glm::radians(10.0f)))) {
		orientation = newOrientation;
		orientation = glm::rotate(orientation, glm::radians(-roty), up);
	}
	glfwSetCursorPos(window, (w / 2), (h / 2));
}