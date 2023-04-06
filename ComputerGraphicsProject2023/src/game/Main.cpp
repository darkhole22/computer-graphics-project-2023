#include <iostream>
#include <cstdlib>

#include "vulture/core/Game.h"
#include "vulture/core/Application.h"
#include "vulture/core/Input.h"

using namespace vulture;

class TestGame : public Game
{
public:
	void setup() override
	{
        auto& s = Application::getScene();

		Input::setAction("MOVE_UP", InputAction{
			.keyboardBindings = {
					KeyboardBinding{{GLFW_KEY_W}},
					KeyboardBinding{{GLFW_KEY_UP, GLFW_KEY_LEFT_SHIFT}}
			},
			.gamepadButtonBindings = {
					GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_UP}}
			}
		});

		Input::setAction("MOVE_DOWN", InputAction{
				.keyboardBindings = {
						KeyboardBinding{{GLFW_KEY_S}},
						KeyboardBinding{{GLFW_KEY_DOWN}}
				},
				.gamepadButtonBindings = {
						GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_DOWN}}
				}
		});

		Input::setAction("MOVE_LEFT", InputAction{
				.keyboardBindings = {
						KeyboardBinding{{GLFW_KEY_A}},
						KeyboardBinding{{GLFW_KEY_LEFT}}
				},
				.gamepadButtonBindings = {
						GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_LEFT}}
				},
				.gamepadAxisBindings = {
						GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_NEG}}}
				}
		});

		Input::setAction("MOVE_RIGHT", InputAction{
				.keyboardBindings = {
						KeyboardBinding{{GLFW_KEY_D}},
						KeyboardBinding{{GLFW_KEY_RIGHT}}
				},
				.gamepadButtonBindings = {
						GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_RIGHT}}
				},
				.gamepadAxisBindings = {
						GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_POS}}}
				}
		});


		position = glm::vec2(0.0f);
	}

	void update(float dt) override
	{
		std::cout << "Horiz: " << Input::getAxis("MOVE_LEFT", "MOVE_RIGHT") << std::endl;}
private:
	glm::vec2 position;
	const float SPEED = 100.0f;
};

int main()
{
	try
	{
        TestGame game;

		Application::launch(game, vulture::AppConfig{ "Hello Application", 800, 600 });

        return EXIT_SUCCESS;
    }
	catch (const std::exception &exception)
	{
		std::cerr << exception.what() << std::endl;
    }

	return EXIT_FAILURE;
}