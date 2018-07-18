#pragma once
#include "GLTexture.h"
#include <GL\glew.h>
#include <GL\wglew.h>
#include <string>
#include <vector>

namespace GL {
	class FrameBuffer
	{
		GLuint renderBuffer;
	public:
		std::vector<Texture*> attachedTextures;
		GLuint fbo;

		FrameBuffer(void);
		~FrameBuffer(void);

		bool CreateGeneralFBO(int width, int height, int colorBuffers, bool depthBuffer);
		void Bind();
		void Unbind();
		bool CheckStatus();
	};
}

