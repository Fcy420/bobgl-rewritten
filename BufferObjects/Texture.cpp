#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include<stb/stb_image.h>
// Create texture from image file and assign it to texture
void Texture::Create(const char* fileName, Shader* shader) {
	int wImg, hImg, nColH;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(fileName, &wImg, &hImg, &nColH, 0);
	if (!bytes) {
		std::cout << "Failed to load texture\n";
	}
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wImg, hImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
	GLuint tex0Uni = glGetUniformLocation(shader->id, "tex0");
	shader->Bind();
	glUniform1i(tex0Uni, 0);
}
