#pragma once

#include <stdexcept>
#include <string>
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulture/core/Game.h"
#include "vulture/core/Core.h"

#include "vulture/renderer/Window.h"
#include "vulture/renderer/Renderer.h"
#include "vulture/scene/Scene.h"

namespace vulture {

struct AppConfig
{
    const char *name;
    uint32_t width;
    uint32_t height;
};

class Application
{
public:
    NO_COPY(Application)

    inline static void launch(Game& game, AppConfig config)
    {
        if (!s_Instance.expired())
        {
            throw std::runtime_error("Application is already running!");
        }

        auto instance = std::shared_ptr<Application>(new Application(game, config));
        s_Instance = instance;

        instance->run();
    }

    inline static std::shared_ptr<Application> getInstance() { return s_Instance.lock(); }

    inline static const Scene& getScene() { return getInstance()->m_Scene; }

    ~Application();
private:
    Application(Game& game, AppConfig config);

    static std::weak_ptr<Application> s_Instance;

    const std::string c_Name;
	Window m_Window;
	Renderer m_Renderer;
	Scene m_Scene;
    Game* m_Game;

    void run();

    void setup();
    void update(float delta);
};

} // namespace vulture
