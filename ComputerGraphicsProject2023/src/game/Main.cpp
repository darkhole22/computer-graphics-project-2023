#include <iostream>
#include <cstdlib>

#include "vulture/core/Game.h"
#include "vulture/core/Application.h"
#include "vulture/core/Input.h"

using namespace vulture;

struct ModelBufferObject
{
	glm::mat4 model = glm::mat4(1);
};

class TestGame : public Game
{
public:
	Scene* scene = nullptr;
	Camera* camera = nullptr;
	UIHandler* handlerUI = nullptr;
	Ref<DescriptorSetLayout> descriptorSetLayout;
	PipelineHandle pipeline = -1;
	Ref<Model> model;
	Uniform<ModelBufferObject> objUniform;
	Ref<Texture> objTexture;
	Ref<UIText> text;

	void setup() override
	{
		InputAction leftAction{};
		leftAction.keyboardBindings = { 
			KeyboardBinding{{GLFW_KEY_A}}, 
			KeyboardBinding{{GLFW_KEY_LEFT}} 
		};
		leftAction.gamepadButtonBindings = { 
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_LEFT}}
		};
		leftAction.gamepadAxisBindings = { 
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_NEG}}}
		};
		Input::setAction("MOVE_LEFT", leftAction);

		InputAction rightAction{};
		rightAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_D}},
			KeyboardBinding{{GLFW_KEY_RIGHT}}
		};
		rightAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_RIGHT}}
		};
		rightAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_POS}}}
		};
		Input::setAction("MOVE_RIGHT", rightAction);

		InputAction upAction{};
		upAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_W}},
			KeyboardBinding{{GLFW_KEY_UP}}
		};
		upAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_UP}}
		};
		upAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_NEG}}}
		};
		Input::setAction("MOVE_UP", upAction);

		InputAction downAction{};
		downAction.keyboardBindings = {
			KeyboardBinding{{GLFW_KEY_S}},
			KeyboardBinding{{GLFW_KEY_DOWN}}
		};
		downAction.gamepadButtonBindings = {
			GamepadButtonBinding{{GLFW_GAMEPAD_BUTTON_DPAD_DOWN}}
		};
		downAction.gamepadAxisBindings = {
			GamepadAxisBinding{{{GLFW_GAMEPAD_AXIS_LEFT_Y, GAMEPAD_AXIS_POS}}}
		};
		Input::setAction("MOVE_DOWN", downAction);

		scene = Application::getScene();
		camera = scene->getCamera();
		handlerUI = scene->getUIHandle();

		descriptorSetLayout = Application::makeDescriptorSetLayout();
		descriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		descriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		descriptorSetLayout->create();

		pipeline = scene->makePipeline("res/shaders/baseVert.spv", "res/shaders/baseFrag.spv", descriptorSetLayout);

		model = Application::makeModel("res/models/vulture.obj");
		objUniform = Application::makeUniform<ModelBufferObject>();
		objTexture = Application::makeTexture("res/textures/vulture.png");

		scene->addObject(pipeline, model, descriptorSetLayout, { objUniform , *objTexture });

		camera->position = glm::vec3(10, 5, 10);

		text = handlerUI->makeText("0123546879");
		text->setSize(30);
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
			static float fps = 60.0f;
			static float delta = 0;
			
			static const float WRITE_FPS_TIMEOUT = 0.5; // seconds
			static const float FPS_AVG_WEIGHT = 0.1f; // 0 <= x <= 1
			
			delta += dt;
			fps = fps * (1.0f - FPS_AVG_WEIGHT) + (1.0f / dt) * FPS_AVG_WEIGHT;

			if (delta > WRITE_FPS_TIMEOUT)
			{
				text->setText("FPS: " + std::to_string(fps));
				delta -= 1.0f;
			}
		}
	}
private:
	const float SPEED = 10;
};

int main()
{
	try
	{
		Ref<Application> app; // The game must be destroyed before the application
		{
			TestGame game;

			app = Application::launch(game, vulture::AppConfig{ "Vulture demo", 800, 600 });
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception &exception)
	{
		std::cerr << exception.what() << std::endl;
	}

	return EXIT_FAILURE;
}