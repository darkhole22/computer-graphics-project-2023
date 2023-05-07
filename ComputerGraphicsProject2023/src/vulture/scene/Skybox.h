#pragma once
#include "vulture/renderer/FrameContext.h"
#include "vulture/scene/Camera.h"
#include "vulture/event/Event.h"

namespace vulture {

struct SkyboxBufferObject
{
	alignas(16) glm::mat4 proj;
	alignas(16) glm::mat4 view;
};

struct SkyboxVertex
{
	glm::vec3 position = { 0, 0, 0 };
};

class SkyboxRecreated
{
};

class Skybox
{
	EVENT(SkyboxRecreated)

public:
	void set(const String& name);

	~Skybox() = default;

	friend class Scene;
private:
	Skybox(DescriptorPool& descriptorsPool);

	String m_CurrentName = "";

	DescriptorPool* m_DescriptorPool;

	Ref<DescriptorSetLayout> m_DSLayout;
	Ref<Pipeline> m_Pipeline;

	Uniform<SkyboxBufferObject> m_Uniform;
	Ref<Texture> m_Texture;
	Ref<TextureSampler> m_TextureSampler;

	Ref<DescriptorSet> m_DescriptorSet;

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	constexpr static u32 c_IndexCount = 36;

	void recordCommandBuffer(FrameContext& target);
	void updateUniforms(FrameContext& target, const Camera& camera);

	static VertexLayout s_VertexLayout;
};

} // namespace vulture
