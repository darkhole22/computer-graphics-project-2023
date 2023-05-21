#define VU_LOGGER_TRACE_ENABLED

#include "vulture/core/Logger.h"
#include "vulture/core/Application.h"
#include "UI.h"
#include "GameManager.h"

using namespace vulture;

namespace game {

class TestGame : public Game
{
public:
	Scene* scene = nullptr;
	float c_CameraHeight = 1.5f;

	Ref<UI> ui = nullptr;
	Ref<GameManager> gameManager = nullptr;

#if 1
	String skyboxName = "desert";
#else
	String skyboxName = "rural";
#endif

	void setup() override
	{
		scene = Application::getScene();

		setupInputActions();

		/**********
		 * SKYBOX *
		 **********/
		scene->setSkybox(skyboxName);

		/**********
		 *   UI   *
		 **********/
		ui = makeRef<UI>();

		/***********
		 * VOLCANO *
		 ***********/
		auto volcano = makeRef<GameObject>("vulture");
		volcano->transform.setPosition(-5.0f, 0.0f, -5.0f);
		scene->addObject(volcano);

		Ref<Tween> tween = scene->makeTween();
		tween->loop();
		tween->addIntervalTweener(0.5f);

		std::function<void(float)> scaleCallback = [volcano](float size) {
			volcano->transform.setScale(size, size, size);
		};

		tween->addMethodTweener(scaleCallback, 3.0f, 6.0f, 2.0f);
		tween->addMethodTweener(scaleCallback, 6.0f, 3.0f, 2.0f);

		/*********
		 * LIGHT *
		 *********/
		scene->getWorld()->directLight.color = glm::vec4(1.0f);
		scene->getWorld()->directLight.direction = glm::normalize(glm::vec3(-1.0f));

		std::function<void(float)> lightRotation = [this](float angle) {
			scene->getWorld()->directLight.direction = glm::vec3(sin(angle), 0.0f, cos(angle));
		};

		auto lightTween = scene->makeTween();
		lightTween->loop();
		lightTween->addMethodTweener(lightRotation, 0.0f, glm::radians(360.0f), 10.0f);

		/*********
		 * FLOOR *
		 *********/

		auto f = makeRef<GameObject>("floor");
		f->transform.setPosition(-50.0f, 0, -50.0f);
		f->transform.setScale(100.0f, 1.0f, 100.0f);
		scene->addObject(f);

		/**************
		 * GAME LOGIC *
		 **************/
		 gameManager = makeRef<GameManager>();
	}

	void update(float dt) override
	{
		if (Input::isActionJustPressed("TOGGLE_SKYBOX"))
		{
			static bool useSkybox = false;
			scene->setSkybox(useSkybox ? skyboxName : "rural");
			useSkybox = !useSkybox;
		}

		if (Input::isKeyPressed(GLFW_KEY_H))
		{
			scene->getCamera()->lookAt(glm::vec3(-5.0f, 5.0f, -5.0f));
		}

		gameManager->update(dt);

		ui->update(dt);
	}

private:
	static void setupInputActions()
	{
		/**********************************************
		 *                MOVEMENT                    *
		 **********************************************/
		InputAction leftAction{};
		leftAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_A}} };
		leftAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_NEG}}} };
		Input::setAction("MOVE_LEFT", leftAction);

		InputAction rightAction{};
		rightAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_D}} };
		rightAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_POS}}} };
		Input::setAction("MOVE_RIGHT", rightAction);

		InputAction upAction{};
		upAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_W}} };
		upAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_NEG}}} };
		Input::setAction("MOVE_UP", upAction);

		InputAction downAction{};
		downAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_S}} };
		downAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_POS}}} };
		Input::setAction("MOVE_DOWN", downAction);

		/**********************************************
		 *                  ACTIONS                   *
		 **********************************************/
		InputAction fireAction{};
		fireAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_SPACE}} };
		Input::setAction("FIRE", fireAction);

		/**********************************************
		 *                  CAMERA                    *
		 **********************************************/

		InputAction rotateLeftAction{};
		rotateLeftAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_LEFT}},
		};
		rotateLeftAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_X, GAMEPAD_AXIS_NEG}}} };
		Input::setAction("ROTATE_LEFT", rotateLeftAction);

		InputAction rotateRightAction{};
		rotateRightAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_RIGHT}},
		};
		rotateRightAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_X, GAMEPAD_AXIS_POS}}} };
		Input::setAction("ROTATE_RIGHT", rotateRightAction);

		InputAction rotateUpAction{};
		rotateUpAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_UP}},
		};
		rotateUpAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_Y, GAMEPAD_AXIS_NEG}}} };
		Input::setAction("ROTATE_UP", rotateUpAction);

		InputAction rotateDownAction{};
		rotateDownAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_DOWN}},
		};
		rotateDownAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_Y, GAMEPAD_AXIS_POS}}} };
		Input::setAction("ROTATE_DOWN", rotateDownAction);

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
