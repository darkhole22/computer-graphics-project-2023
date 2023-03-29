#include <iostream>
#include <cstdlib>

#include "vulture/core/Game.h"
#include "vulture/core/Application.h"

using namespace vulture;

class TestGame : public Game
{
public:
	void setup() override
	{
        auto& s = Application::getScene();
	}

	void update(float dt) override
	{
	}
};

int main()
{
	try
	{
        TestGame game;

		Application::launch(game, vulture::AppConfig{
                .name = "Hello Application",
                .width = 800,
                .height = 600
        });

        return EXIT_SUCCESS;
    }
	catch (const std::exception &exception)
	{
		std::cerr << exception.what() << std::endl;
    }

	return EXIT_FAILURE;
}