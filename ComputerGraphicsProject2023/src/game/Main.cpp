#define VU_LOGGER_TRACE_ENABLED

#include "vulture/core/Logger.h"
#include "vulture/core/Application.h"
#include "game/ui/DebugUI.h"
#include "GameManager.h"
#include "terrain/Terrain.h"

using namespace vulture;

namespace game {

class TestGame : public Game
{
public:
	Window* window = nullptr;
	Scene* scene = nullptr;
	float c_CameraHeight = 1.5f;

	Ref<DebugUI> m_DebugUI = nullptr;
	Ref<HUD> m_HUD = nullptr;

	Ref<GameManager> gameManager = nullptr;
	Ref<Terrain> terrain = nullptr;

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
		window->setCursorMode(CursorMode::DISABLED);

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

		/***********
		 * VOLCANO *
		 ***********/
		auto volcano = makeRef<GameObject>("vulture");
		volcano->transform.setPosition(100.0f, 50.0f, 100.0f);
		scene->addObject(volcano);

		auto tween = scene->makeTween()->loop();

		std::function<void(float)> scaleCallback = [volcano](float size) {
			volcano->transform.setScale(size, size, size);
		};

		tween->addMethodTweener(scaleCallback, 1.0f, 3.0f, 2.0f);
		tween->addMethodTweener(scaleCallback, 3.0f, 1.0f, 2.0f);

		/*********
		 * LIGHT *
		 *********/
		scene->getWorld()->directLight.color = glm::vec4(1.0f);
		scene->getWorld()->directLight.direction = glm::normalize(glm::vec3(-1.0f));

		scene->makeTween()->loop()->addMethodTweener<float>([this](float angle) {
			constexpr float hAngle = glm::radians(10.0f);
			scene->getWorld()->directLight.direction = glm::vec3(sin(angle) * cos(hAngle), sin(hAngle), cos(angle) * cos(hAngle));
		}, 0.0f, glm::radians(360.0f), 10.0f);

		/***********
		 * TERRAIN *
		 ***********/
		TerrainGenerationConfig terrainConfig{};
		terrainConfig.noiseScale = 2;
		terrainConfig.heightScale = 250;
		terrain = makeRef<Terrain>(terrainConfig);
		
		/**************
		 * GAME LOGIC *
		 **************/
		gameManager = makeRef<GameManager>(terrain);
	}

	void update(float dt) override
	{
		if (Input::isActionJustPressed("TOGGLE_SKYBOX"))
		{
			static bool useSkybox = false;
			scene->setSkybox(useSkybox ? skyboxName : "rural");
			useSkybox = !useSkybox;
		}

		gameManager->update(dt);

		m_DebugUI->update(dt);

		auto cameraPos = scene->getCamera()->position;
		terrain->setReferencePosition({ cameraPos.x, cameraPos.z });

		terrain->update(dt);
	}

	~TestGame()
	{
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
		Input::setAction("FIRE", fireAction);

		InputAction inputModeAction{};
		inputModeAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_I}} };
		Input::setAction("TOGGLE_INPUT_MODE", inputModeAction);

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
