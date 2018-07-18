#pragma once
#include <vector>
#include <string>
#include <chrono>

#include "GLCamera.h"
#include "GLProgram.h"
#include "GLFrameBuffer.h"
#include "GLArrayBuffer.h"
#include "GLText.h"
#include "AssimpObject.h"
#include "Interpolation.h"

class RenderingEngine
{
public:
	static RenderingEngine& Engine() {
		static RenderingEngine engine;

		return engine;
	}
private:
	RenderingEngine() {}


	glm::vec3 sun, sunPos, sunLook, sunColor;
	vector<LightAnim> lights;

	bool canDraw = true;
	GL::Camera camera;

	GL::Text *text = NULL;
	std::chrono::steady_clock::time_point time = std::chrono::high_resolution_clock::now();
	double fps = 0;
	double perFrame = 0;

	std::vector<GL::Program*> programs;
	std::vector<GL::OpenGLShaders*> shaders;
	std::vector<AssimpObject *> models;
	std::vector<glm::ivec3> objects;
	std::vector<int> movement;
	std::vector<glm::mat4> modelMatrices;

	GL::ArrayBuffer *skybox;
	GL::Texture *skyboxTexture;
	glm::mat4 skyboxModel;

	bool useFbo1 = true;
	GL::FrameBuffer *raytracerFbo = NULL;
	GL::FrameBuffer *accuFbo1 = NULL;
	GL::FrameBuffer *accuFbo2 = NULL;
	GL::ArrayBuffer *quad = NULL;
	int max_rays = std::numeric_limits<int>::max();
	int rays = 0;
	std::chrono::steady_clock::time_point render_start = std::chrono::high_resolution_clock::now();
	double rendering_time = 0;

	void DrawSkybox();
	void SetLight();
	void BindTextures(int idx, bool bind);
	void DrawModels(bool texture = true);
	void DrawSphere();
	void RaytraceSphere();
	void DrawFps();

	void ClearShaders();

	void LoadModel(std::string fileName, std::string texturesFileName);
	void LoadModelMatrix(glm::vec3 pos, glm::vec3 scale, glm::vec3 euler);
	void LoadSkybox();
	void LoadFpsCounter();
	void LoadFullScreenQuad();
public:
	RenderingEngine(RenderingEngine const&) = delete;
	void operator=(RenderingEngine const&) = delete;

	void SetCamera(float width, float height, float fovy);
	void LoadScene();
	void LoadShaders();
	void LoadFbos();
	void Render();
};

