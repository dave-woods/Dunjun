#include <Dunjun/Common.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

GLOBAL const int g_windowWidth = 854;
GLOBAL const int g_windowHeight = 480;
GLOBAL const char* windowTitle = "Dunjun v0.0.1";

void glfwHints()
{
	glfwWindowHint(GLFW_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_VERSION_MINOR, 1);
}

int main(int argc, char** argv)
{
	GLFWwindow* window;

	/*Initialise the library*/
	if (!glfwInit())
		return EXIT_FAILURE;
	
	glfwHints();

	/*Create a windowed mode window and its OpenGL context*/
	window = glfwCreateWindow(g_windowWidth, g_windowHeight, windowTitle, nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	/*Make the window's context current*/
	glfwMakeContextCurrent(window);

	//if (!glewInit())
		//return EXIT_FAILURE;
	glewInit();

	float vertices[] = {
		0.0f, 0.5f, //first vertex
		-0.5f, -0.5f, //second
		0.5f, -0.5f //third
	};

	GLuint vbo; //vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	/*
		GL_STATIC_DRAW // drawn once
		GL_DYNAMIC_DRAW // animated, but not often
		GL_STREAM_DRAW // redrawn very regularly
	*/

	const char* vertexShaderText = {
		"#version 120\n"
		"\n"
		"attribute vec2 position;"
		"void main()"
		"{"
		"    gl_Position = vec4(position, 0.0, 1.0);"
		"}"
	};

	const char* fragmentShaderText = {
		"#version 120\n"
		"\n"
		"void main()"
		"{"
		"    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
		"}"
	};

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderText, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderText, nullptr);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glBindAttribLocation(shaderProgram, 0, "position");

	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);


	bool fullscreen = false;
	bool running = true;
	/*Loop until the user closes the window*/
	while (running)
	{
		/*Default pixel value*/
		glClearColor(0.5f, 0.69f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/*Render here*/
		{
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

			glDrawArrays(GL_TRIANGLES, 0, 3);

			glDisableVertexAttribArray(0);




		}





		/*Swap front and back buffers*/
		glfwSwapBuffers(window);

		/*Poll for and process events*/
		glfwPollEvents();


		if (glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE))
			running = false;

		if (glfwGetKey(window, GLFW_KEY_F11))
		{
			fullscreen = !fullscreen;
			
			GLFWwindow* newWindow;
			glfwHints();
			if (fullscreen)
			{
				/*Get number of modes*/
				int count;
				/*Getting the monitors width and height*/
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
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}