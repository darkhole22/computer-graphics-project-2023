#pragma once

#include <string>
#include <stdint.h>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include "vulkan_wrapper.h"

namespace computergraphicsproject {

bool launch(const char* name, uint32_t width, uint32_t height);

class Application
{
public:
	Application(const char* name, uint32_t width, uint32_t height);

	void setup();
	void update();

	void run();

	inline void setFrameBufferResized(bool value) { m_FramebufferResized = value; }

	~Application();
private:
	const uint32_t c_width, c_height;
	const std::string c_name;
	GLFWwindow* m_Window;
	bool m_FramebufferResized = false;
	Instance m_Instance;

	void initWindow();
	void initVulkan();
};

}
