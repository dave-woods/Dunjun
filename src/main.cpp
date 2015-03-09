#include <Dunjun/OpenGL.hpp>
#include <Dunjun/Common.hpp>
#include <Dunjun/ShaderProgram.hpp>
#include <Dunjun/Image.hpp>
#include <Dunjun/Texture.hpp>

#include <Dunjun/OpenGL.hpp>

#include <GLFW/glfw3.h>

#include <cmath>
#include <string>
#include <fstream>
#include <sstream>

class Clock
{
public:
	inline double getElapsedTime() const { return (glfwGetTime() - m_startTime); }
	
	double restart()
	{
		double now = glfwGetTime();
		double elapsed = now - m_startTime;
		m_startTime = now;

		return elapsed;
	}
private:
	double m_startTime = glfwGetTime();
};

class TickCounter
{
public:
	bool update(double frequency)
	{
		bool reset = false;
		if (m_clock.getElapsedTime() >= frequency)
		{
			m_tickRate = m_tick / frequency;
			m_tick = 0;
			reset = true;
			m_clock.restart();
		}

		m_tick++;

		return reset;
	}

	inline std::size_t getTickRate() const { return m_tickRate; }

private:
	std::size_t m_tick = 0;
	std::size_t m_tickRate = 0;
	Clock m_clock;

};

GLOBAL const int g_windowWidth = 854;
GLOBAL const int g_windowHeight = 480;
GLOBAL const char* windowTitle = "Dunjun v0.0.1";

INTERNAL void glfwHints()
{
	glfwWindowHint(GLFW_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_VERSION_MINOR, 1);
}

INTERNAL void render()
{
	/*Default pixel value (background colour)*/
	glClearColor(0.5f, 0.69f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0); //vertPosition
	glEnableVertexAttribArray(1); //vertColor
	glEnableVertexAttribArray(2); //vertTexCoord

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const GLvoid*)(2 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const GLvoid*)(5 * sizeof(float)));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(0); //vertPosition
	glDisableVertexAttribArray(1); //vertColor
	glDisableVertexAttribArray(2); //vertTexCoord
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

int main(int argc, char** argv)
{
	/*Create the window*/
	GLFWwindow* window;

	/*Initialise the library*/
	if (!glfwInit())
		return EXIT_FAILURE;
	
	/*Get the GLFW version*/
	glfwHints();

	/*Create a windowed mode window and its OpenGL context*/
	window = glfwCreateWindow(g_windowWidth, g_windowHeight, windowTitle, nullptr, nullptr);
	if (!window)
	{
		/*Window creation failure*/
		glfwTerminate();
		return EXIT_FAILURE;
	}

	/*Make the window's context current*/
	glfwMakeContextCurrent(window);

	/*Initialise the library*/
	glewInit();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/*Vertices(x, y), colours(r, g, b), texture coordinates (s, t) of the onscreen triangle(s)*/
	float vertices[] = {
	//	    x      y     r     g     b     s     t
		+0.5f, +0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // vertex 0
		-0.5f, +0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // vertex 1
		+0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // vertex 2
		-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // vertex 3
	};

	/*VertexBufferObject*/
	GLuint vbo;
	/*Generate the buffer*/
	glGenBuffers(1, &vbo);
	/*Bind the VBO to the buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	/*Pass the vertices to the buffer*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // drawn once

	Dunjun::ShaderProgram shaderProgram;

	if (!shaderProgram.attachShaderFromFile(Dunjun::ShaderType::Vertex, "data/shaders/default.vert.glsl"))
		throw std::runtime_error(shaderProgram.getErrorLog());
	if (!shaderProgram.attachShaderFromFile(Dunjun::ShaderType::Fragment, "data/shaders/default.frag.glsl"))
		throw std::runtime_error(shaderProgram.getErrorLog());

	shaderProgram.bindAttribLocation(0, "vertPosition");
	shaderProgram.bindAttribLocation(1, "vertColor");
	shaderProgram.bindAttribLocation(2, "vertTexCoord");

	if (!shaderProgram.link())
		throw std::runtime_error(shaderProgram.getErrorLog());
	shaderProgram.use();

	Dunjun::Texture tex;
	tex.loadFromFile("data/textures/batman.jpg");
	tex.bind(0);
	shaderProgram.setUniform("uniTex", 0);

	TickCounter tc;

	bool fullscreen = false;
	bool running = true;
	/*Loop until the user closes the window*/
	while (running)
	{
		//reshape
		{
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			glViewport(0, 0, width, height);
		}

		if (tc.update(0.5))
		{
			std::cout << tc.getTickRate() << std::endl;
			std::stringstream ss;
			ss << windowTitle << " - " << 1000.0 / tc.getTickRate() << " ms";
			glfwSetWindowTitle(window, ss.str().c_str());
		}

		render();

		/*Swap front and back buffers*/
		glfwSwapBuffers(window);
		/*Poll for and process events*/
		glfwPollEvents();

		handleInput(window, &running, &fullscreen);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}