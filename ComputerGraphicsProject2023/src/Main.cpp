#include <iostream>
#include <cstdlib>

#include "project/Application.h"

namespace computergraphicsproject {
	void Application::setup()
	{

	}

	void Application::update()
	{

	}
}

int main()
{
    bool result = computergraphicsproject::launch("Hello Application", 800, 600);

	if (result) return EXIT_SUCCESS;
	std::cin.get();
	return EXIT_FAILURE;
}