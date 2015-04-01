#include <Dunjun/Game.hpp>

#include <Dunjun/Common.hpp>

#include <Dunjun/Window.hpp>
#include <Dunjun/Input.hpp>

#include <Dunjun/Clock.hpp>
#include <Dunjun/TickCounter.hpp>

#include <Dunjun/Math.hpp>
#include <Dunjun/Camera.hpp>
#include <Dunjun/ModelAsset.hpp>

#include <Dunjun/Scene.hpp>
#include <Dunjun/Renderer.hpp>

#include <Dunjun/Level/Level.hpp>
#include <Dunjun/Level/Room.hpp>

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
	GLOBAL const char* default_windowTitle = "Dunjun v0.0.37";
	GLOBAL const f32 TimeStep = (1.0f / 60.0f);
	GLOBAL bool g_running = true;
} // anonymous

GLOBAL ShaderProgram* g_defaultShader;
GLOBAL ModelAsset g_sprite;

GLOBAL Camera g_cameraPlayer;
GLOBAL Camera g_cameraWorld;
GLOBAL Camera* g_currentCamera = &g_cameraPlayer;

GLOBAL SceneNode g_rootNode;
GLOBAL SceneNode* g_player;

GLOBAL Renderer g_renderer;

GLOBAL std::map<std::string, Material> g_materials;
GLOBAL std::map<std::string, Mesh*> g_meshes;

GLOBAL Level* g_level;

namespace Game
{
	INTERNAL void handleInput()
	{
		if (Window::shouldClose() || Input::isKeyPressed(Input::Key::Escape))
			g_running = false;

		if (Input::isKeyPressed(Input::Key::F11))
		{
			Window::g_isFullscreen = !Window::g_isFullscreen;
			if (Window::g_isFullscreen)
			{
				GLFWwindow* w = Window::createWindow(glfwGetPrimaryMonitor());
				Window::destroyWindow();
				Window::g_ptr = w;
			}
			else
			{
				GLFWwindow* w = Window::createWindow(nullptr);
				Window::destroyWindow();
				Window::g_ptr = w;
			}

			Window::makeContextCurrent();
			Window::swapInterval(1);

			//Initial OpenGL settings
			glInit();
		}
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

		meshData.vertices
			.append({ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f })
			.append({ +0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f })
			.append({ +0.5f, +0.5f, 0.0f }, { 1.0f, 1.0f })
			.append({ -0.5f, +0.5f, 0.0f }, { 0.0f, 1.0f });

		meshData.addFace(0, 1, 2).addFace(2, 3, 0);

		g_meshes["sprite"] = new Mesh(meshData);

		g_sprite.material = &g_materials["character"];
		g_sprite.mesh = g_meshes["sprite"];
	}

	INTERNAL void generateWorld()
	{
		g_rootNode.onStart();
	}

	INTERNAL void loadInstances()
	{
		generateWorld();

		// Player
		{
			auto player = make_unique<SceneNode>();
			
			player->name = "player";
			player->transform.position = { 4, 0.5, 4 };
			
			player->addComponent<MeshRenderer>(g_sprite);
			player->addComponent<FaceCamera>(g_cameraWorld);
			
			//player->visible = false;

			g_player = player.get();

			g_rootNode.attachChild(std::move(player));
		}

		// Level
		{
			auto level = make_unique<Level>();

			level->material = &g_materials["terrain"];
			level->generate();

			g_level = level.get();
			g_rootNode.attachChild(std::move(level));
		}

		g_cameraPlayer.transform.position = { -4, 7, 14 };
		g_cameraPlayer.lookAt({ 4, 0, 0 });
		g_cameraPlayer.projectionType = ProjectionType::Perspective;
		g_cameraPlayer.fieldOfView = Degree(50.0f);
		g_cameraPlayer.orthoScale = 8;

		g_cameraWorld = g_cameraPlayer;

		g_cameraPlayer.projectionType = ProjectionType::Orthographic;	

		g_rootNode.onStart();
	}

	INTERNAL void update(f32 dt)
	{
		g_rootNode.update(dt);

		f32 camVel = 10.0f;

		// Gamepad stuff
		/*{
			if (Input::isGamepadPresent(Input::Gamepad_1))
			{
			Input::GamepadAxes axes = Input::getGamepadAxes(Input::Gamepad_1);

			const f32 lookSensitivity = 2.0f;
			const f32 deadZone = 0.21f;

			Vector2 rts = axes.rightThumbstick;
			if (std::abs(rts.x) < deadZone)
			rts.x = 0;
			if (std::abs(rts.y) < deadZone)
			rts.y = 0;

			g_cameraWorlld.offsetOrientation(-lookSensitivity * Radian(rts.x * dt), lookSensitivity * Radian(rts.y * dt));

			Vector2 lts = axes.leftThumbstick;
			if (std::abs(lts.x) < deadZone)
			lts.x = 0;
			if (std::abs(lts.y) < deadZone)
			lts.y = 0

			if (length(lts) > 1.0f)
			lts = normalize(lts);

			Vector3 velDir = { 0, 0, 0 };

			Vector3 forward = g_cameraWorld.forward();
			forward.y = 0;
			forward = normalize(forward);
			velDir += lts.x * g_cameraWorld.right();
			velDir += lts.y * forward;

			Input::GamepadButtons buttons = Input::getGamepadButtons(Input::Gamepad_1);

			if (buttons[(usize)Input::XboxButton::RightShoulder])
			{
			velDir.y += 1;
			}
			if (buttons[(usize)Input::XboxButton::LeftShoulder])
			{
			velDir.y -= 1;
			}

			if (buttons[(usize)Input::XboxButton::DpadUp])
			{
			Vector3 f = g_cameraWorld.forward();
			f.y = 0;
			f = normalize(f);
			velDir += f;
			}
			if (buttons[(usize)Input::XboxButton::DpadDown])
			{
			Vector3 b = g_cameraWorld.backward();
			b.y = 0;
			b = normalize(b);
			velDir += b;
			}

			if (buttons[(usize)Input::XboxButton::DpadLeft])
			{
			Vector3 l = g_cameraWorld.left();
			l.y = 0;
			l = normalize(l);
			velDir += l;
			}
			if (buttons[(usize)Input::XboxButton::DpadRight])
			{
			Vector3 r = g_cameraWorld.right();
			r.y = 0;
			r = normalize(r);
			velDir += r;
			}

			if (length(velDir) > 1.0f)
			velDir = normalize(velDir);

			g_cameraWorld.transform.position += camVel * velDir * dt;

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
			g_cameraWorld.offsetOrientation(-mouseSensitivity * Radian(curPos.x * dt), -mouseSensitivity * Radian(curPos.y * dt));
			Input::setCursorPosition({ 0, 0 });
			Input::setCursorMode(Input::CursorMode::Disabled); // Problem in fullscreen - this fixes it, sort of?
			Vector3 camPos = { 0, 0, 0 };

			f32 superspeed;
			if (Input::isKeyPressed(Input::Key::Space))
			{
				superspeed = 4.0f;
			}
			else
			{
				superspeed = 1.0f;
			}
			if (Input::isKeyPressed(Input::Key::A))
			{
				Vector3 f = g_cameraWorld.left();
				f.y = 0;
				f = normalize(f);
				camPos += f;
			}
			if (Input::isKeyPressed(Input::Key::D))
			{
				Vector3 f = g_cameraWorld.right();
				f.y = 0;
				f = normalize(f);
				camPos += f;
			}
			if (Input::isKeyPressed(Input::Key::W))
			{
				Vector3 f = g_cameraWorld.forward();
				f.y = 0;
				f = normalize(f);
				camPos += f;
			}
			if (Input::isKeyPressed(Input::Key::S))
			{
				Vector3 f = g_cameraWorld.backward();
				f.y = 0;
				f = normalize(f);
				camPos += f;
			}
			if (Input::isKeyPressed(Input::Key::LControl))
				camPos += {0, -1, 0};
			if (Input::isKeyPressed(Input::Key::LShift))
				camPos += {0, +1, 0};

			g_cameraWorld.transform.position += (camVel * superspeed) * camPos * dt;

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

			g_player->transform.position += (playerVel * superspeed) * velDir * dt;

			{
#if 0 // Billboard
				player.transform.orientation = conjugate(quaternionLookAt(player.transform.position, g_camera.transform.position, { 0, 1, 0 }));
#elif 0// Billboard fixed y axis
				Vector3 f = player.transform.position - g_camera.transform.position;

				f.y = 0;
				if (f.x == 0 && f.z == 0)
					player.transform.orientation = Quaternion();
				else
				{
					Radian a(-std::atan(f.z / f.x));
					a += Radian(Constants::Tau / 4.0f);
					if (f.x >= 0)
						a -= Radian(Constants::Tau / 2.0f);

					player.transform.orientation = angleAxis(a, { 0, 1, 0 });
				}
#endif
			}
		}

		// Camera lag in orthographic view
		g_cameraPlayer.transform.position.x = lerp(g_cameraPlayer.transform.position.x, g_player->transform.position.x - 5, 10.0f * dt);
		g_cameraPlayer.transform.position.z = lerp(g_cameraPlayer.transform.position.z, g_player->transform.position.z + 12, 10.0f * dt);

		f32 aspectRatio = Window::getFramebufferSize().x / Window::getFramebufferSize().y;
		if (aspectRatio && Window::getFramebufferSize().y > 0)
		{
			g_cameraPlayer.viewportAspectRatio = aspectRatio;
			g_cameraWorld.viewportAspectRatio = aspectRatio;
		}

		// Camera swapping
		{
			if (Input::isKeyPressed(Input::Key::Num1))
				g_currentCamera = &g_cameraPlayer;
			else if (Input::isKeyPressed(Input::Key::Num2))
			{
				g_cameraWorld.transform = g_cameraPlayer.transform;
				g_currentCamera = &g_cameraWorld;
			}
		}
	}

	INTERNAL void render()
	{
		{
			Vector2 fbSize = Window::getFramebufferSize();
			glViewport(0, 0, (GLsizei)fbSize.x, (GLsizei)fbSize.y);
		}
		
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		g_renderer.reset();
		g_renderer.setCamera(*g_currentCamera);
		g_renderer.draw(g_rootNode);


		Window::swapBuffers();
	}

	void init()
	{
		if (!Window::init())
			return;

		glewInit();
		glInit();
		
		Input::setup();
		Input::setCursorPosition({ 0, 0 });
		Input::setCursorMode(Input::CursorMode::Disabled);

		loadShaders();
		loadMaterials();
		loadSpriteAsset();
		loadInstances();
	}

	void run()
	{
		std::stringstream titleStream;

		TickCounter tc;
		Clock frameClock;

		f64 accumulator = 0;
		f64 prevTime = Input::getTime();

		while (g_running)
		{
			Window::makeContextCurrent();

			f64 currentTime = Input::getTime();
			f64 dt = currentTime - prevTime;
			prevTime = currentTime;
			accumulator += dt;

			if (accumulator > 1.2f) // Remove "loop of death"... (fullscreen)
				accumulator = 1.2f;

			while (accumulator >= TimeStep)
			{
				accumulator -= TimeStep;
				Window::pollEvents();
				handleInput();
				Input::updateGamepads();
				update(TimeStep);
			}

			if (tc.update(0.5))
			{
				titleStream.str("");
				titleStream.clear();
				titleStream << default_windowTitle << " - " << 1000.0 / tc.getTickRate() << " ms";
				Window::setTitle(titleStream.str().c_str());
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
		Window::cleanup();
	}

	void glInit()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
	
} //namespace Game
} //namespace Dunjun
