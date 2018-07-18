#pragma once
#include <vector>
#include <string>
#include <memory>
#include <glm\glm.hpp>
#include "GLArrayBuffer.h"
#include "GLTexture.h"

class AssimpObject
{
public:
	glm::vec4 material;
	GL::ArrayBuffer *buffer;
	std::shared_ptr<GL::Texture> diffuseTexture;
	std::shared_ptr<GL::Texture> displacementTexture;
	std::shared_ptr<GL::Texture> normalTexture;

	AssimpObject(void);
	~AssimpObject(void);
	
	void LoadFromFile(std::string fileName);
	void LoadTexturesFromFile(std::string fileName);
};

