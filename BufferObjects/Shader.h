#ifndef SHADER_H
#define SHADER_H
#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

//CTRL C CTRL V FROM VICTOR GORDON
static std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

class Shader {
public:
	GLuint id;
	// Create shader from vertex and fragment shader files
	void Create(const char* vertexFileName, const char* fragmentFileName) {
		std::string vertexFileContents = get_file_contents(vertexFileName);
		std::string fragmentFileContents = get_file_contents(fragmentFileName);
		const char* vertex = vertexFileContents.c_str();
		const char* fragment = fragmentFileContents.c_str();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertex, NULL);
		glCompileShader(vertexShader);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragment, NULL);
		glCompileShader(fragmentShader);
		id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glLinkProgram(id);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	// Bind the shader for use
	void Bind() {
		glUseProgram(id);
	}
	// Unload shader from memory
	void Unload() {
		glDeleteProgram(id);
	}
};

#endif