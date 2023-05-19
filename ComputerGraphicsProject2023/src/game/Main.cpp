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

#if 1
	String skyboxName = "desert";
#else
	String skyboxName = "rural";
#endif

	glm::vec3 cameraInitialPosition = glm::vec3(0.0f, 1.5f, 5.0f);

	void setup() override {
		setupInputActions();

		scene = Application::getScene();

		camera = scene->getCamera();
		camera->position = cameraInitialPosition;

		scene->setSkybox(skyboxName);

		scene->getWorld()->directLight.color = glm::vec4(1.0f);
		scene->getWorld()->directLight.direction = glm::normalize(glm::vec3(-1.0f));

		ui = makeRef<UI>();

		auto volcano = makeRef<GameObject>("vulture", "vulture");
		volcano->setPosition(-5.0f, 0.0f, -5.0f);
		scene->addObject(volcano);

		Ref<Tween> tween = scene->makeTween();
		tween->loop();
		tween->addIntervalTweener(0.5f);

		auto parTween = tween->addParallelTweener();

		std::function<void(float)> scaleCallback = [volcano](float size) {
			volcano->setScale(size, size, size);
		};

		std::function<void(float)> lightRotation = [this](float angle) {
			scene->getWorld()->directLight.direction = glm::vec3(sin(angle), 0.0f, cos(angle));
		};

		parTween->addMethodTweener(scaleCallback, 1.0f, 3.0f, 2.0f);
		parTween->addMethodTweener(scaleCallback, 3.0f, 1.0f, 2.0f);

		auto lightTween = scene->makeTween();
		lightTween->loop();
		lightTween->addMethodTweener(lightRotation, 0.0f, glm::radians(360.0f), 10.0f);

		auto f = makeRef<GameObject>("floor", "floor");
		f->setPosition(-50.0f, 0, -50.0f);
		f->setScale(100.0f, 1.0f, 100.0f);
		scene->addObject(f);

		character = makeRef<Character>(makeRef<GameObject>("character", "character"));
		scene->addObject(character->m_GameObject);

		tweenTest = makeRef<TweenTest>();
	}

	void update(float dt) override
	{
		if (Input::isActionJustPressed("TOGGLE_SKYBOX"))
		{
			static bool useSkybox = false;
			scene->setSkybox(useSkybox ? skyboxName : "");
			useSkybox = !useSkybox;
		}

		character->update(dt);
		static float rotation = 0;

		rotation += Input::getAxis("ROTATE_LEFT", "ROTATE_RIGHT") * dt;

		auto camRot = glm::vec4(cameraInitialPosition, 1.0f) * glm::rotate(glm::mat4(1), rotation, {0.0f, 1.0f, 0.0f});
		camera->position = character->m_GameObject->getPosition() + glm::vec3(camRot);
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

		InputAction rotateLeftAction{};
		rotateLeftAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_Q}},
		};
		Input::setAction("ROTATE_LEFT", rotateLeftAction);

		InputAction rotateRightAction{};
		rotateRightAction.keyboardBindings = {
				KeyboardBinding{{GLFW_KEY_E}},
		};
		Input::setAction("ROTATE_RIGHT", rotateRightAction);

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
