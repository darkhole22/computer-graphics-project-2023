#define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Logger.h"
#include "vulture/core/Game.h"
#include "vulture/core/Application.h"
#include "vulture/core/Core.h"
#include "vulture/core/Input.h"

#include "vulture/scene/Tween.h"

using namespace vulture;

struct ModelBufferObject
{
	glm::mat4 model = glm::mat4(1);
};

class TestGame : public Game
{
public:
	Scene *scene = nullptr;
	Camera *camera = nullptr;
	UIHandler *handlerUI = nullptr;
	Ref<DescriptorSetLayout> descriptorSetLayout;
	PipelineHandle pipeline = -1;
	Ref<Model> model;
	Uniform<ModelBufferObject> objUniform;
	Ref<Texture> objTexture;
	Ref<TextureSampler> objTextureSampler;
	Ref<UIText> text;
	Ref<UIText> text2;
	
	Ref<Tween> tween;
	Ref<UIText> tweenText;
	glm::vec2 tweenValue = {10, 10};
	u64 tweenValue2 = 15;

	void setup() override
	{
		setupInputActions();

		scene = Application::getScene();
		camera = scene->getCamera();
		handlerUI = scene->getUIHandle();

		descriptorSetLayout = Ref<DescriptorSetLayout>(new DescriptorSetLayout());
		descriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		descriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		descriptorSetLayout->create();

		pipeline = scene->makePipeline("res/shaders/baseVert.spv", "res/shaders/baseFrag.spv", descriptorSetLayout);

		model = Ref<Model>(Model::make("res/models/vulture.obj"));
		objUniform = Renderer::makeUniform<ModelBufferObject>();
		objTexture = Ref<Texture>(new Texture("res/textures/vulture.png"));
		objTextureSampler = makeRef<TextureSampler>(*objTexture);

		scene->addObject(pipeline, model, descriptorSetLayout, { objUniform, *objTextureSampler });

		camera->position = glm::vec3(10, 5, 10);

		text = handlerUI->makeText("FPS");
		text2 = handlerUI->makeText("Frame Time");
		text2->setPosition({20, 50});
		text->setVisible(false);
		text2->setVisible(false);

		tweenText = handlerUI->makeText("Tween: running");
		tweenText->setPosition({ 20, 100 });
		std::function<void(float)> callback = [this](float size) {
			tweenText->setSize(size);
		};
		tween = scene->makeTween();
		tween->loop();
		tween->addIntervalTweener(0.5);
		auto parallel = tween->addParallelTweener();
		{
			auto sequential = parallel->addSequentialTweener();
			{
				sequential->addValueTweener(&tweenValue2, 3000ULL, 0.5);
				sequential->addValueTweener(&tweenValue2, 150ULL, 0.5);
				sequential->addCallbackTweener([this]() {
					const glm::vec3 colors[3] = {
						{1.0f, 0.0f, 0.0f},
						{0.0f, 1.0f, 0.0f},
						{0.0f, 0.0f, 1.0f}
					};
					static u64 index = 0;
					tweenText->setColor(colors[index]);
					index = (index + 1) % 3;
				});
			}
			parallel->addValueTweener(&tweenValue, { 20, 300 }, 1);
			parallel->addMethodTweener(callback, 22.0f, 40.0f, 1.0f);
		}
		tween->addIntervalTweener(0.5);
		tween->addValueTweener(&tweenValue, { 250, 250 }, 1.5f);
		tween->addValueTweener(&tweenValue, { 20, 100 }, 1.0f);
		tween->addMethodTweener(callback, 40.0f, 22.0f, 0.5f);

	}

	void update(float dt) override
	{
		static float time = 0;
		time += dt;

		float x = Input::getAxis("MOVE_LEFT", "MOVE_RIGHT");
		float y = Input::getAxis("MOVE_DOWN", "MOVE_UP");

		{
			static glm::vec3 objPos{};
			objPos += glm::vec3(x * SPEED * dt, y * SPEED * dt, 0.0f);
			objUniform->model = glm::translate(glm::mat4(1), objPos);
			camera->lookAt(objPos);
		}

		{
			static bool wasKPressed = false;
			bool isKPressed = Input::isKeyPressed(GLFW_KEY_K);
			if (isKPressed && !wasKPressed)
			{
				tween->stop();
			}
			wasKPressed = isKPressed;

			tweenText->setText(stringFormat("Time %f\nValue: %llu", time, tweenValue2));
			tweenText->setPosition(tweenValue);
		}

		{
			// Press F3 to toggle info
			static bool wasF3Pressed = false;
			bool isF3Pressed = Input::isKeyPressed(GLFW_KEY_F3);
			if (isF3Pressed && !wasF3Pressed)
			{
				text->setVisible(!text->isVisible());
				text2->setVisible(!text2->isVisible());
			}
			wasF3Pressed = isF3Pressed;

			static float fps = 0.0f;
			static float delta = 0;

			static const float WRITE_FPS_TIMEOUT = 0.5; // seconds
			static const float FPS_AVG_WEIGHT = 0.1f;	// 0 <= x <= 1

			delta += dt;
			fps = fps * (1.0f - FPS_AVG_WEIGHT) + (1.0f / dt) * FPS_AVG_WEIGHT;

			if (delta > WRITE_FPS_TIMEOUT)
			{
				text->setText(stringFormat("FPS: %.0f", fps));
				text2->setText(stringFormat("Frame time: %.4fms", dt * 1000));

				delta -= WRITE_FPS_TIMEOUT;
			}
		}
	}

private:
	const float SPEED = 10;

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

int main()
{
	Logger logger("output.log");
	try
	{
		Ref<Application> app; // The game must be destroyed before the application
		{
			TestGame game;

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