#pragma once

#include <stdexcept>
#include <string>
#include <memory>

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

    inline static Ref<Application> launch(Game& game, AppConfig config)
    {
        if (!s_Instance.expired())
        {
            throw std::runtime_error("Application is already running!");
        }

        auto instance = Ref<Application>(new Application(game, config));
        s_Instance = instance;

        instance->run();

        return getInstance();
    }

    inline static Ref<Application> getInstance() { return s_Instance.lock(); }

    inline static Scene* getScene() { return getInstance()->m_Scene.get(); }
    // inline static Ref<DescriptorSetLayout> makeDescriptorSetLayout() { return getInstance()->m_Renderer.makeDescriptorSetLayout(); }
    // inline static Ref<Model> makeModel(const String& path) { return getInstance()->m_Renderer.makeBaseModel(path); }
    // template <class T> inline static Uniform<T> makeUniform() { return getInstance()->m_Renderer.makeUniform<T>(); }
    // inline static Ref<Texture> makeTexture(const String& path) { return getInstance()->m_Renderer.makeTexture(path); }
    
    ~Application();
private:
    Application(Game& game, AppConfig config);

    static WRef<Application> s_Instance;

    const String c_Name;
	Window m_Window;
	Ref<Scene> m_Scene;
    Game* m_Game;

    void run();

    void setup();
    void update(float delta);
};

} // namespace vulture
