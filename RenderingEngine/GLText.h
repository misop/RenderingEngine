#pragma once
#include <string>
#include <GL\glew.h>
#include <GL\wglew.h>
#include <glm/glm.hpp>
#include "GLCamera.h"
#include "GLArrayBuffer.h"
using namespace std;

namespace GL {
	struct GlyphMetrics
	{
		int left;
		int top;
		int advance;
		int bitmap_width;
		int bitmap_height;
	};

	class Text
	{
		ArrayBuffer *vao;
		GLuint characters[96];
		glm::mat4 models[96];
		GlyphMetrics glyphs[96];
	public:
		float fontSize;
		glm::vec4 fontColor;

		Text(string fileName);
		~Text(void);

		void RenderText(float x, float y, string text);
	};

}
