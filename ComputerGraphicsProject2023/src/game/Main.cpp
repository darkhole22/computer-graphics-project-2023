#include <iostream>
#include <cstdlib>

#include "vulture/core/Game.h"
#include "vulture/core/Application.h"
#include "vulture/core/Input.h"

using namespace vulture;

class TestGame : public Game
{
public:
	Scene* scene = nullptr;
	Ref<GameObject> obj;

	void setup() override
	{
		InputAction leftAction{};
		leftAction.keyboardBindings = { 
			KeyboardBinding{{GLFW_KEY_A}}, 
			KeyboardBinding{{GLFW_KEY_LEFT}} 
		};
		leftAction.gamepadButtonBindings = { 
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_LEFT}}
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
		rightAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_RIGHT}}
		};
		rightAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_POS}}}
		};
		Input::setAction("MOVE_RIGHT", rightAction);

		scene = Application::getScene();

		obj = scene->makeObject("res/models/vulture.obj", "res/textures/vulture.png");
	}

	void update(float dt) override
	{
		static float time = 0;
		time += dt;

		float x = Input::getAxis("MOVE_LEFT", "MOVE_RIGHT");
		obj->translate(glm::vec3(x * SPEED * dt, 0.0f, 0.0f));
	}
private:
	const float SPEED = 10;
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