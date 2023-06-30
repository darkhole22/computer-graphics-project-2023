#define VU_LOGGER_TRACE_ENABLED

#include "vulture/core/Logger.h"
#include "vulture/util/ScopeTimer.h"
#include "vulture/core/Application.h"
#include "game/ui/DebugUI.h"
#include "GameManager.h"
#include "terrain/Terrain.h"
#include "vulture/core/Job.h"

using namespace vulture;

namespace game {

class TestGame : public Game
{
public:
	Window* window = nullptr;
	Scene* scene = nullptr;

	Ref<DebugUI> m_DebugUI = nullptr;
	Ref<HUD> m_HUD = nullptr;

	Ref<GameManager> gameManager = nullptr;

#if 1
	String skyboxName = "desert";
#else
	String skyboxName = "rural";
#endif

	void setup() override
	{
		/**********
		 *  SETUP *
		 **********/
		window = Application::getWindow();
		window->setCursorMode(CursorMode::NORMAL);

		scene = Application::getScene();
		EventBus::init();
		setupInputActions();

		/**********
		 * SKYBOX *
		 **********/
		scene->setSkybox(skyboxName);

		/**********
		 *   UI   *
		 **********/
		m_DebugUI = makeRef<DebugUI>();
		m_HUD = makeRef<HUD>();

		/*********
		 * LIGHT *
		 *********/
		scene->getWorld()->directLight.color = glm::vec4(1.0f);
		scene->getWorld()->directLight.direction = glm::normalize(glm::vec3(-1.0f));

		auto* directLightTween = scene->makeTween()->loop();
		directLightTween->addMethodTweener<f32>([this](f32 angle) {
			constexpr float hAngle = glm::radians(10.0f);
			scene->getWorld()->directLight.color =
				glm::vec4(1.0f, 0.2f + 0.7f * std::pow(sin(angle), 3.0f), std::pow(sin(angle), 5.0f) * 0.9f, 1.0f);
			scene->getWorld()->directLight.direction = glm::vec3(cos(angle) * cos(hAngle), sin(angle) * cos(hAngle), sin(hAngle));
		}, 0.0f, glm::radians(180.0f), 60.0f);
		directLightTween->addMethodTweener<f32>([this](f32 angle) {
			constexpr float hAngle = glm::radians(10.0f);
			f32 decay = 1.0f - std::pow(sin(angle), 5.0f);
			scene->getWorld()->directLight.color = static_cast<f32>(1.0f - 0.5f * sin(angle)) *
				glm::vec4(decay * 1.0f, decay * 0.2f, 1.0f - decay, 1.0f);
			scene->getWorld()->directLight.direction = glm::vec3(cos(angle) * cos(hAngle), sin(angle) * cos(hAngle), sin(hAngle));
		}, glm::radians(180.0f), 0.0f, 60.0f);


		Job::submit([](void*)->bool {
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
			gameManager = makeRef<GameManager>(terrainConfig);

			m_HUD->loadingEnded();
		});
	}

	void update(float dt) override
	{
		if (Input::isActionJustPressed("TOGGLE_SKYBOX"))
		{
			static bool useSkybox = false;
			scene->setSkybox(useSkybox ? skyboxName : "rural");
			useSkybox = !useSkybox;
		}

		if (gameManager) gameManager->update(dt);
		m_DebugUI->update(dt);
	}

	~TestGame()
	{
		gameManager.reset();
		EventBus::cleanup();
	}
private:
	static void setupInputActions()
	{
		/**********************************************
		 *                   UI                       *
		 **********************************************/
		InputAction pauseAction{};
		pauseAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_ESCAPE}} };
		pauseAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_START}} };
		Input::setAction("TOGGLE_PAUSE", pauseAction);

		InputAction restartAction{};
		restartAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_R}} };
		restartAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_START}} };
		Input::setAction("RESTART", restartAction);

		/**********************************************
		 *                MOVEMENT                    *
		 **********************************************/
		InputAction leftAction{};
		leftAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_A}} };
		leftAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, AXIS_NEG}}} };
		Input::setAction("MOVE_LEFT", leftAction);

		InputAction rightAction{};
		rightAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_D}} };
		rightAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, AXIS_POS}}} };
		Input::setAction("MOVE_RIGHT", rightAction);

		InputAction upAction{};
		upAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_W}} };
		upAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, AXIS_NEG}}} };
		Input::setAction("MOVE_UP", upAction);

		InputAction downAction{};
		downAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_S}} };
		downAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, AXIS_POS}}} };
		Input::setAction("MOVE_DOWN", downAction);

		/**********************************************
		 *                  ACTIONS                   *
		 **********************************************/
		InputAction fireAction{};
		fireAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_SPACE}} };
		fireAction.mouseBindings = {
				MouseBinding{{GLFW_MOUSE_BUTTON_1}} };
		fireAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER}}
		};
		Input::setAction("FIRE", fireAction);

		InputAction dashAction{};
		dashAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_LEFT_SHIFT}}
		};
		dashAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_LEFT_BUMPER }}
		};
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
		lookLeftAction.mouseAxisBindings = {
				MouseAxisBinding{{Axis::NEGATIVE_X}} };
		Input::setAction("LOOK_LEFT", lookLeftAction);

		InputAction lookRightAction{};
		lookRightAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_RIGHT}},
		};
		lookRightAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_X, AXIS_POS}}} };
		lookRightAction.mouseAxisBindings = {
				MouseAxisBinding{{Axis::POSITIVE_X}} };
		Input::setAction("LOOK_RIGHT", lookRightAction);

		InputAction lookUpAction{};
		lookUpAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_UP}},
		};
		lookUpAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_Y, AXIS_NEG}}} };
		lookUpAction.mouseAxisBindings = {
				MouseAxisBinding{{Axis::NEGATIVE_Y}} };
		Input::setAction("LOOK_UP", lookUpAction);

		InputAction lookDownAction{};
		lookDownAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_DOWN}},
		};
		lookDownAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_Y, AXIS_POS}}} };
		lookDownAction.mouseAxisBindings = {
				MouseAxisBinding{{Axis::POSITIVE_Y}} };
		Input::setAction("LOOK_DOWN", lookDownAction);

		/**********************************************
		 *                  DEBUG                     *
		 **********************************************/

		InputAction toggleInfoAction{};
		toggleInfoAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_F3}},
		};
		toggleInfoAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_BACK}}
		};
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
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_Y}}
		};
		Input::setAction("TOGGLE_SKYBOX", toggleSkyboxAction);

		/**********************************************
		 *                 TERRAIN                    *
		 **********************************************/

		InputAction terrainDownAction{};
		terrainDownAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_K}},
		};
		Input::setAction("TERRAIN_DOWN", terrainDownAction);

		InputAction terrainUpAction{};
		terrainUpAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_L}},
		};
		Input::setAction("TERRAIN_UP", terrainUpAction);
	}
};

} // namespace game

int main()
{
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
