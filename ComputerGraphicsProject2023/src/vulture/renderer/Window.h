#pragma once

#include <string>
#include <tuple>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulture/event/Event.h"
#include "vulture/event/WindowEvents.h"

namespace vulture {

class Window
{
    EVENT(KeyPressedEvent)
    EVENT(KeyReleasedEvent)

public:
	Window(const char* name, uint32_t width, uint32_t height);

	bool shouldClose() const;
	void pollEvents();

	inline const std::string& getName() const { return c_Name; }
	inline GLFWwindow* getHandle() const { return m_Handle; }

	inline void setFrameBufferResized(bool value) { m_FramebufferResized = value; }

	std::pair<int, int> getFramebufferSize() const;
	inline bool getFrameBufferResized() const { return m_FramebufferResized; }

	~Window();

    friend void onGlfwKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
	const std::string c_Name;
	GLFWwindow* m_Handle;
	bool m_FramebufferResized = false;
};

} // namespace vulture