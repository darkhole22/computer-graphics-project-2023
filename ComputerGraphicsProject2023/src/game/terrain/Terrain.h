#pragma once
#include "vulture/core/Application.h"
#include "vulture/scene/Scene.h"

namespace game {

using namespace vulture;

struct TerrainVertexBufferObject
{
	alignas(4) f32 scale = 35.0f;
	alignas(4) f32 waterLevel = 0.35f;
};

class Terrain;

class TerrainChunk
{
public:
	TerrainChunk(Terrain* terrain, glm::vec2 position);

	void update(glm::vec2 position);

	~TerrainChunk();
private:
	Scene* m_Scene = nullptr;
	Terrain* m_Terrain;

	Ref<Texture> m_NoiseTexture;
	Ref<TextureSampler> m_NoiseSampler;

	Uniform<ModelBufferObject> m_Uniform;
	Ref<DescriptorSet> m_DescriptorSet;
	ObjectHandle m_Object;
};

struct TerrainGenerationConfig
{
	f32 renderDistance = 200.0f;
	f32 chunkSize = 80.0f;
	f32 noiseScale = 3.0f;

	static TerrainGenerationConfig defaultConfig;
};

class Terrain
{
public:
	explicit Terrain(const TerrainGenerationConfig& config = TerrainGenerationConfig::defaultConfig);

	void update(f32 dt);

	void setReferencePosition(glm::vec2 position);

	f32 getHeightAt(glm::vec2 position) const;
	f32 getHeightAt(f32 x, f32 y) const { return getHeightAt({ x, y }); }

	friend class TerrainChunk;
private:
	Scene* m_Scene = nullptr;
	Ref<DescriptorSetLayout> m_DescriptorSetLayout;
	PipelineHandle m_Pipeline;
	Ref<Model> m_Model;
	Uniform<TerrainVertexBufferObject> m_VertexUniform;

	TerrainGenerationConfig m_Config;

	u64 m_ChunksSideCount;
	std::vector<Ref<TerrainChunk>> m_Chunks;
	glm::vec2 m_ReferencePosition = {0, 0 };

	void initializeRenderingComponents();
	void initializeChunks();
};

} // namespace game
