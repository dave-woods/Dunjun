#include <Dunjun/Game.hpp>

#include <Dunjun/OpenGL.hpp>
#include <Dunjun/Common.hpp>
#include <Dunjun/ShaderProgram.hpp>
#include <Dunjun/Image.hpp>
#include <Dunjun/Texture.hpp>

#include <Dunjun/Input.hpp>

#include <Dunjun/Clock.hpp>
#include <Dunjun/TickCounter.hpp>

#include <Dunjun/Vertex.hpp>
#include <Dunjun/Math.hpp>
#include <Dunjun/Transform.hpp>
#include <Dunjun/Camera.hpp>
#include <Dunjun/Mesh.hpp>

#include <GLFW/glfw3.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Dunjun
{
struct Material
{
	ShaderProgram* shaders;
	Texture* texture;
};

struct ModelAsset
{
	const Material* material;
	const Mesh* mesh;
};

struct ModelInstance
{
	ModelAsset* asset;
	Transform transform;
};

namespace
{
	const f32 TIME_STEP = (1.0f / 60.0f);
	GLFWwindow* window;
	int windowWidth = 854;
	int windowHeight = 480;
	const char* windowTitle = "Dunjun v0.0.34";
} // anonymous

GLOBAL ShaderProgram* g_defaultShader;
GLOBAL ModelAsset g_sprite;
GLOBAL ModelAsset g_floor;
GLOBAL ModelAsset g_wall;
GLOBAL std::vector<ModelInstance> g_instances;
GLOBAL Camera g_camera;
GLOBAL std::map<std::string, Material> g_materials;
GLOBAL std::map<std::string, Mesh*> g_meshes;

namespace Game
{
	INTERNAL void glfwHints()
	{
		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	}

	INTERNAL void resizeCallback(GLFWwindow* window, int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
	}

	INTERNAL void handleInput(bool* running, bool* fullscreen)
	{
		if (glfwWindowShouldClose(window) || Input::isKeyPressed(Input::Key::Escape))
			*running = false;


		// Fullscreen DISABLED due to crashes
		/*
		if (Input::getKey(GLFW_KEY_F11))
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
			throw std::runtime_error(g_defaultShader->errorLog);
		if (!g_defaultShader->attachShaderFromFile(ShaderType::Fragment, "data/shaders/default.frag.glsl"))
			throw std::runtime_error(g_defaultShader->errorLog);

		g_defaultShader->bindAttribLocation((u32)AttribLocation::Position, "a_position");
		g_defaultShader->bindAttribLocation((u32)AttribLocation::TexCoord, "a_texCoord");
		g_defaultShader->bindAttribLocation((u32)AttribLocation::Color, "a_color");

		if (!g_defaultShader->link())
			throw std::runtime_error(g_defaultShader->errorLog);

	}

	INTERNAL void loadMaterials()
	{
		g_materials["default"].shaders = g_defaultShader;
		g_materials["default"].texture = new Texture();
		g_materials["default"].texture->loadFromFile("data/textures/default.png");

		g_materials["smile"].shaders = g_defaultShader;
		g_materials["smile"].texture = new Texture();
		g_materials["smile"].texture->loadFromFile("data/textures/smile.jpg");

		g_materials["batman"].shaders = g_defaultShader;
		g_materials["batman"].texture = new Texture();
		g_materials["batman"].texture->loadFromFile("data/textures/batman.jpg");
	}

	INTERNAL void loadSpriteAsset()
	{
		Mesh::Data meshData;

		meshData.vertices.push_back({ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, { { 0xFF, 0x00, 0x00, 0xFF } } });
		meshData.vertices.push_back({ { +0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, { { 0xFF, 0xFF, 0x00, 0xFF } } });
		meshData.vertices.push_back({ { +0.5f, +0.5f, 0.0f }, { 1.0f, 1.0f }, { { 0x00, 0xFF, 0xFF, 0xFF } } });
		meshData.vertices.push_back({ { -0.5f, +0.5f, 0.0f }, { 0.0f, 1.0f }, { { 0x00, 0x00, 0xFF, 0xFF } } });

		meshData.indices.push_back(0);
		meshData.indices.push_back(3);
		meshData.indices.push_back(2);
		meshData.indices.push_back(2);
		meshData.indices.push_back(1);
		meshData.indices.push_back(0);

		g_meshes["sprite"] = new Mesh(meshData);

		g_sprite.material = &g_materials["batman"];
		g_sprite.mesh = g_meshes["sprite"];
	}

	INTERNAL void loadFloorAsset()
	{
		Mesh::Data meshData;

		meshData.vertices.push_back({ { -0.5f, 0.0f, -0.5f }, { 0.0f, 0.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });
		meshData.vertices.push_back({ { +0.5f, 0.0f, -0.5f }, { 1.0f, 0.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });
		meshData.vertices.push_back({ { +0.5f, 0.0f, +0.5f }, { 1.0f, 1.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });
		meshData.vertices.push_back({ { -0.5f, 0.0f, +0.5f }, { 0.0f, 1.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });

		meshData.indices.push_back(0);
		meshData.indices.push_back(3);
		meshData.indices.push_back(2);
		meshData.indices.push_back(2);
		meshData.indices.push_back(1);
		meshData.indices.push_back(0);

		g_meshes["floor"] = new Mesh(meshData);

		g_floor.material = &g_materials["default"];
		g_floor.mesh = g_meshes["floor"];
	}

	INTERNAL void generateWorld()
	{
		Mesh::Data floorMD;
		int mapSize = 5;
		for (int i = -mapSize; i <= mapSize; i++)
		{
			for (int j = -mapSize; j <= mapSize; j++)
			{
				usize index = floorMD.vertices.size();

				floorMD.vertices.push_back({ { -0.5f + i, 0.0f, -0.5f + j }, { 0.0f, 0.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });
				floorMD.vertices.push_back({ { +0.5f + i, 0.0f, -0.5f + j }, { 1.0f, 0.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });
				floorMD.vertices.push_back({ { +0.5f + i, 0.0f, +0.5f + j }, { 1.0f, 1.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });
				floorMD.vertices.push_back({ { -0.5f + i, 0.0f, +0.5f + j }, { 0.0f, 1.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });

				floorMD.indices.push_back(index + 0);
				floorMD.indices.push_back(index + 3);
				floorMD.indices.push_back(index + 2);
				floorMD.indices.push_back(index + 2);
				floorMD.indices.push_back(index + 1);
				floorMD.indices.push_back(index + 0);
			}
		}
		
		g_meshes["floor"] = new Mesh(floorMD);
		ModelInstance floorMI;
		floorMI.asset = new ModelAsset();
		floorMI.asset->mesh = g_meshes["floor"];
		floorMI.asset->material = &g_materials["default"];
		floorMI.transform.position.y = -0.5;
		g_instances.push_back(floorMI);
	}

	INTERNAL void loadInstances()
	{
		Transform parent;

		ModelInstance a;
		a.asset = &g_sprite;
		a.transform.position = { 0, 0, 0 };
		g_instances.push_back(a);

		generateWorld();

		g_camera.transform.position = { 0, 2, 7 };
		g_camera.lookAt({ 0, 0, 0 });
		g_camera.projectionType = ProjectionType::Perspective;
		g_camera.fieldOfView = Degree(50.0f);
		//g_camera.projectionType = ProjectionType::Orthographic;
		//g_camera.orthoScale = 1.0f;
	}

	INTERNAL void update(f32 dt)
	{
		ModelInstance& player = g_instances[0];

		f32 camVel = 5.0f;

		// Gamepad stuff
		/*{
			if (Input::isGamepadPresent(Input::Gamepad_1))
			{
				std::cout << "GAMEPAD DETECTED" << std::endl;
				Input::GamepadAxes axes = Input::getGamepadAxes(Input::Gamepad_1);

				const f32 lookSensitivity = 2.0f;
				const f32 deadZone = 0.21f;

				Vector2 rts = axes.rightThumbstick;
				if (std::abs(rts.x) < deadZone)
					rts.x = 0;
				if (std::abs(rts.y) < deadZone)
					rts.y = 0;

				g_camera.offsetOrientation(-lookSensitivity * Radian(rts.x * dt), lookSensitivity * Radian(rts.y * dt));

				Vector2 lts = axes.leftThumbstick;
				if (std::abs(lts.x) < deadZone)
					lts.x = 0;
				if (std::abs(lts.y) < deadZone)
					lts.y = 0;

				lts.y = -lts.y; //xbox thumbstick has inverted y mapping

				if (length(lts) > 1.0f)
					lts = normalize(lts);

				Vector3 velDir = { 0, 0, 0 };

				Vector3 forward = g_camera.forward();
				forward.y = 0;
				forward = normalize(forward);
				velDir += lts.x * g_camera.right();
				velDir += lts.y * forward;

				Input::GamepadButtons buttons = Input::getGamepadButtons(Input::Gamepad_1);

				if (buttons[(usize)Input::XboxButton::RightShoulder])
				{
					velDir += {0, 1, 0};
				}
				if (buttons[(usize)Input::XboxButton::LeftShoulder])
				{
					velDir += {0, -1, 0};
				}

				if (buttons[(usize)Input::XboxButton::DpadUp])
				{
					Vector3 f = g_camera.forward();
					f.y = 0;
					f = normalize(f);
					velDir += f;
				}
				if (buttons[(usize)Input::XboxButton::DpadDown])
				{
					Vector3 b = g_camera.backward();
					b.y = 0;
					b = normalize(b);
					velDir += b;
				}

				if (buttons[(usize)Input::XboxButton::DpadLeft])
				{
					Vector3 l = g_camera.left();
					l.y = 0;
					l = normalize(l);
					velDir += l;
				}
				if (buttons[(usize)Input::XboxButton::DpadRight])
				{
					Vector3 r = g_camera.right();
					r.y = 0;
					r = normalize(r);
					velDir += r;
				}

				if (length(velDir) > 1.0f)
					velDir = normalize(velDir);

				g_camera.transform.position += camVel * velDir * dt;

				// Vibrate
				if (Input::isGamepadButtonPressed(Input::Gamepad_1, Input::XboxButton::A))
				{
					Input::setGamepadVibration(Input::Gamepad_1, 0.5f, 0.5f);
				}
				else
				{
					Input::setGamepadVibration(Input::Gamepad_1, 0.0f, 0.0f);
				}
			}
		}*/

		f32 playerVel = 4.0f;

		// Mouse/Keyboard stuff
		{
			//Vector2 curPos = Input::getCursorPosition();
			//const f32 mouseSensitivity = 0.05f;
			////negative mouseSensitivity for inverted
			//g_camera.offsetOrientation(-mouseSensitivity * Radian(curPos.x * dt), -mouseSensitivity * Radian(curPos.y * dt));
			//Input::setCursorPosition({ 0, 0 });
			//Vector3& camPos = g_camera.transform.position;
			
			/*if (Input::isKeyPressed(Input::Key::A))
				camPos += {-1, 0, 0};
			if (Input::isKeyPressed(Input::Key::D))
				camPos += {+1, 0, 0};
			if (Input::isKeyPressed(Input::Key::W))
				camPos += {0, 0, -1};
			if (Input::isKeyPressed(Input::Key::S))
				camPos += {0, 0, +1};*/

			Vector3 velDir = { 0, 0, 0 };
			
			if (Input::isKeyPressed(Input::Key::Up))
				velDir += {0, 0, -1};
			if (Input::isKeyPressed(Input::Key::Down))
				velDir += {0, 0, 1};

			if (Input::isKeyPressed(Input::Key::Left))
				velDir += {-1, 0, 0};
			if (Input::isKeyPressed(Input::Key::Right))
				velDir += {1, 0, 0};

			if (Input::isKeyPressed(Input::Key::RShift))
				velDir += {0, +1, 0};
			if (Input::isKeyPressed(Input::Key::RControl))
				velDir += {0, -1, 0};

			if (length(velDir) > 0)
				velDir = normalize(velDir);
			
				player.transform.position += playerVel * velDir * dt;

			{
#if 0 // Billboard
				player.transform.orientation = conjugate(quaternionLookAt(player.transform.position, g_camera.transform.position, { 0, 1, 0 }));
#elif 0 // Billboard fixed y axis
				Vector3 f = player.transform.position - g_camera.transform.position;
				
				f.y = 0;
				if (f.x == 0 && f.z == 0)
					player.transform.orientation = Quaternion();
				else
				{
					Radian a(-std::atan(f.z / f.x));
					a += Radian(Constants::TAU / 4.0f);
					if (f.x < 0)
						a -= Radian(Constants::TAU / 2.0f);

					player.transform.orientation = angleAxis(a, { 0, 1, 0 });
				}
#endif
			}
		}
		g_camera.transform.position.x = player.transform.position.x;
		g_camera.lookAt(player.transform.position);
		g_camera.viewportAspectRatio = getWindowSize().x / getWindowSize().y;

	}

	INTERNAL void renderInstance(const ModelInstance& inst)
	{
		ModelAsset* asset = inst.asset;
		ShaderProgram* shaders = asset->material->shaders;

		shaders->setUniform("u_camera", g_camera.getMatrix());
		shaders->setUniform("u_transform", inst.transform);
		shaders->setUniform("u_tex", (Dunjun::u32)0);

		asset->material->texture->bind(0);

		asset->mesh->draw();
	}

	INTERNAL void render()
	{
		glViewport(0, 0, windowWidth, windowHeight);

		/*Default pixel value (background colour)*/
		glClearColor(0, 0.09f, 0.21f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ShaderProgram* currentShaders = nullptr;

		for (const auto& inst : g_instances)
		{
			if (inst.asset->material->shaders != currentShaders)
			{
				if (currentShaders)
					currentShaders->stopUsing();

				currentShaders = inst.asset->material->shaders;
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

	void init()
	{
		if (!glfwInit())
			return;

		glfwHints();
			
		window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			return;
		}

		glfwSetWindowSizeCallback(window, resizeCallback);

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		glewInit();

		Input::setup();

		Input::setCursorPosition({ 0, 0 });
		//Input::setCursorMode(Input::CursorMode::Disabled);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		loadShaders();
		loadMaterials();
		loadFloorAsset();
		loadSpriteAsset();
		loadInstances();
	}

	void run()
	{
		bool fullscreen = false;
		bool running = true;

		std::stringstream titleStream;

		TickCounter tc;
		Clock frameClock;

		f64 accumulator = 0;
		f64 prevTime = Input::getTime();

		while (running)
		{
			f64 currentTime = Input::getTime();
			f64 dt = currentTime - prevTime;
			prevTime = currentTime;
			accumulator += dt;


			while (accumulator >= TIME_STEP)
			{
				accumulator -= TIME_STEP;
				handleInput(&running, &fullscreen);
				Input::updateGamepads();
				update(TIME_STEP);
			}

			if (tc.update(0.5))
			{
				titleStream.str("");
				titleStream.clear();
				titleStream << windowTitle << " - " << 1000.0 / tc.getTickRate()
					<< " ms";
				glfwSetWindowTitle(window, titleStream.str().c_str());
			}

			render();

			//Frame Limitation
			while (frameClock.getElapsedTime() < 1.0 / 240.0)
				;
			frameClock.restart();
		}
	}

	void cleanup()
	{
		Input::cleanup();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	GLFWwindow* getGlfwWindow()
	{
		return window;
	}

	Vector2 getWindowSize()
	{
		return Vector2(windowWidth, windowHeight);
	}
} //namespace Game
} //namespace Dunjun
