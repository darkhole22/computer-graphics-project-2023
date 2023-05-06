#define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Logger.h"
#include "vulture/core/Application.h"
#include "Character.h"
#include "UI.h"
#include "TweenTest.h"

using namespace vulture;

namespace game
{

class TestGame : public Game {
public:
	Scene *scene = nullptr;
	Camera *camera = nullptr;
	
	Ref<UI> ui = nullptr;
	Ref<Character> character = nullptr;
	Ref<TweenTest> tweenTest;

	glm::vec3 cameraInitialPosition = glm::vec3(0.0f, 1.5f, 5.0f);

	void setup() override {
		setupInputActions();

		scene = Application::getScene();

		camera = scene->getCamera();
		camera->position = cameraInitialPosition;

		ui = makeRef<UI>();

		auto volcano = makeRef<GameObject>("res/models/vulture.obj", "res/textures/vulture.png");
		volcano->setPosition(-5.0f, 0.0f, -5.0f);
		scene->addObject(volcano);

		Ref<Tween> tween = scene->makeTween();
		tween->loop();
		tween->addIntervalTweener(0.5f);

		std::function<void(float)> callback = [volcano](float size) {
			volcano->setScale(size, size, size);
		};

		tween->addMethodTweener(callback, 1.0f, 3.0f, 2.0f);
		tween->addMethodTweener(callback, 3.0f, 1.0f, 2.0f);

		auto f = makeRef<GameObject>("res/models/floor.obj", "res/textures/floor.png");
		f->setPosition(-50.0f, 0, -50.0f);
		f->setScale(100.0f, 1.0f, 100.0f);
		scene->addObject(f);

		character = makeRef<Character>(makeRef<GameObject>("res/models/character.obj", "res/textures/character.png"));
		scene->addObject(character->m_GameObject);

		tweenTest = makeRef<TweenTest>();
	}

	void update(float dt) override
	{
		character->update(dt);
		camera->position = character->m_GameObject->getPosition() + cameraInitialPosition;
		camera->lookAt(character->m_GameObject->getPosition());

		tweenTest->update(dt);
		ui->update(dt);
	}

private:
	static void setupInputActions()
	{
		InputAction leftAction{};
		leftAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_A}},
				KeyboardBinding{{GLFW_KEY_LEFT}}};
		leftAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_LEFT}}};
		leftAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_NEG}}}};
		Input::setAction("MOVE_LEFT", leftAction);

		InputAction rightAction{};
		rightAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_D}},
				KeyboardBinding{{GLFW_KEY_RIGHT}}};
		rightAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_RIGHT}}};
		rightAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_POS}}}};
		Input::setAction("MOVE_RIGHT", rightAction);

		InputAction upAction{};
		upAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_W}},
				KeyboardBinding{{GLFW_KEY_UP}}};
		upAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_UP}}};
		upAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_NEG}}}};
		Input::setAction("MOVE_UP", upAction);

		InputAction downAction{};
		downAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_S}},
				KeyboardBinding{{GLFW_KEY_DOWN}}};
		downAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_DOWN}}};
		downAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_POS}}}};
		Input::setAction("MOVE_DOWN", downAction);

		InputAction toggleInfoAction{};
		toggleInfoAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_F3}},
		};
		toggleInfoAction.gamepadButtonBindings = {
				GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_BACK}}
		};
		toggleInfoAction.gamepadAxisBindings = {
				GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_RIGHT_X, GAMEPAD_AXIS_POS}}}
		};
		Input::setAction("TOGGLE_INFO", toggleInfoAction);
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

			app = Application::launch(game, vulture::AppConfig{"Vulture demo", 800, 600});
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception &exception)
	{
		VUFATAL("An exception was thrown.\nMessage: %s\n\nShutting down!", exception.what());
	}

	return EXIT_FAILURE;
}
