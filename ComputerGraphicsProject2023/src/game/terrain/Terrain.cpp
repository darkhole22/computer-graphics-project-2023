#include "Terrain.h"

#include "vulture/core/Logger.h"
#include "vulture/core/Input.h"

#include "stb_perlin.h"

namespace game {

using namespace vulture;

f32 noiseFunction(f32 x, f32 y);
glm::vec4 noise(f32 x, f32 y);

struct NoiseConfig
{};

TerrainGenerationConfig TerrainGenerationConfig::defaultConfig{};

Terrain::Terrain(const TerrainGenerationConfig& config) :
	m_Config(config)
{
	initializeRenderingComponents();

	initializeChunks();

	debugText = m_Scene->getUIHandle()->makeText("Water Level: ");
	debugText->setPosition({ 20, 150 });
	debugText->setBorder(true);
}

void Terrain::update(f32 dt)
{
	f32 dh = Input::getAxis("TERRAIN_DOWN", "TERRAIN_UP") * dt;

	m_VertexUniform->waterLevel = std::clamp(m_VertexUniform->waterLevel + dh, 0.0f, 1.0f);

	debugText->setText(stringFormat("Water Level : %f", m_VertexUniform->waterLevel));
}

void Terrain::setReferencePosition(glm::vec2 position)
{
	i64 oldX = static_cast<i64>(std::floor(m_ReferencePosition.x / m_Config.chunkSize));
	i64 oldY = static_cast<i64>(std::floor(m_ReferencePosition.y / m_Config.chunkSize));

	i64 targetX = static_cast<i64>(std::floor(position.x / m_Config.chunkSize));
	i64 targetY = static_cast<i64>(std::floor(position.y / m_Config.chunkSize));

	if (oldX != targetX || oldY != targetY)
	{
		std::vector<Ref<TerrainChunk>> newChunks(m_Chunks.size());

		i64 count = m_ChunksSideCount;
		for (i64 y = 0; y < count; y++)
		{
			for (i64 x = 0; x < count; x++)
			{
				i64 vectorCoordX = x + (targetX - oldX);
				i64 vectorCoordY = y + (targetY - oldY);

				if (vectorCoordX < 0 || vectorCoordX >= count || vectorCoordY < 0 || vectorCoordY >= count)
				{
					glm::vec2 chunkPosition = {
						static_cast<f32>(targetX + x - count / 2),
						static_cast<f32>(targetY + y - count / 2)
					};
					newChunks[y * count + x] = makeRef<TerrainChunk>(this, chunkPosition);
				}
				else
				{
					newChunks[y * count + x] = m_Chunks[vectorCoordY * count + vectorCoordX];
				}
			}
		}

		m_Chunks = newChunks;
	}
	m_ReferencePosition = position;
}

f32 Terrain::getHeightAt(glm::vec2 position) const
{
	auto chunkPosition = position * m_Config.noiseScale / m_Config.chunkSize;
	f32 h = noiseFunction(chunkPosition.x, chunkPosition.y);
	return std::clamp(h, m_VertexUniform->waterLevel, 1.0f) * m_VertexUniform->scale;
}

void Terrain::initializeRenderingComponents()
{
	m_Scene = Application::getScene();

	m_DescriptorSetLayout = makeRef<DescriptorSetLayout>();
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS);
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
	m_DescriptorSetLayout->create();

	m_Pipeline = m_Scene->makePipeline("res/shaders/Terrain_vert.spv", "res/shaders/Terrain_frag.spv", m_DescriptorSetLayout);

	m_Model = Model::getPlane(200, 200);
	m_VertexUniform = Renderer::makeUniform<TerrainVertexBufferObject>();
}

void Terrain::initializeChunks()
{
	m_ChunksSideCount = 1 + static_cast<u32>(std::ceil(m_Config.renderDistance / m_Config.chunkSize)) * 2LL;

	m_Chunks.resize(m_ChunksSideCount * m_ChunksSideCount);

	i64 count = m_ChunksSideCount;

	for (i64 y = 0; y < count; y++)
	{
		for (i64 x = 0; x < count; x++)
		{
			glm::vec2 chunkPosition = {
				static_cast<f32>(x - count / 2),
				static_cast<f32>(y - count / 2)
			};
			m_Chunks[y * count + x] = makeRef<TerrainChunk>(this, chunkPosition);
		}
	}
}

TerrainChunk::TerrainChunk(Terrain* terrain, glm::vec2 position) :
	m_Terrain(terrain)
{
	m_Scene = terrain->m_Scene;

	glm::vec2 noiseSize = glm::vec2(1, 1) * terrain->m_Config.noiseScale * terrain->m_Config.chunkSize / 100.0f;
	m_NoiseTexture = Texture::make(128, 128, position * noiseSize, noiseSize, noise);
	TextureSamplerConfig samplerConfig;
	samplerConfig.setAddressMode(VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
	m_NoiseSampler = makeRef<TextureSampler>(*m_NoiseTexture, samplerConfig);

	m_Uniform = Renderer::makeUniform<ModelBufferObject>();

	m_Uniform->model = glm::translate(glm::mat4(1), glm::vec3(position.x, 0, position.y) * terrain->m_Config.chunkSize) *
		glm::scale(glm::mat4(1), { terrain->m_Config.chunkSize, 1, terrain->m_Config.chunkSize });

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
