#include "Application.h"
#include "Input.h"

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
	m_Game(&game)
{
	if (!Renderer::init(config.name, m_Window))
	{
		throw std::runtime_error("Unable to initialize the Renderer.");
	}

	m_Scene = makeRef<Scene>();
}

void Application::run()
{
	auto startTime = std::chrono::high_resolution_clock::now();
	float lastTime = 0.0f;

	setup();

	while (!m_Window.shouldClose()) {
		Input::reset();
		m_Window.pollEvents();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		float deltaT = time - lastTime;
		lastTime = time;

		update(deltaT);

		m_Scene->render(Renderer::getFrameContext(), deltaT);
	}

	Renderer::waitIdle();
}

void Application::setup()
{
	Input::initialize(m_Window);
	m_Game->setup();
}

void Application::update(float delta)
{
	m_Game->update(delta);
}

Application::~Application()
{
	m_Scene.reset();
	Input::cleanup();

	Renderer::cleanup();
}

} // namespace vulture