#include <iostream>
#include <cstdlib>

#include "application/Application.h"

#include "event/Event.h"

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

class KeyPressed
{
public:
	int keyCode;
};

void fun(int i)
{
	std::cout << "Fun:" << i << std::endl;
}

void f2(const KeyPressed &e)
{
	std::cout << "Key: " << e.keyCode << std::endl;
}

class C
{
	EVENT(int)
	EVENT(KeyPressed)

public:
	C()
	{
	}

	void d()
	{
		emit(420);
		emit(69);
		emit(KeyPressed{12});
	}
};

int main()
{
	C c;
	c.addCallback(fun);
	c.addCallback(f2);

	c.removeCallback(fun);

	c.d();

	try
	{
		// computergraphicsproject::launch<TestApplication>("Hello Application", 800, 600);
		return EXIT_SUCCESS;
	}
	catch (const std::exception &exception)
	{
		std::cerr << exception.what() << std::endl;
	}

	return EXIT_FAILURE;
}