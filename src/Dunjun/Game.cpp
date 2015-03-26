#include <Dunjun/Game.hpp>

#include <Dunjun/OpenGL.hpp>
#include <Dunjun/Common.hpp>

#include <Dunjun/Input.hpp>

#include <Dunjun/Clock.hpp>
#include <Dunjun/TickCounter.hpp>

#include <Dunjun/Math.hpp>
#include <Dunjun/Camera.hpp>
#include <Dunjun/ModelAsset.hpp>
#include <Dunjun/Level.hpp>

#include <GLFW/glfw3.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Dunjun
{
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
	const char* windowTitle = "Dunjun v0.0.37";
} // anonymous

GLOBAL ShaderProgram* g_defaultShader;
GLOBAL ModelAsset g_sprite;
GLOBAL std::vector<ModelInstance> g_instances;
GLOBAL Camera g_camera;
GLOBAL std::map<std::string, Material> g_materials;
GLOBAL std::map<std::string, Mesh*> g_meshes;

GLOBAL Level g_level;

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
		g_materials["character"].shaders = g_defaultShader;
		g_materials["character"].texture = new Texture();
		g_materials["character"].texture->loadFromFile("data/textures/rudy.png", TextureFilter::Nearest);

		g_materials["default"].shaders = g_defaultShader;
		g_materials["default"].texture = new Texture();
		g_materials["default"].texture->loadFromFile("data/textures/default.png");

		g_materials["stone"].shaders = g_defaultShader;
		g_materials["stone"].texture = new Texture();
		g_materials["stone"].texture->loadFromFile("data/textures/stone.png");

		g_materials["terrain"].shaders = g_defaultShader;
		g_materials["terrain"].texture = new Texture();
		g_materials["terrain"].texture->loadFromFile("data/textures/terrain.png", TextureFilter::Nearest);
	}

	INTERNAL void loadSpriteAsset()
	{
		Mesh::Data meshData;

		meshData.vertices.push_back({ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });
		meshData.vertices.push_back({ { +0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });
		meshData.vertices.push_back({ { +0.5f, +0.5f, 0.0f }, { 1.0f, 1.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });
		meshData.vertices.push_back({ { -0.5f, +0.5f, 0.0f }, { 0.0f, 1.0f }, { { 0xFF, 0xFF, 0xFF, 0xFF } } });

		meshData.indices.push_back(0);
		meshData.indices.push_back(1);
		meshData.indices.push_back(2);
		meshData.indices.push_back(2);
		meshData.indices.push_back(3);
		meshData.indices.push_back(0);

		g_meshes["sprite"] = new Mesh(meshData);

		g_sprite.material = &g_materials["character"];
		g_sprite.mesh = g_meshes["sprite"];
	}

	INTERNAL void generateWorld()
	{
		g_level.material = &g_materials["terrain"];
		
		int mapWidth = 16;
		int mapDepth = 16;
		int mapHeight = 3;

		Level::TileId lightWoodTile = { 0, 11 };
		Level::RandomTileSet stoneTiles;
		for (int i = 0; i < 2; i++)
			stoneTiles.emplace_back(Level::TileId{ i, 15 });

		for (int i = 0; i < mapWidth; i++)
			for (int j = 0; j < mapDepth; j++)
				g_level.addTileSurface(Vector3(i, 0, j), Level::TileSurfaceFace::Up, lightWoodTile);

		for (int k = 0; k < mapHeight; k++)
		{
			for (int j = 0; j < mapDepth; j++)
				g_level.addTileSurface(Vector3(0, k, j), Level::TileSurfaceFace::Right, stoneTiles);

			for (int j = 0; j < mapDepth; j++)
				g_level.addTileSurface(Vector3(mapWidth, k, j), Level::TileSurfaceFace::Left, stoneTiles);

			for (int i = 0; i < mapWidth; i++)
				g_level.addTileSurface(Vector3(i, k, 0), Level::TileSurfaceFace::Front, stoneTiles);

			for (int i = 0; i < mapWidth; i++)
				g_level.addTileSurface(Vector3(i, k, mapDepth), Level::TileSurfaceFace::Back, stoneTiles);
		}
		
		g_level.generate();

		//ModelInstance level;
		
		//g_instances.push_back(level);
	}

	INTERNAL void loadInstances()
	{
		Transform parent;

		ModelInstance a;
		a.asset = &g_sprite;
		a.transform.position = { 4, 0.5, 4 };
		a.transform.scale = { 1, 1, 1 };
		g_instances.push_back(a);

		generateWorld();

		g_camera.transform.position = { 4, 4, 10 };
		g_camera.lookAt({ 4, 0, 0 });
		g_camera.projectionType = ProjectionType::Perspective;
		g_camera.fieldOfView = Degree(50.0f);
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
			Vector2 curPos = Input::getCursorPosition();
			const f32 mouseSensitivity = 0.05f;
			//negative mouseSensitivity for inverted
			g_camera.offsetOrientation(-mouseSensitivity * Radian(curPos.x * dt), -mouseSensitivity * Radian(curPos.y * dt));
			Input::setCursorPosition({ 0, 0 });
			Vector3 camPos = { 0, 0, 0 };
			
			if (Input::isKeyPressed(Input::Key::A))
			{
				Vector3 f = g_camera.left();
				f.y = 0;
				f = normalize(f);
				camPos += f;
			}
			if (Input::isKeyPressed(Input::Key::D))
			{
				Vector3 f = g_camera.right();
				f.y = 0;
				f = normalize(f);
				camPos += f;
			}
			if (Input::isKeyPressed(Input::Key::W))
			{
				Vector3 f = g_camera.forward();
				f.y = 0;
				f = normalize(f);
				camPos += f;
			}
			if (Input::isKeyPressed(Input::Key::S))
			{
				Vector3 f = g_camera.backward();
				f.y = 0;
				f = normalize(f);
				camPos += f;
			}
			if (Input::isKeyPressed(Input::Key::LControl))
				camPos += {0, -1, 0};
			if (Input::isKeyPressed(Input::Key::LShift))
				camPos += {0, +1, 0};

			g_camera.transform.position += camVel * camPos * dt;

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
#elif 1// Billboard fixed y axis
				Vector3 f = player.transform.position - g_camera.transform.position;
				
				f.y = 0;
				if (f.x == 0 && f.z == 0)
					player.transform.orientation = Quaternion();
				else
				{
					Radian a(-std::atan(f.z / f.x));
					a += Radian(Constants::TAU / 4.0f);
					if (f.x >= 0)
						a -= Radian(Constants::TAU / 2.0f);

					player.transform.orientation = angleAxis(a, { 0, 1, 0 });
				}
#endif
			}
		}
		//g_camera.transform.position.x = player.transform.position.x;
		//g_camera.lookAt(player.transform.position);
		g_camera.viewportAspectRatio = getWindowSize().x / getWindowSize().y;

	}

	INTERNAL void renderInstance(const ModelInstance& inst)
	{
		ModelAsset* asset = inst.asset;
		ShaderProgram* shaders = asset->material->shaders;

		shaders->setUniform("u_camera", g_camera.getMatrix());
		shaders->setUniform("u_transform", inst.transform);
		shaders->setUniform("u_tex", (Dunjun::u32)0);

		asset->mesh->draw();
	}

	INTERNAL void renderLevel(const Level& level)
	{
		ShaderProgram* shaders = level.material->shaders;

		shaders->setUniform("u_camera", g_camera.getMatrix());
		shaders->setUniform("u_transform", level.transform);
		shaders->setUniform("u_tex", (Dunjun::u32)0);

		level.mesh->draw();
	}

	INTERNAL void render()
	{
		glViewport(0, 0, windowWidth, windowHeight);

		/*Default pixel value (background colour)*/
		glClearColor(33.0f / 256.0f, 28.0f / 256.0f, 24.0f / 256.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const ShaderProgram* currentShaders = nullptr;
		const Texture* currentTexture = nullptr;

		//Instances
		for (const auto& inst : g_instances)
		{
			if (inst.asset->material->shaders != currentShaders)
			{
				if (currentShaders)
					currentShaders->stopUsing();

				currentShaders = inst.asset->material->shaders;
				currentShaders->use();
			}

			if (inst.asset->material->texture != currentTexture)
			{
				currentTexture = inst.asset->material->texture;
				Texture::bind(currentTexture, 0);
			}

			renderInstance(inst);
		}

		//Level
		{
			if (g_level.material->shaders != currentShaders)
			{
				if (currentShaders)
					currentShaders->stopUsing();

				currentShaders = g_level.material->shaders;
				currentShaders->use();
			}

			if (g_level.material->texture != currentTexture)
			{
				currentTexture = g_level.material->texture;
				Texture::bind(currentTexture, 0);
			}

			renderLevel(g_level);
		}



		if (currentShaders)
			currentShaders->stopUsing();
		
		Texture::bind(nullptr, 0);

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
		Input::setCursorMode(Input::CursorMode::Disabled);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		loadShaders();
		loadMaterials();
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