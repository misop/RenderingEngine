#include "stdafx.h"
#include "GLShader.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
using namespace GL;

Shader::Shader(GLenum shaderType)
{
	shader = glCreateShader(shaderType);
	name = "";
}

Shader::Shader(GLenum shaderType, std::string shaderName)
{
	shader = glCreateShader(shaderType);
	name = shaderName;
}

Shader::~Shader(void)
{
	glDeleteShader(shader);
	shader = 0;
}

GLuint Shader::GetShader() {
	return shader;
}

std::string Shader::readShaderFile(std::string fileName) {
	ifstream input(fileName);
	stringstream buffer;
	buffer << input.rdbuf();
	input.close();

	return buffer.str();
}

void Shader::Load(std::string shaderFileName) {
	name = shaderFileName;
	string shaderSource = readShaderFile(shaderFileName);
	GLint length = shaderSource.length();
	const char *shaderSourceCstr = shaderSource.c_str();

	glShaderSource(shader, 1, &shaderSourceCstr, &length);
}

void Shader::Load(std::string shaderFileName, std::map<std::string, std::string> replaceMap) {
	string shaderSource = readShaderFile(shaderFileName);

	for(map<string, string>::iterator it = replaceMap.begin(); it != replaceMap.end(); it++) {
		string key = it->first;
		string value = it->second;
		size_t found = shaderSource.find(key);

		while (found != string::npos) {
			shaderSource.replace(found, key.length(), value);

			found = shaderSource.find(key);
		}
	}

	GLint length = shaderSource.length();
	const char *shaderSourceCstr = shaderSource.c_str();

	glShaderSource(shader, 1, &shaderSourceCstr, &length);
}

bool Shader::Compile() {
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	return (compiled == GL_TRUE);
}

std::string Shader::GetShaderLog() {
	GLint logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength == 0) return "";

	GLchar *logCstr = new GLchar[logLength];
	glGetShaderInfoLog(shader, logLength, NULL, logCstr);
	string log(logCstr);
	delete [] logCstr;

	return log;
}

void Shader::SaveShaderLog() {
	string fileName = "logs/log_shader_" + name + ".txt";
	ofstream output(fileName);
	output << GetShaderLog();
	output.close();
}