#define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Application.h"
#include "vulture/core/Job.h"
#include "vulture/core/Logger.h"
#include "vulture/util/ScopeTimer.h"

#include "game/GameManager.h"
#include "game/terrain/Terrain.h"
#include "game/ui/DebugUI.h"

namespace game {

using namespace vulture;

class TestGame : public Game
{
public:
	void setup() override
	{
		/**********
		 *  SETUP *
		 **********/
		m_Window = Application::getWindow();
		m_Window->setCursorMode(CursorMode::NORMAL);

		m_Scene = Application::getScene();
		EventBus::init();
		setupInputActions();

		/**********
		 * SKYBOX *
		 **********/
		m_Scene->setSkybox(SKYBOX_DAY);

		/**********
		 *   UI   *
		 **********/
		m_DebugUI = makeRef<DebugUI>();
		m_HUD = makeRef<HUD>();

		/*********
		 * LIGHT *
		 *********/
		setupLight();

		/***********
		 * LOADING *
		 ***********/
		Job::submit([](void*) -> bool {
			using namespace std::chrono_literals;

			std::this_thread::sleep_for(100ms);
			return true;
		}, nullptr, [this](bool, void*) {
			/***********
			* TERRAIN *
			***********/
			TerrainGenerationConfig terrainConfig{};
			terrainConfig.noiseScale = 2;
			terrainConfig.heightScale = 250;

			/**************
			* GAME LOGIC *
			**************/
			m_GameManager = makeRef<GameManager>(terrainConfig);

			m_HUD->loadingEnded();
		});
	}

	void update(float dt) override
	{
		if (Input::isActionJustPressed("TOGGLE_SKYBOX"))
		{
			static bool useSkybox = false;
			m_Scene->setSkybox(useSkybox ? SKYBOX_DAY : "");
			useSkybox = !useSkybox;
		}

		if (m_GameManager) m_GameManager->update(dt);
		m_DebugUI->update(dt);
	}

	~TestGame()
	{
		m_GameManager.reset();
		EventBus::cleanup();
	}

private:
	static inline const String SKYBOX_MORNING = "skybox/SkyMorning";
	static inline const String SKYBOX_DAY = "skybox/SkyAfterNoon";
	static inline const String SKYBOX_SUNSET = "skybox/SkySunSet";
	static inline const String SKYBOX_MIDNIGHT = "skybox/SkyMidNight";
	static inline const String SKYBOX_NIGHT = "skybox/SkyNight";

	Window* m_Window = nullptr;
	Scene* m_Scene = nullptr;

	Ref<DebugUI> m_DebugUI = nullptr;
	Ref<HUD> m_HUD = nullptr;
	Ref<GameManager> m_GameManager = nullptr;

	void setupLight()
	{
		m_Scene->getWorld()->directLight.color = glm::vec4(1.0f);
		m_Scene->getWorld()->directLight.direction = glm::normalize(glm::vec3(-1.0f));

		auto lightFun = [this](f32 angle) {
			constexpr float hAngle = glm::radians(10.0f);

			m_Scene->getWorld()->directLight.color = glm::vec4(1.0f, 0.2f + 0.7f * std::pow(sin(angle), 3.0f), std::pow(sin(angle), 5.0f) * 0.9f, 1.0f);
			m_Scene->getWorld()->directLight.direction = glm::vec3(cos(angle) * cos(hAngle), sin(angle) * cos(hAngle), sin(hAngle));
		};

		auto lightInvFun = [this](f32 angle) {
			constexpr float hAngle = glm::radians(10.0f);
			f32 decay = 1.0f - std::pow(sin(angle), 5.0f);

			m_Scene->getWorld()->directLight.color = static_cast<f32>(1.0f - 0.5f * sin(angle)) * glm::vec4(decay * 1.0f, decay * 0.2f, 1.0f - decay, 1.0f);
			m_Scene->getWorld()->directLight.direction = glm::vec3(cos(angle) * cos(hAngle), sin(angle) * cos(hAngle), sin(hAngle));
		};

		auto* directLightTween = m_Scene->makeTween()->loop();

		// Morning
		directLightTween->addCallbackTweener([this]() { m_Scene->setSkybox(SKYBOX_MORNING); });
		directLightTween->addMethodTweener<f32>(lightFun, 0.0f, glm::radians(30.0f), 15.0f);

		// Day
		directLightTween->addCallbackTweener([this]() { m_Scene->setSkybox(SKYBOX_DAY); });
		directLightTween->addMethodTweener<f32>(lightFun, glm::radians(30.0f), glm::radians(150.0f), 60.0f);

		// Sunset
		directLightTween->addCallbackTweener([this]() { m_Scene->setSkybox(SKYBOX_SUNSET); });
		directLightTween->addMethodTweener<f32>(lightFun, glm::radians(150.0f), glm::radians(180.0f), 10.0f);

		// Midnight
		directLightTween->addCallbackTweener([this]() { m_Scene->setSkybox(SKYBOX_MIDNIGHT); });
		directLightTween->addMethodTweener<f32>(lightInvFun, glm::radians(180.0f), glm::radians(30.0f), 10.0f);

		// Night
		directLightTween->addCallbackTweener([this]() { m_Scene->setSkybox(SKYBOX_NIGHT); });
		directLightTween->addMethodTweener<f32>(lightInvFun, glm::radians(150.0f), 0.0f, 50.0f);
	}

	void setupInputActions()
	{
		/**********************************************
		 *                   UI                       *
		 **********************************************/
		InputAction pauseAction{};
		pauseAction.keyboardBindings = { KeyboardBinding{{GLFW_KEY_ESCAPE}} };
		pauseAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_START}} };
		Input::setAction("TOGGLE_PAUSE", pauseAction);

		InputAction restartAction{};
		restartAction.keyboardBindings = { KeyboardBinding{{GLFW_KEY_R}} };
		restartAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_START}} };
		Input::setAction("RESTART", restartAction);

		/**********************************************
		 *                MOVEMENT                    *
		 **********************************************/
		InputAction leftAction{};
		leftAction.keyboardBindings = { KeyboardBinding{{GLFW_KEY_A}} };
		leftAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, AXIS_NEG}}} };
		Input::setAction("MOVE_LEFT", leftAction);

		InputAction rightAction{};
		rightAction.keyboardBindings = { KeyboardBinding{{GLFW_KEY_D}} };
		rightAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, AXIS_POS}}} };
		Input::setAction("MOVE_RIGHT", rightAction);

		InputAction upAction{};
		upAction.keyboardBindings = { KeyboardBinding{{GLFW_KEY_W}} };
		upAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, AXIS_NEG}}} };
		Input::setAction("MOVE_UP", upAction);

		InputAction downAction{};
		downAction.keyboardBindings = { KeyboardBinding{{GLFW_KEY_S}} };
		downAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, AXIS_POS}}} };
		Input::setAction("MOVE_DOWN", downAction);

		/**********************************************
		 *                  ACTIONS                   *
		 **********************************************/
		InputAction fireAction{};
		fireAction.keyboardBindings = { KeyboardBinding{{GLFW_KEY_SPACE}} };
		fireAction.mouseBindings = { MouseBinding{{GLFW_MOUSE_BUTTON_1}} };
		fireAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER}} };
		Input::setAction("FIRE", fireAction);

		InputAction dashAction{};
		dashAction.keyboardBindings = { KeyboardBinding{{GLFW_KEY_LEFT_SHIFT}} };
		dashAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_LEFT_BUMPER}} };
		Input::setAction("DASH", dashAction);

		/**********************************************
		 *                  CAMERA                    *
		 **********************************************/

		InputAction lookLeftAction{};
		lookLeftAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_LEFT}},
		};
		lookLeftAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_X, AXIS_NEG}}} };
		lookLeftAction.mouseAxisBindings = { MouseAxisBinding{{Axis::NEGATIVE_X}} };
		Input::setAction("LOOK_LEFT", lookLeftAction);

		InputAction lookRightAction{};
		lookRightAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_RIGHT}},
		};
		lookRightAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_X, AXIS_POS}}} };
		lookRightAction.mouseAxisBindings = { MouseAxisBinding{{Axis::POSITIVE_X}} };
		Input::setAction("LOOK_RIGHT", lookRightAction);

		InputAction lookUpAction{};
		lookUpAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_UP}},
		};
		lookUpAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_Y, AXIS_NEG}}} };
		lookUpAction.mouseAxisBindings = { MouseAxisBinding{{Axis::NEGATIVE_Y}} };
		Input::setAction("LOOK_UP", lookUpAction);

		InputAction lookDownAction{};
		lookDownAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_DOWN}},
		};
		lookDownAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_Y, AXIS_POS}}} };
		lookDownAction.mouseAxisBindings = { MouseAxisBinding{{Axis::POSITIVE_Y}} };
		Input::setAction("LOOK_DOWN", lookDownAction);

		/**********************************************
		 *                  DEBUG                     *
		 **********************************************/

		InputAction toggleInfoAction{};
		toggleInfoAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_F3}},
		};
		toggleInfoAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_BACK}} };
		Input::setAction("TOGGLE_INFO", toggleInfoAction);

		InputAction toggleGodmodeAction{};
		toggleGodmodeAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_F1}},
		};
		Input::setAction("TOGGLE_GODMODE", toggleGodmodeAction);

		InputAction toggleSkyboxAction{};
		toggleSkyboxAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_M}},
		};
		toggleSkyboxAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_Y}} };
		Input::setAction("TOGGLE_SKYBOX", toggleSkyboxAction);
	}
};

} // namespace game

int main()
{
	using namespace vulture;

	Logger logger("output.log");
	try
	{
		Ref<Application> app; // The game must be destroyed before the application
		{
			game::TestGame game;

			app = Application::launch(game, vulture::AppConfig{ "Vulture demo", 800, 600 });
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception& exception)
	{
		VUFATAL("An exception was thrown.\nMessage: %s\n\nShutting down!", exception.what());
	}

	return EXIT_FAILURE;
}
