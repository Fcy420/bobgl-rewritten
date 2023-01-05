#ifndef MESH_H
#define MESH_H
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "Material.h"

typedef struct {
	std::vector<unsigned char> vertices;
	std::vector<unsigned int> triangles;
	Material* mat;
} MeshComponent;
class Mesh {
public:
	void Initialize(std::vector<unsigned char> vertices, std::vector<unsigned int> triangles, Material* _mat) {
		if (!exists) {
			glGenVertexArrays(1, &vao);
		}
		else {
			glDeleteBuffers(1, &vbo);
		}
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, 6 * sizeof(unsigned char), (void*)0);
		glVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_FALSE, 6 * sizeof(unsigned char), (void*)(3 * sizeof(unsigned char)));
		glVertexAttribPointer(2, 1, GL_UNSIGNED_BYTE, GL_FALSE, 6 * sizeof(unsigned char), (void*)(5 * sizeof(unsigned char)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		this->triangles = triangles;
		mat = _mat;
		exists = true;
	}
	void Draw(glm::vec3 pos, glm::vec4 color) {
		if (triangles.size() > 0) {
			glBindVertexArray(vao);
			glUniform3fv(glGetUniformLocation(mat->shader->id, "pos"), 1, glm::value_ptr(pos));
			glUniform4fv(glGetUniformLocation(mat->shader->id, "aColor"), 1, glm::value_ptr(color));
			glm::mat4 matrix = glm::mat4(1);
			matrix = glm::translate(matrix, pos);
			glUniformMatrix4fv(glGetUniformLocation(mat->shader->id, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(matrix));
			glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, triangles.data());
			glBindVertexArray(0);
		}
	}
	void Unload() {
		glBindVertexArray(vao);
		triangles.clear();
		glDeleteBuffers(1, &vbo);
		exists = false;
		glBindVertexArray(0);
	}
	void Delete() {
		glBindVertexArray(0);
		triangles.clear();
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		exists = false;
	}
	bool Exists() {
		return exists;
	}
	Material* GetMaterial() {
		return mat;
	}
private:
	bool exists = false;
	std::vector<unsigned int> triangles;
	GLuint vbo, vao;
	Material* mat;
};


#endif
