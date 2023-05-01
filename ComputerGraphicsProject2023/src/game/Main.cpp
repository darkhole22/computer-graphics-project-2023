#define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Logger.h"
#include "vulture/core/Game.h"
#include "vulture/core/Application.h"
#include "vulture/core/Core.h"
#include "vulture/core/Input.h"
#include "Volcano.h"
#include "UI.h"

#include <unordered_map>

using namespace vulture;

namespace game
{

class TestGame : public Game {
public:
	Scene *scene = nullptr;
	Camera *camera = nullptr;
	UI* ui = nullptr;
	Volcano *v = nullptr;

	void setup() override {
		setupInputActions();

		scene = Application::getScene();
		camera = scene->getCamera();

		ui = new UI();

		v = new Volcano(makeRef<GameObject>("res/models/vulture.obj", "res/textures/vulture.png"));
		scene->addObject(v->m_GameObject);

		camera->position = glm::vec3(10, 5, 10);
	}

	void update(float dt) override {
		v->update(dt);
		camera->lookAt(v->m_GameObject->getPosition());

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