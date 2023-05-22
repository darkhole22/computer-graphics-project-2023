#include "Terrain.h"

#include "vulture/core/Input.h"

#include "stb_perlin.h"

namespace game {

using namespace vulture;

glm::vec4 noise(f32 x, f32 y);

struct NoiseConfig
{};

Terrain::Terrain()
{
	m_Scene = Application::getScene();

	m_DescriptorSetLayout = makeRef<DescriptorSetLayout>();
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS);
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
	m_DescriptorSetLayout->create();

	m_Pipeline = m_Scene->makePipeline("res/shaders/Terrain_vert.spv", "res/shaders/Terrain_frag.spv", m_DescriptorSetLayout);

	m_Model = Model::getPlane(500, 500);

	m_VertexUniform = Renderer::makeUniform<TerrainVertexBufferObject>();

	chunk = makeRef<TerrainChunk>(this, glm::vec2{ 0, 0 });

	debugText = m_Scene->getUIHandle()->makeText("Water Level: ");
}

void Terrain::update(f32 dt)
{
	f32 dh = Input::getAxis("TERRAIN_DOWN", "TERRAIN_UP") * dt;

	m_VertexUniform->waterLevel = std::clamp(m_VertexUniform->waterLevel + dh, 0.0f, 1.0f);

	debugText->setText(stringFormat("Water Level : %f", m_VertexUniform->waterLevel));
}

TerrainChunk::TerrainChunk(Terrain* terrain, glm::vec2 position) :
	m_Terrain(terrain)
{
	m_Scene = terrain->m_Scene;

	m_NoiseTexture = Texture::make(512, 512, position, { 8, 8 }, noise);
	TextureSamplerConfig samplerConfig;
	samplerConfig.setAddressMode(VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
	m_NoiseSampler = makeRef<TextureSampler>(*m_NoiseTexture, samplerConfig);

	m_Uniform = Renderer::makeUniform<ModelBufferObject>();

	m_Uniform->model = glm::translate(glm::mat4(1), { -5, -25, -15 }) *
		glm::scale(glm::mat4(1), { 100, 1, 100 });

	m_DescriptorSet = m_Scene->getDescriptorPool()->getDescriptorSet(
		terrain->m_DescriptorSetLayout,
		{ m_Uniform, *m_NoiseSampler, terrain->m_VertexUniform });
	m_Object = m_Scene->addObject(terrain->m_Pipeline, terrain->m_Model, m_DescriptorSet);
}

TerrainChunk::~TerrainChunk()
{
	m_Scene->removeObject(m_Terrain->m_Pipeline, m_Object);
}

f32 noiseFunction(f32 x, f32 y)
{
	f32 h = stb_perlin_noise3_seed(x, y, 0.0f, 0, 0, 0, 420) + 1.0f;
	h /= 2.0f;
	return  h;
}

glm::vec4 noise(f32 x, f32 y)
{
	constexpr f32 epsilon = 0.01f;
	f32 h0 = noiseFunction(x, y);
	f32 hx = noiseFunction(x + epsilon, y);
	f32 hy = noiseFunction(x, y + epsilon);
	return glm::vec4(h0, hx, hy, epsilon);
}

} // namespace game
