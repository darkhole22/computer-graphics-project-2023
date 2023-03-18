#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace computergraphicsproject {

class Window
{
public:
	Window(const char* name, uint32_t width, uint32_t height);

	bool shouldClose();
	void pollEvents();

	inline const std::string& getName() const { return c_name; }
	inline GLFWwindow* getHandle() const { return m_Handle; }

	inline void setFrameBufferResized(bool value) { m_FramebufferResized = value; }

	~Window();
private:
	const uint32_t c_width, c_height;
	const std::string c_name;
	GLFWwindow* m_Handle;
	bool m_FramebufferResized = false;
};

}