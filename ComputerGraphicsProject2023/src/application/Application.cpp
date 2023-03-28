#include "Application.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <chrono>

#include "event/WindowEvents.h"

namespace computergraphicsproject {

void handleKeyPressed(const KeyPressedEvent& e)
{
    std::cout << "Pressed: " << e.keyCode << std::endl;
}

Application::Application(const char* name, uint32_t width, uint32_t height)
	: c_name(name), m_Window(name, width, height), m_Renderer(m_Window), m_Scene(m_Renderer)
{
    m_Window.addCallback(handleKeyPressed);
}

void Application::run()
{
	auto startTime = std::chrono::high_resolution_clock::now();
	float lastTime = 0.0f;

	float fps = 60.0f;
	float delta = 0;

	const float WRITE_FPS_TIMEOUT = 0.5; // seconds
	const float FPS_AVG_WEIGHT = 0.1; // 0 <= x <= 1

	setup();
	while (!m_Window.shouldClose()) {
		m_Window.pollEvents();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		float deltaT = time - lastTime;
		lastTime = time;

		delta += deltaT;
		fps = fps * (1.0f - FPS_AVG_WEIGHT) + (1.0 / deltaT) * FPS_AVG_WEIGHT;

        if (delta > WRITE_FPS_TIMEOUT)
		{
			std::cout << '\r' << "[FPS]: " << fps << "      " << std::flush;
			delta -= 1.0f;
		}

		update();

		m_Scene.render(m_Renderer.getRenderTarget());
	}
}

Application::~Application()
{
}

}