#include "Terrain.h"

#include "vulture/core/Logger.h"
#include "vulture/core/Input.h"
#include "vulture/util/Random.h"

#include "stb_perlin.h"

namespace game {

using namespace vulture;

static constexpr f32 NOISE_SCALE_MULTIPLIER = 100.0f;

f32 noiseFunction(f32 x, f32 y);
glm::vec4 noise(f32 x, f32 y);

static i32 terrainNoiseSeed = 0;

TerrainGenerationConfig TerrainGenerationConfig::defaultConfig{};

Terrain::Terrain(const TerrainGenerationConfig& config) :
	m_Config(config)
{
	terrainNoiseSeed = Random::nextInt();
	initializeRenderingComponents();
	initializeChunks();
}

void Terrain::update(f32 dt)
{
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
					vectorCoordX %= count;
					if (vectorCoordX < 0)
					{
						vectorCoordX += count;
					}
					vectorCoordY %= count;
					if (vectorCoordY < 0)
					{
						vectorCoordY += count;
					}

					auto& chunk = m_Chunks[vectorCoordY * count + vectorCoordX];
					chunk->update(chunkPosition);
					newChunks[y * count + x] = chunk;
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
	auto chunkPosition = position * m_Config.noiseScale / NOISE_SCALE_MULTIPLIER;
	f32 h = noiseFunction(chunkPosition.x, chunkPosition.y);
	return std::clamp(h, m_VertexUniform->waterLevel, 1.0f) * m_VertexUniform->scale;
}

glm::vec2 Terrain::getSlopeAt(glm::vec2 position) const
{
	constexpr f32 epsilon = 0.01f;
	auto chunkPosition = position * m_Config.noiseScale / NOISE_SCALE_MULTIPLIER;

	f32 h0 = noiseFunction(chunkPosition.x, chunkPosition.y);
	f32 hx = noiseFunction(chunkPosition.x + epsilon, chunkPosition.y);
	f32 hy = noiseFunction(chunkPosition.x, chunkPosition.y + epsilon);

	h0 = std::clamp(h0, m_VertexUniform->waterLevel, 1.0f) * m_VertexUniform->scale;
	hx = std::clamp(hx, m_VertexUniform->waterLevel, 1.0f) * m_VertexUniform->scale;
	hy = std::clamp(hy, m_VertexUniform->waterLevel, 1.0f) * m_VertexUniform->scale;

	return glm::vec2(hx - h0, hy - h0);
}

bool Terrain::isWater(glm::vec2 position) const
{
	auto chunkPosition = position * m_Config.noiseScale / NOISE_SCALE_MULTIPLIER;
	f32 h = noiseFunction(chunkPosition.x, chunkPosition.y);
	return h <= m_VertexUniform->waterLevel;
}

void Terrain::initializeRenderingComponents()
{
	m_Scene = Application::getScene();

	m_DescriptorSetLayout = makeRef<DescriptorSetLayout>();
	// Camera Buffer Object
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	// Heightmap Texture
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS);
	// TerrainBufferObject
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
	// TextureSamplers for the terrain levels
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	m_DescriptorSetLayout->create();

	m_Pipeline = m_Scene->makePipeline("res/shaders/Terrain_vert.spv", "res/shaders/Terrain_frag.spv", m_DescriptorSetLayout);

	// Model is just a flat plane
	m_Model = Model::getPlane(200, 200);
	m_VertexUniform = Renderer::makeUniform<TerrainVertexBufferObject>();

	m_VertexUniform->scale = m_Config.heightScale;

	m_WaterTexture = Texture::get("water");
	m_WaterSampler = makeRef<TextureSampler>(*m_WaterTexture);

	m_SandTexture = Texture::get("sand");
	m_SandSampler = makeRef<TextureSampler>(*m_SandTexture);

	m_GrassTexture = Texture::get("grass");
	m_GrassSampler = makeRef<TextureSampler>(*m_GrassTexture);

	m_RockTexture = Texture::get("rock");
	m_RockSampler = makeRef<TextureSampler>(*m_RockTexture);
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

	m_Uniform = Renderer::makeUniform<ModelBufferObject>();
	glm::vec2 noiseSize = glm::vec2(1, 1) * terrain->m_Config.noiseScale * terrain->m_Config.chunkSize / NOISE_SCALE_MULTIPLIER;
	updateRenderingComponents(Texture::make(128, 128, position * noiseSize, noiseSize, noise), position);
}

void TerrainChunk::update(glm::vec2 position)
{
	glm::vec2 noiseSize = glm::vec2(1, 1) * m_Terrain->m_Config.noiseScale * m_Terrain->m_Config.chunkSize / NOISE_SCALE_MULTIPLIER;
	Texture::makeAsync(128, 128, position * noiseSize, noiseSize, noise, [this, position](Ref<Texture> texture) {
		m_Scene->removeObject(m_Terrain->m_Pipeline, m_Object);
		updateRenderingComponents(texture, position);
	});
}

TerrainChunk::~TerrainChunk()
{
	m_Scene->removeObject(m_Terrain->m_Pipeline, m_Object);
}

void TerrainChunk::updateRenderingComponents(const Ref<Texture>& texture, glm::vec2 position)
{
	m_NoiseTexture = texture;
	TextureSamplerConfig samplerConfig;
	samplerConfig.setAddressMode(VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT);
	m_NoiseSampler = makeRef<TextureSampler>(*m_NoiseTexture, samplerConfig);

	m_Uniform->model = glm::translate(glm::mat4(1), glm::vec3(position.x, 0, position.y) * m_Terrain->m_Config.chunkSize) *
		glm::scale(glm::mat4(1), { m_Terrain->m_Config.chunkSize, 1, m_Terrain->m_Config.chunkSize });

	m_DescriptorSet.reset();
	m_DescriptorSet = m_Scene->getDescriptorPool()->getDescriptorSet(
		m_Terrain->m_DescriptorSetLayout,
		{ m_Uniform, *m_NoiseSampler, m_Terrain->m_VertexUniform, *m_Terrain->m_WaterSampler,
		*m_Terrain->m_SandSampler, *m_Terrain->m_GrassSampler, *m_Terrain->m_RockSampler });

	m_Object = m_Scene->addObject(m_Terrain->m_Pipeline, m_Terrain->m_Model, m_DescriptorSet);
}

f32 noiseFunction(f32 x, f32 y)
{
	f32 h = stb_perlin_noise3_seed(x, y, 0.0f, 0, 0, 0, terrainNoiseSeed);
	h += stb_perlin_ridge_noise3(x, y, 0.0f, 2.0f, 0.5f, 1.0f, 4) * 0.5f;
	h += 1.5f;
	h /= 3.0f;
	h = 0.5f - 4.0f * std::pow(0.5f - h, 3.0f);
	return h * h;
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
