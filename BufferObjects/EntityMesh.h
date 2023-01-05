#ifndef EntityMESH_H
#define EntityMESH_H
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Material.h"
class EntityMesh {
public:
	static EntityMesh LoadOBJ(const char* filePath, Material* mat) {
		EntityMesh entMesh;
		std::vector<unsigned int> triangles;
		std::vector<float> vertices;
		FILE* file = fopen(filePath, "r");
		if(file == NULL) {
			printf("Failed to load model\n");
			return entMesh;
		}
		while (1) {
			char lineHeader[128];
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break;

			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				vertices.push_back(vertex.x);
				vertices.push_back(vertex.y);
				vertices.push_back(vertex.z);
				vertices.push_back(0.0f);
				vertices.push_back(0.0f);
				vertices.push_back(4.0f);
			}
			else if (strcmp(lineHeader, "f") == 0) {

				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					return entMesh;
				}
				triangles.push_back(vertexIndex[0]-1);
				triangles.push_back(vertexIndex[1]-1);
				triangles.push_back(vertexIndex[2]-1);
			}
		}
		entMesh.Initialize(vertices, triangles, mat);
		return entMesh;
	}
	void Initialize(std::vector<float> vertices, std::vector<unsigned int> triangles, Material* _mat) {
		if (exists) {
			Unload();
		}
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		this->triangles = triangles;
		mat = _mat;
		exists = true;
	}
	void Draw(glm::vec3 pos, glm::mat4 rotation, glm::vec4 color) {
		if (triangles.size() > 0) {
			glBindVertexArray(vao);
			glUniform3fv(glGetUniformLocation(mat->shader->id, "pos"), 1, glm::value_ptr(pos));
			glUniform4fv(glGetUniformLocation(mat->shader->id, "aColor"), 1, glm::value_ptr(color));
			glm::mat4 transform = glm::mat4(1);
			transform = glm::translate(transform, pos);
			transform *= rotation;
			glUniformMatrix4fv(glGetUniformLocation(mat->shader->id, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(transform));
			glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, triangles.data());
			glBindVertexArray(0);
		}
	}
	void Unload() {
		glBindVertexArray(0);
		triangles.clear();
		triangles.shrink_to_fit();
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
