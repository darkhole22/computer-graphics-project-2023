#include "Window.h"

namespace computergraphicsproject {

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->setFrameBufferResized(true);
}

Window::Window(const char* name, uint32_t width, uint32_t height)
	: c_Name(name)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_Handle = glfwCreateWindow(width, height, name, nullptr, nullptr);
	glfwSetWindowUserPointer(m_Handle, this);
	glfwSetFramebufferSizeCallback(m_Handle, framebufferResizeCallback);
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

std::pair<int, int> Window::getFramebufferSize() const
{
	int width, height;
	glfwGetFramebufferSize(m_Handle, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(m_Handle, &width, &height);
		glfwWaitEvents();
	}
	return { width , height };
}

Window::~Window()
{
	glfwDestroyWindow(m_Handle);

	glfwTerminate();
}

}