#include "Application.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vec4.hpp>
#include <mat4x4.hpp>

#include <iostream>

namespace computergraphicsproject {

bool launch(const char* name, uint32_t width, uint32_t height)
{
	try
	{
		Application application(name, width, height);
		application.run();
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		return false;
	}

	return true;
}

Application::Application(const char* name, uint32_t width, uint32_t height)
	: c_width(width), c_height(height), c_name(name)
{
	initWindow();
	initVulkan();
	setup();
}

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	app->setFrameBufferResized(true);
}

void Application::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_Window = glfwCreateWindow(c_width, c_height, c_name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_Window, this);
	glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
}

void Application::initVulkan()
{
	m_Instance = std::move(Instance(c_name));
	m_DebugMessager = std::move(DebugUtilMessanger(m_Instance));
	m_Surface = std::move(Surface(m_Instance, m_Window));
	m_PhysicalDevice = PhysicalDevice::pickDevice(m_Instance, m_Surface);
}

void Application::run()
{
	while (!glfwWindowShouldClose(m_Window)) {
		glfwPollEvents();

		update();

		// drawFrame();
	}

	// vkDeviceWaitIdle(m_Device);
}

Application::~Application()
{
	glfwDestroyWindow(m_Window);

	glfwTerminate();
}

}