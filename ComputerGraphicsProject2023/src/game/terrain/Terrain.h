#pragma once
#include "vulture/core/Application.h"
#include "vulture/scene/Scene.h"

namespace game {

using namespace vulture;

class Terrain
{
public:
	Terrain();

	void update(float dt);
private:
	Scene* m_Scene = nullptr;
	Ref<DescriptorSetLayout> m_DescriptorSetLayout;
	PipelineHandle m_Pipeline;
	Uniform<ModelBufferObject> m_Uniform;
	Ref<Texture> m_NoiseTexture;
	Ref<TextureSampler> m_NoiseSampler;
	Ref<DescriptorSet> m_DescriptorSet;
	Ref<Model> m_Model;
	ObjectHandle m_Object;
};

} // namespace game
