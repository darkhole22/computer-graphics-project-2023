#include "Window.h"

namespace computergraphicsproject {

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->setFrameBufferResized(true);
}

Window::Window(const char* name, uint32_t width, uint32_t height)
	: c_width(width), c_height(height), c_name(name)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_Handle = glfwCreateWindow(c_width, c_height, c_name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_Handle, this);
	glfwSetFramebufferSizeCallback(m_Handle, framebufferResizeCallback);
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(m_Handle);
}

void Window::pollEvents()
{
	glfwPollEvents();
}

Window::~Window()
{
	glfwDestroyWindow(m_Handle);

	glfwTerminate();
}

}