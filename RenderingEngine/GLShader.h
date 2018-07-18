#pragma once
#include <GL\glew.h>
#include <GL\wglew.h>
#include <string>
#include <map>

namespace GL {
	class Shader
	{
	private:
		GLuint shader;

		std::string readShaderFile(std::string fileName);
	public:
		std::string name;

		Shader(GLenum shaderType, std::string shaderName);
		Shader(GLenum shaderType);
		~Shader(void);

		GLuint GetShader();
		void Load(std::string shaderFileName);
		void Load(std::string shaderFileName, std::map<std::string, std::string> replaceMap);
		bool Compile();
		std::string GetShaderLog();
		void SaveShaderLog();
	};

	struct OpenGLShaders {
		Shader *vert;
		Shader *ctrl;
		Shader *eval;
		Shader *geom;
		Shader *frag;

		OpenGLShaders() : vert(NULL), ctrl(NULL), eval(NULL), geom(NULL), frag(NULL) { }
		~OpenGLShaders() {
			if (vert) delete vert;
			if (ctrl) delete ctrl;
			if (eval) delete eval;
			if (geom) delete geom;
			if (frag) delete frag;
		}
	};
}