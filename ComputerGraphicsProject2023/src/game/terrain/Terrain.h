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

class Terrain
{
public:
	Terrain();

	void update(f32 dt);

	friend class TerrainChunk;
private:
	Scene* m_Scene = nullptr;
	Ref<DescriptorSetLayout> m_DescriptorSetLayout;
	PipelineHandle m_Pipeline;
	Ref<Model> m_Model;
	Uniform<TerrainVertexBufferObject> m_VertexUniform;

	Ref<TerrainChunk> chunk;
	Ref<UIText> debugText;

};

} // namespace game
