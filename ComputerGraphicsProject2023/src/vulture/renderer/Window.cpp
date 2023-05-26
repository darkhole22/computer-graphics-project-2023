#include "Window.h"

namespace vulture {

void framebufferResizeCallback(GLFWwindow* window, i32 width, i32 height)
{
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->setFrameBufferResized(true);
}

void onGlfwKeyEvent(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods)
{
	auto w = static_cast<Window*>(glfwGetWindowUserPointer(window));

	switch (action)
	{
	case GLFW_RELEASE:
	{
		w->emit(KeyReleasedEvent{ key });
		break;
	}
	case GLFW_PRESS:
	{
		w->emit(KeyPressedEvent{ key, false });
		break;
	}
	case GLFW_REPEAT:
	{
		w->emit(KeyPressedEvent{ key, true });
		break;
	}
	default:
	break;
	}
}

Window::Window(const char* name, u32 width, u32 height) : c_Name(name)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_Handle = glfwCreateWindow(width, height, name, nullptr, nullptr);
	glfwSetWindowUserPointer(m_Handle, this);
	glfwSetFramebufferSizeCallback(m_Handle, framebufferResizeCallback);

	glfwSetKeyCallback(m_Handle, onGlfwKeyEvent);
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(m_Handle);
}

void Window::pollEvents()
{
	setFrameBufferResized(false);
	glfwPollEvents();
}

std::pair<i32, i32> Window::getFramebufferSize() const
{
	i32 width, height;
	glfwGetFramebufferSize(m_Handle, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(m_Handle, &width, &height);
		glfwWaitEvents();
	}
	return { width , height };
}

void Window::setCursorMode(CursorMode mode)
{
	switch (mode)
	{
	case CursorMode::NORMAL: glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case CursorMode::HIDDEN: glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	case CursorMode::DISABLED: glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	}
}

CursorMode Window::getCursorMode()
{
	switch (glfwGetInputMode(m_Handle, GLFW_CURSOR))
	{
	case GLFW_CURSOR_NORMAL: return CursorMode::NORMAL;
	case GLFW_CURSOR_HIDDEN: return CursorMode::HIDDEN;
	case GLFW_CURSOR_DISABLED: return CursorMode::DISABLED;
	}
	return CursorMode::NORMAL;
}

Window::~Window()
{
	glfwDestroyWindow(m_Handle);

	glfwTerminate();
}

} // namespace vulture