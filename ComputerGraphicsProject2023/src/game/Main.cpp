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
					KeyboardBinding{{GLFW_KEY_LEFT_SHIFT, GLFW_KEY_UP}}
			},
			.mouseBindings = {
					MouseBinding{{GLFW_MOUSE_BUTTON_1}},
					MouseBinding{{GLFW_MOUSE_BUTTON_3, GLFW_MOUSE_BUTTON_4}}
			}
		});
	}

	void update(float dt) override
	{

		if (Input::isActionPressed("MOVE_UP"))
		{
			std::cout << "Moving Up: " << dt << std::endl;
		}
	}
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