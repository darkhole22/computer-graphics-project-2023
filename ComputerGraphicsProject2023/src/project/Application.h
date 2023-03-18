#pragma once

#include <string>
#include <type_traits>
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <renderer/Window.h>
#include <renderer/Renderer.h>

namespace computergraphicsproject {

class Application
{
public:
	Application(const char* name, uint32_t width, uint32_t height);

	virtual void setup() = 0;
	virtual void update() = 0;

	void run();

	virtual ~Application();
private:
	const std::string c_name;
	Window m_Window;
	Renderer m_Renderer;

};

template<typename T>
void launch(const char* name, uint32_t width, uint32_t height)
{
	static_assert(std::is_base_of<Application, T>::value, "Trying to launch a class that is not an Application!");
	
	std::unique_ptr<T> app = std::make_unique<T>(name, width, height);
	app->run();
}

}
