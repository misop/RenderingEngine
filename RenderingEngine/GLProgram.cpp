#include "stdafx.h"
#include "GLProgram.h"
#include <fstream>

using namespace std;
using namespace GL;

Program::Program(void)
{
	program = glCreateProgram();
	name = "";
}

Program::Program(std::string programName)
{
	program = glCreateProgram();
	name = programName;
}


Program::~Program(void)
{
	glDeleteProgram(program);
}

GLuint Program::GetProgram() {
	return program;
}

void Program::AttachShader(Shader *shader) {
	glAttachShader(program, shader->GetShader());
}

void Program::AttachShaders(OpenGLShaders *shaders) {
	if (shaders->vert) glAttachShader(program, shaders->vert->GetShader());
	if (shaders->ctrl) glAttachShader(program, shaders->ctrl->GetShader());
	if (shaders->eval) glAttachShader(program, shaders->eval->GetShader());
	if (shaders->geom) glAttachShader(program, shaders->geom->GetShader());
	if (shaders->frag) glAttachShader(program, shaders->frag->GetShader());
}

bool Program::Link() {
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	return (linked == GL_TRUE);
}

void Program::Use() {
	glUseProgram(program);
}

void Program::BindAttribLocation(GLuint index, string name) {
	glBindAttribLocation(program, index, name.c_str());
}

GLint Program::getUniformLocation(std::string name) {
	GLint location = glGetUniformLocation(program, name.c_str());
	if (location == -1) {
		GLenum err = glGetError();
		string s;
		s = string((char*)gluErrorString(err));
	}
	return location;
}

string Program::GetProgramLog() {
	GLint logLength;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength == 0) return "";

	GLchar *logCstr = new GLchar[logLength];
	glGetProgramInfoLog(program, logLength, NULL, logCstr);
	string log(logCstr);
	delete [] logCstr;
	return log;
}

void Program::SaveProgramLog() {
	string log = GetProgramLog();
	string fileName = "logs/log_program_" + name + ".txt";
	//if (!boost::filesystem::exists(fileName) && log.size() == 0) return;

	ofstream output(fileName);
	output << log;
	output.close();
}