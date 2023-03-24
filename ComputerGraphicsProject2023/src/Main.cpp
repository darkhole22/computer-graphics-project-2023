#include <iostream>
#include <cstdlib>

#include "application/Application.h"

class TestApplication : public computergraphicsproject::Application
{
public:
	// This inherits the parent class constructors.
	using Application::Application;

	void setup()
	{

	}

	void update()
	{

	}
};

int main()
{
	try
	{
		computergraphicsproject::launch<TestApplication>("Hello Application", 800, 600);

		// std::cin.get();
		return EXIT_SUCCESS;
	}
	catch (const std::exception &exception)
	{
		std::cerr << exception.what() << std::endl;
	}

	return EXIT_FAILURE;
}