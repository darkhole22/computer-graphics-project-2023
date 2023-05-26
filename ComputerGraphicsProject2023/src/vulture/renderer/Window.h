#pragma once

#include <string>
#include <tuple>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulture/util/String.h>
#include "vulture/event/Event.h"
#include "vulture/event/WindowEvents.h"

namespace vulture {

enum class CursorMode
{
	NORMAL, HIDDEN, DISABLED
};

class Window
{
	EVENT(KeyPressedEvent)
	EVENT(KeyReleasedEvent)

public:
	Window(const char* name, u32 width, u32 height);

	bool shouldClose() const;
	void pollEvents();

	inline const String& getName() const { return c_Name; }
	inline GLFWwindow* getHandle() const { return m_Handle; }

	inline void setFrameBufferResized(bool value) { m_FramebufferResized = value; }

	std::pair<i32, i32> getFramebufferSize() const;
	inline bool getFrameBufferResized() const { return m_FramebufferResized; }

	void setCursorMode(CursorMode mode);
	CursorMode getCursorMode();

	~Window();

	friend void onGlfwKeyEvent(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
private:
	const String c_Name;
	GLFWwindow* m_Handle;
	bool m_FramebufferResized = false;
};

} // namespace vulture