#include "Application.h"
#include "Input.h"
#include "vulture/core/Job.h"
#include "vulture/util/ScopeTimer.h"

#include <iostream>

namespace vulture {

WRef<Application> Application::s_Instance{};

Application::Application(Game& game, AppConfig config) :
	c_Name(config.name),
	m_Window(config.name, config.width, config.height),
	m_Game(&game)
{
	DEBUG_TIMER("Application creation");
	if (!Job::init())
	{
		throw std::runtime_error("Unable to initialize the Job System.");
	}

	if (!Renderer::init(config.name, m_Window))
	{
		throw std::runtime_error("Unable to initialize the Renderer.");
	}

	m_Scene = makeRef<Scene>();
}

void Application::run()
{
	SystemTimer timer;

	setup();

	while (!m_Window.shouldClose())
	{
		Input::reset();
		m_Window.pollEvents();

		Job::process();

		f64 dt = static_cast<f64>(timer.restart()) / 1000000000.0;
		f32 deltaT = static_cast<f32>(dt);

		update(deltaT);

		m_Scene->render(Renderer::getFrameContext(), deltaT);
	}

	Renderer::waitIdle();
}

void Application::setup()
{
	Input::initialize(m_Window);
	DEBUG_TIMER("Game creation");
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

	Job::cleanup();
}

} // namespace vulture