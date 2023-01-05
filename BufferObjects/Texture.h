#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include"Shader.h"

class Texture {
public:
	void Create(const char* fileName, Shader* shader);
	void Bind() {
		glBindTexture(GL_TEXTURE_2D, id);
	}
	void Unload() {
		glDeleteTextures(1, &id);
	}

private:
	GLuint id;
};

#endif