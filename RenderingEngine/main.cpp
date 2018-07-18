// RenderingEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include "RenderingEngine.h"

bool InitGL() {
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		return false;
	}

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported " << version << std::endl;

	// othe OpenGL setup
	glClearColor(1.0f, 1.0f, 1.0f, 0.f);				// Transparent background
	glClearDepth(1.0f);									// Depth buffer setup
	glEnable(GL_DEPTH_TEST);							// Enables depth testing
	glDepthFunc(GL_LEQUAL);								// The type of depth testing to do
														//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really nice perspective calculations
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(5);

	RenderingEngine::Engine().LoadShaders();

	return true;
}

#pragma region Callbacks

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		RenderingEngine::Engine().LoadShaders();
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		RenderingEngine::Engine().LoadShaders();
		RenderingEngine::Engine().LoadFbos();
	}
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	if (width == 0) {
		width = 1;
	}
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);
	RenderingEngine::Engine().SetCamera(width, height, 60);
	RenderingEngine::Engine().LoadFbos();
}

#pragma endregion

int main(void)
{
	GLFWwindow* window;

	// initialize the library
	if (!glfwInit()) {
		std::cout << "ERROR: couldn't start glfw3" << std::endl;
		return -1;
	}

	// create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(800, 600, "Rendering Engine", NULL, NULL);
	if (!window) {
		std::cout << "ERROR: couldn't open window with glfw3" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	if (!InitGL()) {
		std::cout << "ERROR: couldn't initial OpenGL with glew" << std::endl;
		glfwTerminate();
		return -1;
	}

	//register events
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	// initialize rendering
	RenderingEngine::Engine().SetCamera(800, 600, 60);
	RenderingEngine::Engine().LoadScene();

	// loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// render here
		RenderingEngine::Engine().Render();

		// swap front and back buffers
		glfwSwapBuffers(window);

		// poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
