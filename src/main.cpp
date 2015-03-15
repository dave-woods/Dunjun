#include <Dunjun/OpenGL.hpp>
#include <Dunjun/Common.hpp>
#include <Dunjun/ShaderProgram.hpp>
#include <Dunjun/Image.hpp>
#include <Dunjun/Texture.hpp>
#include <Dunjun/Clock.hpp>
#include <Dunjun/TickCounter.hpp>
#include <Dunjun/Math.hpp>
#include <Dunjun/Vertex.hpp>
#include <Dunjun/Transform.hpp>

#include <GLFW/glfw3.h>

#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace Dunjun;

GLOBAL int g_windowWidth = 854;
GLOBAL int g_windowHeight = 480;
GLOBAL const char* windowTitle = "Dunjun v0.0.1";

GLOBAL const f32 TIME_STEP = (1.0f / 60.0f);

struct ModelAsset
{
	ShaderProgram* shaders;
	Texture* texture;

	GLuint vbo;
	GLuint ibo;
	
	GLenum drawType;
	GLint drawCount;
};

struct ModelInstance
{
	ModelAsset* asset;
	Transform transform;
};

GLOBAL ShaderProgram* g_defaultShader;
GLOBAL ModelAsset g_sprite;
GLOBAL std::vector<ModelInstance> g_instances;
GLOBAL Matrix4 g_cameraMatrix;

INTERNAL void glfwHints()
{
	glfwWindowHint(GLFW_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_VERSION_MINOR, 1);
}

INTERNAL void resizeCallback(GLFWwindow* window, int width, int height)
{
	g_windowWidth = width;
	g_windowHeight = height;
}

INTERNAL void handleInput(GLFWwindow* window, bool* running, bool* fullscreen)
{
	if (glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE))
		*running = false;

	// Fullscreen DISABLED due to crashes
	/*
	if (glfwGetKey(window, GLFW_KEY_F11))
	{
	*fullscreen = !(*fullscreen);

	GLFWwindow* newWindow;
	glfwHints();
	if (*fullscreen)
	{
	/*Get number of modes
	int count;
	/*Getting the monitors width and height
	const GLFWvidmode* modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &count);

	newWindow = glfwCreateWindow(modes[count - 1].width, modes[count - 1].height, windowTitle, glfwGetPrimaryMonitor(), window);
	}
	else
	{
	newWindow = glfwCreateWindow(g_windowWidth, g_windowHeight, windowTitle, nullptr, window);
	}
	glfwDestroyWindow(window);
	window = newWindow;
	glfwMakeContextCurrent(window);
	}
	*/
}

INTERNAL void loadShaders()
{
	g_defaultShader = new ShaderProgram();
	if (!g_defaultShader->attachShaderFromFile(ShaderType::Vertex, "data/shaders/default.vert.glsl"))
		throw std::runtime_error(g_defaultShader->getErrorLog());
	if (!g_defaultShader->attachShaderFromFile(ShaderType::Fragment, "data/shaders/default.frag.glsl"))
		throw std::runtime_error(g_defaultShader->getErrorLog());

	g_defaultShader->bindAttribLocation(0, "a_position");
	g_defaultShader->bindAttribLocation(1, "a_color");
	g_defaultShader->bindAttribLocation(2, "a_texCoord");

	if (!g_defaultShader->link())
		throw std::runtime_error(g_defaultShader->getErrorLog());

}

INTERNAL void loadSpriteAsset()
{
	Vertex vertices[] = {
		//	    x      y     z         r     g     b     a         s     t
		{ { -0.5f, -0.5f, 0.0f }, { 0xFF, 0x00, 0x00, 0xFF }, { 0.0f, 0.0f } }, // vertex 0
		{ { +0.5f, -0.5f, 0.0f }, { 0xFF, 0xFF, 0x00, 0xFF }, { 1.0f, 0.0f } }, // vertex 1
		{ { +0.5f, +0.5f, 0.0f }, { 0x00, 0xFF, 0xFF, 0xFF }, { 1.0f, 1.0f } }, // vertex 2
		{ { -0.5f, +0.5f, 0.0f }, { 0x00, 0x00, 0xFF, 0xFF }, { 0.0f, 1.0f } }, // vertex 3
	};

	glGenBuffers(1, &g_sprite.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_sprite.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // drawn once

	u32 indices[] = {0, 1, 2, 2, 3, 0};

	glGenBuffers(1, &g_sprite.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_sprite.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	g_sprite.shaders = g_defaultShader;
	g_sprite.texture = new Texture();
	g_sprite.texture->loadFromFile("data/textures/batman.jpg");

	g_sprite.drawType = GL_TRIANGLES;
	g_sprite.drawCount = 6;
}

INTERNAL void loadInstances()
{
	Transform parent;

	ModelInstance a;
	a.asset = &g_sprite;
	a.transform.position = { 0, 0, 0 };
	a.transform.scale = { 3, 3, 3 };
	a.transform.orientation = angleAxis(Degree(45), { 0, 0, 1 });
	g_instances.push_back(a);

	ModelInstance b;
	b.asset = &g_sprite;
	b.transform.position = { 2, 0, 0 };
	g_instances.push_back(b);

	ModelInstance c;
	c.asset = &g_sprite;
	c.transform.position = { 0, 0, 1 };
	c.transform.orientation = angleAxis(Degree(45), { 0, 1, 0 });
	g_instances.push_back(c);
}

INTERNAL void update(float dt)
{
	g_instances[0].transform.orientation = angleAxis(Degree(120) * dt, { 0, 1, 0 }) * g_instances[0].transform.orientation;

	//TODO(dave): Create camera type
	{
		Matrix4 view = lookAt({ 1.0f, 2.0f, 4.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0, 1, 0 });
		Matrix4 proj = perspective(Degree(50.0f),
			(f32)g_windowWidth / (f32)g_windowHeight,
			0.1f, 100.0f);

		g_cameraMatrix = proj * view;
	}
}

INTERNAL void renderInstance(const ModelInstance& inst)
{
	ModelAsset* asset = inst.asset;
	ShaderProgram* shaders = asset->shaders;

	shaders->setUniform("u_camera", g_cameraMatrix);
	shaders->setUniform("u_transform", inst.transform);
	shaders->setUniform("u_tex", (Dunjun::u32)0);

	asset->texture->bind(0);

	glBindBuffer(GL_ARRAY_BUFFER, g_sprite.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_sprite.ibo);

	glEnableVertexAttribArray(0); //vertPosition
	glEnableVertexAttribArray(1); //vertColor
	glEnableVertexAttribArray(2); //vertTexCoord

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const GLvoid*)(sizeof(Dunjun::Vector3)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(sizeof(Dunjun::Vector3) + sizeof(Dunjun::Color)));

	glDrawElements(asset->drawType, asset->drawCount, GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(0); //vertPosition
	glDisableVertexAttribArray(1); //vertColor
	glDisableVertexAttribArray(2); //vertTexCoord
}

INTERNAL void render(GLFWwindow* window)
{
	glViewport(0, 0, g_windowWidth, g_windowHeight);

	/*Default pixel value (background colour)*/
	glClearColor(0.5f, 0.69f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ShaderProgram* currentShaders = nullptr;

	for (const auto& inst : g_instances)
	{
		if (inst.asset->shaders != currentShaders)
		{
			
			currentShaders = inst.asset->shaders;
			currentShaders->use();
		}
		renderInstance(inst);
	}

	if (currentShaders)
		currentShaders->stopUsing();

	/*Swap front and back buffers*/
	glfwSwapBuffers(window);
	/*Poll for and process events*/
	glfwPollEvents();
}

int main(int argc, char** argv)
{
	GLFWwindow* window;

	if (!glfwInit())
		return EXIT_FAILURE;
	
	glfwHints();

	window = glfwCreateWindow(g_windowWidth, g_windowHeight, windowTitle, nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetWindowSizeCallback(window, resizeCallback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glewInit();

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	loadShaders();
	loadSpriteAsset();
	loadInstances();

	bool fullscreen = false;
	bool running = true;

	std::stringstream titleStream;

	TickCounter tc;
	Clock frameClock;

	double accumulator = 0;
	double prevTime = glfwGetTime();

	while (running)
	{
		double currentTime = glfwGetTime();
		double dt = currentTime - prevTime;
		prevTime = currentTime;
		accumulator += dt;

		handleInput(window, &running, &fullscreen);

		while (accumulator >= TIME_STEP)
		{
			accumulator -= TIME_STEP;
			update(TIME_STEP);
		}
		
		render(window);

		//Frame Limitation
		while (frameClock.getElapsedTime() < 1.0 / 240.0)
			;
		frameClock.restart();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}
