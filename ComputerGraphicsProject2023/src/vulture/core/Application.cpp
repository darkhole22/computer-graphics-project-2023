#include "Application.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <chrono>

namespace vulture {

std::weak_ptr<Application> Application::s_Instance = std::weak_ptr<Application>();

Application::Application(Game& game, AppConfig config) :
    c_Name(config.name),
    m_Window(config.name, config.width, config.height),
    m_Renderer(m_Window),
    m_Scene(m_Renderer),
    m_Game(&game)
{
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

		update(deltaT);

		m_Scene.render(m_Renderer.getRenderTarget());
	}
}

void Application::setup()
{
    m_Game->setup();
}

void Application::update(float delta)
{
    m_Game->update(delta);
}

Application::~Application() = default;

} // namespace vulture