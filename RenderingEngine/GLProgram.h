#pragma once
#include "GLShader.h"
#include "Uniforms.h"

namespace GL {
	class Program
	{
	private:
	public:
		GLuint program;
		std::string name;

		Program(void);
		Program(std::string programName);
		~Program(void);

		GLuint GetProgram();
		void AttachShader(Shader *shader);
		void AttachShaders(OpenGLShaders *shaders);
		bool Link();
		void Use();
		void BindAttribLocation(GLuint index, std::string name);
		GLint getUniformLocation(std::string name);
		std::string GetProgramLog();
		void SaveProgramLog();
	};
}

