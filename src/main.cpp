#include <Dunjun/Common.hpp>
#include <Dunjun/ShaderProgram.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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

	/*Vertices(x, y), colours(r, g, b), texture coordinates (s, t) of the onscreen triangle(s)*/
	float vertices[] = {
	//	    x      y     r     g     b     s     t
		+0.5f, +0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // vertex 0
		-0.5f, +0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // vertex 1
		+0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // vertex 2
		-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // vertex 3
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
	shaderProgram.attachShaderFromFile(Dunjun::ShaderType::Vertex, "data/shaders/default.vert.glsl");
	shaderProgram.attachShaderFromFile(Dunjun::ShaderType::Fragment, "data/shaders/default.frag.glsl");

	shaderProgram.bindAttribLocation(0, "vertPosition");
	shaderProgram.bindAttribLocation(1, "vertColor");
	shaderProgram.bindAttribLocation(2, "vertTexCoord");

	shaderProgram.link();
	shaderProgram.use();

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // nearest = pixelated
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // linear = blurred


	unsigned char* image;
	int width, height, comp;
	image = stbi_load("data/textures/batman.jpg", &width, &height, &comp, 0);

	//Checkerboard pattern
	float pixels[] = {
		0, 0, 1, 1, 0, 0,
		0, 1, 0, 1, 1, 0,
	};
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glActiveTexture(GL_TEXTURE0);
	shaderProgram.setUniform("uniTex", 0);

	stbi_image_free(image);




	bool fullscreen = false;
	bool running = true;
	/*Loop until the user closes the window*/
	while (running)
	{
		{
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			glViewport(0, 0, width, height);
		}

		/*Default pixel value (background colour)*/
		glClearColor(0.5f, 0.69f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/*Render here*/
		{
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