#ifndef SHADER_H
#define SHADER_H
#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

//CTRL C CTRL V
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
	void Create(const char* vertexFileName, const char* fragmentFileName) {
		std::string v = get_file_contents(vertexFileName);
		std::string f = get_file_contents(fragmentFileName);
		const char* vertex = v.c_str();
		const char* fragment = f.c_str();
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
	void Bind() {
		glUseProgram(id);
	}
	void Unload() {
		glDeleteProgram(id);
	}
};

#endif