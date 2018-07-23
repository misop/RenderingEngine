#pragma once
#include <GL\glew.h>
#include <GL\wglew.h>
#include <string>
#include <map>
#include <vector>

namespace GL {
	class Texture
	{
	public:
		GLuint texture;
		GLenum target;

		Texture(GLenum texType);
		~Texture(void);

		void Enable();
		void Disable();
		void Bind();
		void Unbind();
		void UseTexture(GLint loc, int unit);
		void TexParameteri(GLenum pname, GLint param);

		void Texture2D(int width, int height, GLenum internalFormat, GLenum format, GLenum type, GLubyte *data);
		void FunctionTexture(int width, int height, float *data);
		void RGBATexture(int width, int height, GLenum format, GLenum type, GLubyte *data);
		void DepthTexture(int width, int height, GLenum type);

		void LoadRGBATextureFromImage(std::string img);
		void LoadCubeTextureFromImages(std::string posxFile, std::string negxFile, std::string posyFile, std::string negyFile, std::string poszFile, std::string negzFile);
		void LoadHDRCubeTextureFromImages(std::string posxFile, std::string negxFile, std::string posyFile, std::string negyFile, std::string poszFile, std::string negzFile);
	};
}

