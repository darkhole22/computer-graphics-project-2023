#include <iostream>
#include <cstdlib>

#include "vulture/core/Game.h"
#include "vulture/core/Application.h"
#include "vulture/core/Input.h"
#include "Player.h"

using namespace vulture;

class TestGame : public Game
{
public:
	Player* player;

	Scene* scene = nullptr;
	Camera* camera = nullptr;
	UIHandler* handlerUI = nullptr;
	Ref<DescriptorSetLayout> descriptorSetLayout;
	PipelineHandle pipeline = -1;
	Ref<Model> model;
	Uniform<ModelBufferObject> objUniform;
	Ref<Texture> objTexture;
	Ref<UIText> text;

	void setup() override
	{
		InputAction leftAction{};
		leftAction.keyboardBindings = { 
			KeyboardBinding{{GLFW_KEY_A}}, 
			KeyboardBinding{{GLFW_KEY_LEFT}} 
		};
		leftAction.gamepadAxisBindings = { 
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_NEG}}}
		};
		Input::setAction("MOVE_LEFT", leftAction);

		InputAction rightAction{};
		rightAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_D}},
			KeyboardBinding{{GLFW_KEY_RIGHT}}
		};
		rightAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_POS}}}
		};
		Input::setAction("MOVE_RIGHT", rightAction);

		InputAction upAction{};
		upAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_W}},
				KeyboardBinding{{GLFW_KEY_UP}}
		};
		upAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_POS}}}
		};
		Input::setAction("MOVE_UP", upAction);

		InputAction downAction{};
		downAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_S}},
				KeyboardBinding{{GLFW_KEY_DOWN}}
		};
		downAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_NEG}}}
		};
		Input::setAction("MOVE_DOWN", downAction);

		scene = Application::getScene();
		camera = scene->getCamera();
		handlerUI = scene->getUIHandle();

		player = new Player(scene->makeObject("res/models/vulture.obj", "res/textures/vulture.png"));

		camera->position = glm::vec3(10, 5, 10);

		text = handlerUI->makeText("0123546879");
		text->setSize(30);
	}

	void update(float dt) override
	{
		static float time = 0;
		time += dt;

		player->update(dt);

		camera->lookAt(glm::vec3(0.0f));

		{
			static float fps = 60.0f;
			static float delta = 0;
			
			static const float WRITE_FPS_TIMEOUT = 0.5; // seconds
			static const float FPS_AVG_WEIGHT = 0.1f; // 0 <= x <= 1
			
			delta += dt;
			fps = fps * (1.0f - FPS_AVG_WEIGHT) + (1.0f / dt) * FPS_AVG_WEIGHT;

			if (delta > WRITE_FPS_TIMEOUT)
			{
				text->setText("FPS: " + std::to_string(fps));
				delta -= 1.0f;
			}
		}
	}
};

int main()
{
	try
	{
		Ref<Application> app; // The game must be destroyed before the application
		{
			TestGame game;

			app = Application::launch(game, vulture::AppConfig{ "Vulture demo", 800, 600 });
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception &exception)
	{
		std::cerr << exception.what() << std::endl;
	}

	return EXIT_FAILURE;
}