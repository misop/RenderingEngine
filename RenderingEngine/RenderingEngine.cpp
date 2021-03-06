// RenderingEngine.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "RenderingEngine.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Shaders {
	Skybox = 0,
	EnvironmentMap,
	Raytracer,
	Integrator,
	Tonemapping,
	Text
};

namespace {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-1, 1);

	float randn() {
		return dis(gen);
	}

	glm::vec3 GenerateRandomRay() {
		glm::vec3 ray(0);
		glm::vec3 normal(0, 0, -1);

		do {
			do {
				ray.x = randn();
				ray.y = randn();
				ray.z = randn();
			} while(glm::length2(ray) < 1e-5);
			ray = glm::normalize(ray);
		} while (glm::dot(normal, ray) < -0.866);
		return ray;// glm::normalize(ray);
	}
}

#pragma region Scene Management

void RenderingEngine::LoadModel(std::string fileName, std::string texturesFileName) {
	models.push_back(new AssimpObject());
	models.back()->LoadFromFile(fileName);
	models.back()->LoadTexturesFromFile(texturesFileName);
}

void RenderingEngine::LoadModelMatrix(glm::vec3 pos, glm::vec3 scale, glm::vec3 euler) {
	glm::quat qx = glm::angleAxis(euler.x, glm::vec3(1, 0, 0));
	glm::quat qy = glm::angleAxis(euler.y, glm::vec3(0, 1, 0));
	glm::quat qz = glm::angleAxis(euler.z, glm::vec3(0, 0, 1));
	glm::quat q = qz * qy * qx;
	glm::mat4 R = glm::toMat4(q);
	glm::mat4 S = glm::scale(glm::mat4(1.0), scale);
	glm::mat4 T = glm::translate(glm::mat4(1.0), pos);
	glm::mat4 M = T * S * R;
	modelMatrices.push_back(M);
}

void RenderingEngine::LoadSkybox() {
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	GLfloat cube_vertices[] = { 1,1,1,    -1,1,1,   -1,-1,1,  1,-1,1,
		1,1,1,    1,-1,1,   1,-1,-1,  1,1,-1,
		1,1,1,    1,1,-1,   -1,1,-1,  -1,1,1,
		-1,1,1,   -1,1,-1,  -1,-1,-1, -1,-1,1,
		-1,-1,-1, 1,-1,-1,  1,-1,1,   -1,-1,1,
		1,-1,-1,  -1,-1,-1, -1,1,-1,  1,1,-1 };
	GLuint cube_indices[] = { 0,1,2,3,  4,5,6,7,  8,9,10,11,   12,13,14,15,   16,17,18,19,  20,21,22,23 };

	vector<float> vertices(begin(cube_vertices), end(cube_vertices));
	vector<int> indices(begin(cube_indices), end(cube_indices));

	if (skybox) delete skybox;
	skybox = new GL::ArrayBuffer();
	skybox->Bind();
	skybox->BindBufferDataf(vertices, 3, GL_STATIC_DRAW);
	skybox->BindElement(indices, GL_STATIC_DRAW);

	float scaling = 1e4;
	skyboxModel = glm::scale(glm::mat4(1.0), glm::vec3(scaling, scaling, scaling));
	skyboxModel *= glm::rotate(glm::mat4(1.0), 180.0f, glm::vec3(0, 1, 0));

	if (skyboxTexture) delete skyboxTexture;
	skyboxTexture = new GL::Texture(GL_TEXTURE_CUBE_MAP);
	//skyboxTexture->LoadCubeTextureFromImages("skybox/SaintPetersBasilica/posX.jpg", "skybox/SaintPetersBasilica/negX.jpg",
	//	"skybox/SaintPetersBasilica/posY.jpg", "skybox/SaintPetersBasilica/negY.jpg",
	//	"skybox/SaintPetersBasilica/posZ.jpg", "skybox/SaintPetersBasilica/negZ.jpg");
	skyboxTexture->LoadHDRCubeTextureFromImages("skybox/graceHDR/posX.hdr", "skybox/graceHDR/negX.hdr",
		"skybox/graceHDR/posY.hdr", "skybox/graceHDR/negY.hdr",
		"skybox/graceHDR/posZ.hdr", "skybox/graceHDR/negZ.hdr");
	//skyboxTexture->LoadCubeTextureFromImages("skybox/Creek/posX.jpg", "skybox/Creek/negX.jpg",
	//	"skybox/Creek/posY.jpg", "skybox/Creek/negY.jpg",
	//	"skybox/Creek/posZ.jpg", "skybox/Creek/negZ.jpg");
}

void RenderingEngine::SetCamera(float width, float height, float fovy) {
	camera.projection = glm::perspective<float>(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 15000);
	camera.fovy = fovy;
	camera.aspect = (float)width / (float)height;
	camera.setWidth(width);
	camera.setHeight(height);
}

void RenderingEngine::LoadFpsCounter() {
	delete text;
	text = new GL::Text("fonts/Consolas_Bold.ttf");
	time = std::chrono::high_resolution_clock::now();;
}

void RenderingEngine::LoadFullScreenQuad() {
	GLfloat verts[] = { 0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0 };
	vector<float> vertices(std::begin(verts), std::end(verts));
	delete quad;
	quad = new GL::ArrayBuffer();
	quad->Bind();
	quad->BindBufferDataf(vertices, 2, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void RenderingEngine::LoadFbos() {
	bool prevDraw = canDraw;
	canDraw = false;

	delete raytracerFbo;
	raytracerFbo = new GL::FrameBuffer();
	raytracerFbo->CreateGeneralFBO(camera.width, camera.height, 1, false);

	delete accuFbo1;
	accuFbo1 = new GL::FrameBuffer();
	accuFbo1->CreateGeneralFBO(camera.width, camera.height, 1, false);

	delete accuFbo2;
	accuFbo2 = new GL::FrameBuffer();
	accuFbo2->CreateGeneralFBO(camera.width, camera.height, 1, false);

	rays = 0;
	render_start = std::chrono::high_resolution_clock::now();

	canDraw = prevDraw;
}

void RenderingEngine::LoadScene() {
	bool prevDraw = canDraw;
	canDraw = false;

	std::ifstream inputFile("scenes/scene.scn");
	//cant create variables in case
	char command;
	std::string str1, str2;
	int int1, int2, int3;
	glm::vec3 pos, scale, euler;
	glm::vec4 att;
	char c;

	while (!inputFile.eof()) {
		inputFile >> command;
		switch (command) {
		case '#':
			getline(inputFile, str1);
			break;
		case 'm':
			inputFile >> str1 >> str2;
			LoadModel(str1, str2);
			break;
		case 'o':
			inputFile >> int1 >> int2 >> c;
			inputFile >> pos.x >> pos.y >> pos.z >> c >> scale.x >> scale.y >> scale.z >> c >> euler.x >> euler.y >> euler.z;
			objects.push_back(glm::ivec3(int1, int1, int1));
			movement.push_back(int2);
			LoadModelMatrix(pos, scale, euler);
			break;
		case 's':
			inputFile >> sunPos.x >> sunPos.y >> sunPos.z >> c >> sunLook.x >> sunLook.y >> sunLook.z >> c >> sunColor.x >> sunColor.y >> sunColor.z;
			sun = glm::normalize(sunLook - sunPos);
			sunColor /= 255.0;
			break;
		case 'l':
			inputFile >> int1 >> c >> pos.x >> pos.y >> pos.z >> c >> scale.x >> scale.y >> scale.z >> c >> att.x >> att.y >> att.z >> att.w;
			scale /= 255.0;
			lights.push_back(LightAnim(pos, scale, att, int1));
			break;
		default:
			break;
		}
	}

	LoadFullScreenQuad();
	LoadSkybox();
	LoadShaders();
	LoadFpsCounter();
	LoadFbos();

	canDraw = prevDraw;
}

#pragma endregion

#pragma region Shader Management

void RenderingEngine::ClearShaders() {
	for (int i = 0; i < shaders.size(); i++) {
		delete shaders[i];
		shaders[i] = NULL;
	}
	for (int i = 0; i < programs.size(); i++) {
		delete programs[i];
		programs[i] = NULL;
	}
	shaders.clear();
	programs.clear();
}

void RenderingEngine::LoadShaders()
{
	bool prevDraw = canDraw;
	canDraw = false;
	ClearShaders();
#pragma region Skybox
	GL::OpenGLShaders *skyboxShaders = new GL::OpenGLShaders();
	skyboxShaders->vert = new GL::Shader(GL_VERTEX_SHADER);
	skyboxShaders->vert->Load("shaders/skybox.vert");
	skyboxShaders->vert->Compile();

	skyboxShaders->frag = new GL::Shader(GL_FRAGMENT_SHADER);
	skyboxShaders->frag->Load("shaders/skybox.frag");
	skyboxShaders->frag->Compile();

	GL::Program *skyboxProgram = new GL::Program("Skybox");
	skyboxProgram->AttachShaders(skyboxShaders);
	skyboxProgram->Link();
	skyboxProgram->SaveProgramLog();

	shaders.push_back(skyboxShaders);
	programs.push_back(skyboxProgram);
#pragma endregion
#pragma region Envoironment Map
	GL::OpenGLShaders *environmentmapShaders = new GL::OpenGLShaders();
	environmentmapShaders->vert = new GL::Shader(GL_VERTEX_SHADER);
	environmentmapShaders->vert->Load("shaders/environmentmap.vert");
	environmentmapShaders->vert->Compile();

	environmentmapShaders->frag = new GL::Shader(GL_FRAGMENT_SHADER);
	environmentmapShaders->frag->Load("shaders/environmentmap.frag");
	environmentmapShaders->frag->Compile();

	GL::Program *environmentmapProgram = new GL::Program("Environmentmap");
	environmentmapProgram->AttachShaders(environmentmapShaders);
	environmentmapProgram->Link();
	environmentmapProgram->SaveProgramLog();

	shaders.push_back(environmentmapShaders);
	programs.push_back(environmentmapProgram);
#pragma endregion
#pragma region Raytracer
	GL::OpenGLShaders *raytracerShaders = new GL::OpenGLShaders();
	raytracerShaders->vert = new GL::Shader(GL_VERTEX_SHADER);
	raytracerShaders->vert->Load("shaders/raytracer.vert");
	raytracerShaders->vert->Compile();

	raytracerShaders->frag = new GL::Shader(GL_FRAGMENT_SHADER);
	raytracerShaders->frag->Load("shaders/raytracer.frag");
	raytracerShaders->frag->Compile();

	GL::Program *raytracerProgram = new GL::Program("Raytracer");
	raytracerProgram->AttachShaders(raytracerShaders);
	raytracerProgram->Link();
	raytracerProgram->SaveProgramLog();

	shaders.push_back(raytracerShaders);
	programs.push_back(raytracerProgram);
#pragma endregion
#pragma region Integrator
	GL::OpenGLShaders *integratorShaders = new GL::OpenGLShaders();
	integratorShaders->vert = new GL::Shader(GL_VERTEX_SHADER);
	integratorShaders->vert->Load("shaders/integrator.vert");
	integratorShaders->vert->Compile();

	integratorShaders->frag = new GL::Shader(GL_FRAGMENT_SHADER);
	integratorShaders->frag->Load("shaders/integrator.frag");
	integratorShaders->frag->Compile();

	GL::Program *integratorProgram = new GL::Program("Integrator");
	integratorProgram->AttachShaders(integratorShaders);
	integratorProgram->Link();
	integratorProgram->SaveProgramLog();

	shaders.push_back(integratorShaders);
	programs.push_back(integratorProgram);
#pragma endregion
#pragma region Tonemapping
	GL::OpenGLShaders *tonemappingShaders = new GL::OpenGLShaders();
	tonemappingShaders->vert = new GL::Shader(GL_VERTEX_SHADER);
	tonemappingShaders->vert->Load("shaders/tonemapping.vert");
	tonemappingShaders->vert->Compile();

	tonemappingShaders->frag = new GL::Shader(GL_FRAGMENT_SHADER);
	tonemappingShaders->frag->Load("shaders/tonemapping.frag");
	tonemappingShaders->frag->Compile();

	GL::Program *tonemappingProgram = new GL::Program("Tonemapping");
	tonemappingProgram->AttachShaders(tonemappingShaders);
	tonemappingProgram->Link();
	tonemappingProgram->SaveProgramLog();

	shaders.push_back(tonemappingShaders);
	programs.push_back(tonemappingProgram);
#pragma endregion
#pragma region Text Shaders
	GL::OpenGLShaders *textShaders = new GL::OpenGLShaders();
	textShaders->vert = new GL::Shader(GL_VERTEX_SHADER);
	textShaders->vert->Load("shaders/text.vert");
	textShaders->vert->Compile();

	textShaders->frag = new GL::Shader(GL_FRAGMENT_SHADER);
	textShaders->frag->Load("shaders/text.frag");
	textShaders->frag->Compile();

	GL::Program *textProgram = new GL::Program("Text");
	textProgram->AttachShaders(textShaders);
	textProgram->Link();
	textProgram->SaveProgramLog();

	shaders.push_back(textShaders);
	programs.push_back(textProgram);
#pragma endregion
	canDraw = prevDraw;
}

#pragma endregion

#pragma region OpenGL utilities

void RenderingEngine::BindTextures(int idx, bool bind) {
	//set material
	glUniform4f(MATERIAL, models[idx]->material.x, models[idx]->material.y, models[idx]->material.z, models[idx]->material.w);
	//set textures
	if (models[idx]->diffuseTexture) {
		glActiveTexture(GL_TEXTURE0);
		if (bind)
			models[idx]->diffuseTexture->Bind();
		else
			models[idx]->diffuseTexture->Unbind();
	}
	if (models[idx]->displacementTexture) {
		glActiveTexture(GL_TEXTURE1);
		if (bind)
			models[idx]->displacementTexture->Bind();
		else
			models[idx]->displacementTexture->Unbind();
	}
	if (models[idx]->normalTexture) {
		glActiveTexture(GL_TEXTURE2);
		if (bind)
			models[idx]->normalTexture->Bind();
		else
			models[idx]->normalTexture->Unbind();
	}
}

#pragma endregion

#pragma region Rendering

void RenderingEngine::DrawFps() {
	programs[Shaders::Text]->Use();
	glUniform1i(SWAP, true);

	std::ostringstream fps_str;
	fps_str << std::setprecision(5) << fps << " fps (" << perFrame << " mus)";
	std::ostringstream rays_str;
	rays_str << "Traced rays: " << rays;
	std::ostringstream rtime_str;
	rtime_str << "Rendering time: " << rendering_time << " s";

	glm::mat4 proj = glm::ortho(0.0f, camera.width, 0.0f, camera.height, -1.0f, 1.0f);
	glUniformMatrix4fv(PROJECTION_MATRIX, 1, GL_FALSE, glm::value_ptr(proj));

	int offset = 20;
	text->RenderText(20, camera.height - offset, fps_str.str());

	offset += 20;
	text->RenderText(20, camera.height - offset, rays_str.str());

	offset += 20;
	text->RenderText(20, camera.height - offset, rtime_str.str());
}

void RenderingEngine::DrawSkybox() {
	//skybox
	programs[Shaders::Skybox]->Use();
	glUniformMatrix4fv(MODEL_MATRIX, 1, GL_FALSE, glm::value_ptr(skyboxModel));
	camera.getCameraMatrices(PROJECTION_MATRIX, VIEW_MATRIX);

	glActiveTexture(GL_TEXTURE0);
	skyboxTexture->Bind();
	skybox->DrawElement(0, GL_QUADS);
}

void RenderingEngine::SetLight() {
	//glUniformMatrix4fv(SHADOW_MATRIX, 1, GL_FALSE, glm::value_ptr(depthMVP));
	glUniform4f(SUN, sun.x, sun.y, sun.z, 0.0);
	glUniform4f(SUN_COLOR, sunColor.r, sunColor.g, sunColor.b, 1);

	for (int i = 0; i < lights.size(); i++) {
		glm::vec4 pos = lights[i].pos;
		glUniform4f(POINT1 + i, pos.x, pos.y, pos.z, 1.0);
		glUniform4f(POINT1_COLOR + i, lights[i].color.r, lights[i].color.g, lights[i].color.b, 1);
		glUniform4f(POINT1_ATTEN + i, lights[i].attenuation.x, lights[i].attenuation.y, lights[i].attenuation.z, lights[i].attenuation.w);
	}
}

void RenderingEngine::DrawModels(bool texture) {
	for (int i = 0; i < objects.size(); i++) {
		glm::mat4 model = modelMatrices[i];
		glUniformMatrix4fv(MODEL_MATRIX, 1, GL_FALSE, glm::value_ptr(model));
		camera.setupNormalMatrix(model, NORMAL_MATRIX);
		int idx = objects[i].y;
		if (texture) {
			BindTextures(idx, true);
		}
		glActiveTexture(GL_TEXTURE3);
		skyboxTexture->Bind();
		models[idx]->buffer->DrawElement(0, GL_TRIANGLES);
		//unbind textures
		if (texture) {
			BindTextures(idx, false);
		}
		glActiveTexture(GL_TEXTURE3);
		skyboxTexture->Unbind();
	}
}

void RenderingEngine::DrawSphere() {
	programs[Shaders::EnvironmentMap]->Use();
	camera.getCameraMatrices(PROJECTION_MATRIX, VIEW_MATRIX);
	SetLight();

	DrawModels();
}

void RenderingEngine::RaytraceSphere() {
	GL::FrameBuffer *textureFBO = useFbo1 ? accuFbo1 : accuFbo2;
	GL::FrameBuffer *integratedFBO = useFbo1 ? accuFbo2 : accuFbo1;
	if (rays < max_rays) {
		// generate a random light path
		raytracerFbo->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, camera.width, camera.height);
		programs[Shaders::Raytracer]->Use();
		camera.getCameraMatrices(PROJECTION_MATRIX, VIEW_MATRIX);
		glm::vec3 ray_dir = GenerateRandomRay();
		glUniform3f(RAY_DIRECTION, ray_dir.x, ray_dir.y, ray_dir.z);
		DrawModels();
		raytracerFbo->Unbind();
		// integrate the light path
		integratedFBO->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		programs[Shaders::Integrator]->Use();
		glActiveTexture(GL_TEXTURE0);
		textureFBO->attachedTextures[0]->Bind();
		glActiveTexture(GL_TEXTURE1);
		raytracerFbo->attachedTextures[0]->Bind();
		quad->Draw(GL_QUADS);
		integratedFBO->Unbind();
		// record used ray
		rays++;
		if (rays < max_rays) {
			useFbo1 = !useFbo1;
		}
	}
	// render tonemapped image
	programs[Shaders::Tonemapping]->Use();
	glActiveTexture(GL_TEXTURE0);
	integratedFBO->attachedTextures[0]->Bind();
	quad->Draw(GL_QUADS);
}

#pragma endregion

void RenderingEngine::Render() {
	if (!canDraw) return;

	auto t = std::chrono::high_resolution_clock::now();
	perFrame = std::chrono::duration_cast<std::chrono::microseconds>(t - time).count();
	fps = 1e6f / perFrame;
	time = t;
	rendering_time = std::chrono::duration_cast<std::chrono::seconds>(t - render_start).count();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//do business here
	DrawSkybox();
	//DrawSphere();
	RaytraceSphere();
	DrawFps();

	glBindVertexArray(0);
}
