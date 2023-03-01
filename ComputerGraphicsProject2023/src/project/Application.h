#pragma once

#include <string>
#include <stdint.h>
#include <type_traits>

#define GLFW_INCLUDE_VULKAN
#include <GlFW/glfw3.h>

#include "vulkan_wrapper.h"

namespace computergraphicsproject {

class Application
{
public:
	Application(const char* name, uint32_t width, uint32_t height);

	virtual void setup() = 0;
	virtual void update() = 0;

	void run();

	inline void setFrameBufferResized(bool value) { m_FramebufferResized = value; }

	virtual ~Application();
private:
	const uint32_t c_width, c_height;
	const std::string c_name;
	GLFWwindow* m_Window;
	bool m_FramebufferResized = false;

	Instance m_Instance;
	Surface m_Surface;
	PhysicalDevice m_PhysicalDevice;
	Device m_Device;

	void initWindow();
	void initVulkan();
};

template<typename T>
void launch(const char* name, uint32_t width, uint32_t height)
{
	static_assert(std::is_base_of<Application, T>::value, "Trying to launch a class that is not an Application!");

	T application(name, width, height);
	application.run();
}

}
