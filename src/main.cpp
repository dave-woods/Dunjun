#include <Dunjun/Common.hpp>
#include <Dunjun/ShaderProgram.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

GLOBAL const int g_windowWidth = 854;
GLOBAL const int g_windowHeight = 480;
GLOBAL const char* windowTitle = "Dunjun v0.0.1";

INTERNAL void glfwHints()
{
	glfwWindowHint(GLFW_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_VERSION_MINOR, 1);
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

	//if (!glewInit())
		//return EXIT_FAILURE;
	
	/*Initialise the library*/
	glewInit();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/*Vertices and colours of the onscreen triangle*/
	float vertices[] = {
	//	    x      y     r     g     b
		+0.5f, +0.5f, 1.0f, 1.0f, 1.0f, // vertex 0
		-0.5f, +0.5f, 0.0f, 0.0f, 1.0f, // vertex 1
		+0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // vertex 2
		-0.5f, -0.5f, 1.0f, 0.0f, 0.0f // vertex 3
	};

	/*VertexBufferObject*/
	GLuint vbo;
	/*Generate the buffer*/
	glGenBuffers(1, &vbo);
	/*Bind the VBO to the buffer*/
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	/*Pass the vertices to the buffer*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	/*
		GL_STATIC_DRAW // drawn once
		GL_DYNAMIC_DRAW // animated, but not often
		GL_STREAM_DRAW // redrawn very regularly
	*/

	Dunjun::ShaderProgram shaderProgram;
	shaderProgram.attachShaderFromFile(Dunjun::ShaderType::Vertex, "data/shaders/default.vert.glsl");
	shaderProgram.attachShaderFromFile(Dunjun::ShaderType::Fragment, "data/shaders/default.frag.glsl");

	shaderProgram.bindAttribLocation(0, "vertPosition");
	shaderProgram.bindAttribLocation(1, "vertColor");

	shaderProgram.link();
	shaderProgram.use();

	bool fullscreen = false;
	bool running = true;
	/*Loop until the user closes the window*/
	while (running)
	{
		/*Default pixel value (background colour)*/
		glClearColor(0.5f, 0.69f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/*Render here*/
		{
			glEnableVertexAttribArray(0); //vertPosition
			glEnableVertexAttribArray(1); //vertColor

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const GLvoid*)(2 * sizeof(float)));

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			glDisableVertexAttribArray(0); //vertPosition
			glDisableVertexAttribArray(0); //vertColor
		}

		/*Swap front and back buffers*/
		glfwSwapBuffers(window);

		/*Poll for and process events*/
		glfwPollEvents();


		if (glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE))
			running = false;



		// Fullscreen DISABLED due to crashes
		/*
		if (glfwGetKey(window, GLFW_KEY_F11))
		{
			fullscreen = !fullscreen;
			
			GLFWwindow* newWindow;
			glfwHints();
			if (fullscreen)
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

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}