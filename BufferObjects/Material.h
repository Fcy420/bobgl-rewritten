#ifndef MATERIAL_H
#define MATERIAL_H
#include"Texture.h"
#include"Shader.h"

class Material {
public:
	Material(const char* texFileName, Shader* shader) {
		this->shader = shader;
		tex.Create(texFileName, this->shader);
	}
	Shader* shader;
	Texture tex;
};
#endif