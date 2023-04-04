#include <iostream>
#include <cstdlib>

#include "vulture/core/Game.h"
#include "vulture/core/Application.h"

using namespace vulture;

struct ModelBufferObject
{
	glm::mat4 model;
};

class TestGame : public Game
{
public:
	Scene* scene = nullptr;
	Ref<DescriptorSetLayout> descriptorSetLayout;
	PipileneHandle pipeline = -1;
	Ref<Model> model;
	Uniform<ModelBufferObject> objUniform;
	Ref<Texture> objTexture;

	void setup() override
	{
		scene = Application::getScene();

		descriptorSetLayout = Application::makeDescriptorSetLayout();
		descriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		descriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		descriptorSetLayout->create();

		pipeline = scene->makePipeline("res/shaders/baseVert.spv", "res/shaders/baseFrag.spv", descriptorSetLayout);

		model = Application::makeModel("res/models/vulture.obj");
		objUniform = Application::makeUniform<ModelBufferObject>();
		objTexture = Application::makeTexture("res/textures/vulture.png");

		scene->addObject(pipeline, model, descriptorSetLayout, { objUniform , *objTexture });

		objUniform->model = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	}

	void update(float dt) override
	{
		static float time = 0;
		time += dt;
		objUniform->model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		
		objUniform.map();
	}
};

int main()
{
	try
	{
		Ref<Application> app; // The game must be destructed before the application
		{
			TestGame game;

			app = Application::launch(game, vulture::AppConfig{ "Hello Application", 800, 600 });
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception &exception)
	{
		std::cerr << exception.what() << std::endl;
	}

	return EXIT_FAILURE;
}