#include "Application.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vec4.hpp>
#include <mat4x4.hpp>

#include <iostream>

namespace computergraphicsproject {

Application::Application(const char* name, uint32_t width, uint32_t height)
	: c_name(name), m_Window(name, width, height), m_Renderer(m_Window)
{
}

void Application::run()
{
	setup();
	while (!m_Window.shouldClose()) {
		m_Window.pollEvents();

		update();

		m_Scene.render(m_Renderer.getRenderTarget());
	}
}

Application::~Application()
{
}

}