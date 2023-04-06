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
				}
		});

		Input::setAction("MOVE_RIGHT", InputAction{
				.keyboardBindings = {
						KeyboardBinding{{GLFW_KEY_D}},
						KeyboardBinding{{GLFW_KEY_RIGHT}}
				},
				.gamepadButtonBindings = {
						GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_RIGHT}}
				}
		});


		position = glm::vec2(0.0f);
	}

	void update(float dt) override
	{
		glm::vec2 oldPos(position);

		if (Input::isActionPressed("MOVE_UP"))
		{
			position.y -= SPEED * dt;
		}

		if (Input::isActionPressed("MOVE_DOWN"))
		{
			position.y += SPEED * dt;
		}

		if (Input::isActionPressed("MOVE_RIGHT"))
		{
			position.x += SPEED * dt;
		}

		if (Input::isActionPressed("MOVE_LEFT"))
		{
			position.x -= SPEED * dt;
		}

		if (oldPos != position)
		{
			std::printf("(%.2f, %.2f)\n", position.x, position.y);
		}

		float axisValue = Input::getGamepadAxis(GLFW_GAMEPAD_AXIS_LEFT_X);
		if (axisValue != 0) std::cout << axisValue << std::endl;
	}
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