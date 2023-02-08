#ifndef CAMERA_H
#define CAMERA_H
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include"BufferObjects/Shader.h"


//Camera class inspired by Victor Gordons tutorial on camera
class Camera {
public:
	void Initialize(int w, int h, glm::vec3 position);
	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader* shader, const char* uniform);
	void Inputs(GLFWwindow* window);
	glm::mat4 currentViewRotation;
	glm::vec3 position;
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	int width;
	int height;
	float sense = 100.0f;
};


#endif