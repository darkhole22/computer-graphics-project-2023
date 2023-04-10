#pragma once

#include "vulture/renderer/RenderTarget.h"

namespace vulture {

struct UIVertex
{
	glm::vec2 position = { 0 , 0 };
	glm::vec2 textureCoordinate = { 0 , 0 };
};

struct TextBufferObject
{
	glm::vec2 position = { 50 , 50 };
	float scale = 100.0f;
};

using UITextHandle = int64_t;

class UIText
{
public:
	UIText(UITextHandle handle, const Renderer& renderer, DescriptorPool& descriptorPool,
		Ref<DescriptorSetLayout> descriptorSetLayout,	Ref<Texture> fontTexture) :
		m_Hndle(handle)// , m_DescriptorSet(descriptorSet)
	{
		const Device& device = renderer.getDevice();

		m_Uniform = renderer.makeUniform<TextBufferObject>();

		m_DescriptorSet = descriptorPool.getDescriptorSet(*descriptorSetLayout,
			{ *fontTexture, m_Uniform });

		std::vector<UIVertex> vertices{};
		std::vector<uint32_t> indecies{};

		vertices.push_back({ { 0, 0 }, { 0, 0 } });
		vertices.push_back({ { 1, 0 }, { 1, 0 } });
		vertices.push_back({ { 0, 1 }, { 0, 1 } });
		vertices.push_back({ { 1, 1 }, { 1, 1 } });

		indecies.push_back(0);
		indecies.push_back(2);
		indecies.push_back(1);

		indecies.push_back(1);
		indecies.push_back(2);
		indecies.push_back(3);

		VkDeviceSize vertexBufferSize = sizeof(UIVertex) * vertices.size();
		Buffer vertexStagingBuffer(device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vertexStagingBuffer.map(vertices.data());
		m_VertexBuffer = Buffer(device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vertexStagingBuffer.copyToBuffer(vertexBufferSize, m_VertexBuffer);

		VkDeviceSize indexBufferSize = sizeof(uint32_t) * indecies.size();
		Buffer indexStagingBuffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		indexStagingBuffer.map(indecies.data());
		m_IndexBuffer = Buffer(device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		indexStagingBuffer.copyToBuffer(indexBufferSize, m_IndexBuffer);

		m_IndexCount = static_cast<uint32_t>(indecies.size());
	}

	friend class UIHandler;
private:
	const UITextHandle m_Hndle;

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	uint32_t m_IndexCount = 0;

	Uniform<TextBufferObject> m_Uniform;

	WRef<DescriptorSet> m_DescriptorSet;
	inline const DescriptorSet& getDescriptorSet() const { return *m_DescriptorSet.lock(); }
};

struct ScreenBufferObject
{
	float width;
	float height;
};

class UIHandler
{
public:
	Ref<UIText> makeText(std::string text, glm::vec2 position = {0.5, 0.5}, float scale = 1.0f);

	friend class Scene;
private:
	UIHandler(const Renderer& renderer, DescriptorPool& descriptorsPool);

	Ref<DescriptorSetLayout> m_TextDSLayout;
	Ref<DescriptorSetLayout> m_ScreenDSLayout;
	Ref<Pipeline> m_Pipeline;
	Ref<Texture> m_FontAtlas;

	UITextHandle m_NextTextHandle = 0;
	std::unordered_map<UITextHandle, Ref<UIText>> m_Texts;

	Renderer const* m_Renderer;
	DescriptorPool* m_DescriptorPool;
	Uniform<ScreenBufferObject> m_ScreenUniform;
	WRef<DescriptorSet> m_ScreenDescriptorSet;

	void update(float dt);
	void recordCommandBuffer(RenderTarget& target);
	void updateUniforms(RenderTarget& target);
};

} // namespace vulture
