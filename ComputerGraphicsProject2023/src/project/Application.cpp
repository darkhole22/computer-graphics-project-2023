#include "Application.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vec4.hpp>
#include <mat4x4.hpp>

#include <iostream>

namespace computergraphicsproject {

Application::Application(const char* name, uint32_t width, uint32_t height)
	: c_width(width), c_height(height), c_name(name)
{
	initWindow();
	initVulkan();
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
	m_Surface = std::move(Surface(m_Instance, m_Window));
	m_PhysicalDevice = PhysicalDevice::pickDevice(m_Instance, m_Surface);
	m_Device = std::move(Device(m_PhysicalDevice));
	m_RenderPass = std::move(RenderPass(m_Device, m_Surface));
	m_SwapChain = std::move(SwapChain(m_Device, m_Surface, m_RenderPass));
}

void Application::run()
{
	setup();
	while (!glfwWindowShouldClose(m_Window)) {
		glfwPollEvents();

		update();

		// drawFrame();

		// auto renderTarget = RenderTarget(swapchain, device)
		// (scene/sceneMenager).render(renderTarget)
	}

	m_Device.waitIdle();
}

Application::~Application()
{
	glfwDestroyWindow(m_Window);

	glfwTerminate();
}

}