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
	bool repited;
};

class KeyReleased
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
	EVENT_CLASS

	EVENT(int)
	EVENT(KeyPressed)
	EVENT(KeyReleased)

public:
	C()
	{
	}

	void d()
	{
		emit(420);
		emit(69);
		emit(KeyPressed{12, false});
	}
};

int main()
{
	C c;

	auto f = f2;

	std::vector<void(const KeyPressed&)> list;

	list.push_back(f2);
	
	auto it = std::remove_if(list.begin(), list.end(),
		[=](void(*p)(const KeyPressed&)) {
		return p == f;
	});
	list.erase(it, list.end());


	std::cout << list.size() << "  Test\n";
	

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