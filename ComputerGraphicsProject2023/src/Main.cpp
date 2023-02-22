#include <iostream>
#include <cstdlib>

#include "project/Application.h"

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
		return EXIT_SUCCESS;
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
	}

	std::cin.get();
	return EXIT_FAILURE;
}