#include "UIHandler.h"

namespace vulture {
	
UIHandler::UIHandler(const Renderer& renderer, DescriptorPool& descriptorsPool) :
	m_Renderer(&renderer), m_DescriptorPool(&descriptorsPool)
{
	m_TextDSLayout = renderer.makeDescriptorSetLayout();
	m_TextDSLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_TextDSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	m_TextDSLayout->create();

	m_ScreenDSLayout = renderer.makeDescriptorSetLayout();
	m_ScreenDSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_ScreenDSLayout->create();

	m_ScreenUniform = m_Renderer->makeUniform<ScreenBufferObject>();
	m_ScreenDescriptorSet = m_DescriptorPool->getDescriptorSet(*m_ScreenDSLayout, { m_ScreenUniform });

	PipelineAdvancedConfig pipelineConfig{};
	pipelineConfig.compareOprator = VK_COMPARE_OP_ALWAYS;
	pipelineConfig.useAlpha = true;

	m_Pipeline.reset(new Pipeline(
		renderer.getRenderPass(),
		"res/shaders/UItextSDF_vert.spv", "res/shaders/UItextSDF_frag.spv",
		{ m_TextDSLayout.get(), m_ScreenDSLayout.get() },
		VertexLayout(sizeof(UIVertex),
			{ {VK_FORMAT_R32G32_SFLOAT , offsetof(UIVertex, position)},
			{VK_FORMAT_R32G32_SFLOAT , offsetof(UIVertex, textureCoordinate)} }),
		pipelineConfig));

	m_Font = Font::getDefault(renderer);

	makeText("Test");
}

Ref<UIText> UIHandler::makeText(std::string text, glm::vec2 positon, float scale)
{
	UITextHandle handle = m_NextTextHandle++;
	auto uiText = Ref<UIText>(new UIText(handle, *m_Renderer, *m_DescriptorPool,
		m_TextDSLayout, *m_Font));
	m_Texts.insert({ handle, uiText });
	return uiText;
}

void UIHandler::recordCommandBuffer(RenderTarget& target)
{
	target.bindPipeline(*m_Pipeline);

	target.bindDescriptorSet(*m_Pipeline, *m_ScreenDescriptorSet.lock(), 1);

	for (auto& [handle, text] : m_Texts)
	{
		target.bindDescriptorSet(*m_Pipeline, text->getDescriptorSet(), 0);

		target.bindVertexBuffer(text->m_VertexBuffer);
		target.bindIndexBuffer(text->m_IndexBuffer);
		target.drawIndexed(text->m_IndexCount);
	}
}

void UIHandler::updateUniforms(RenderTarget& target)
{
	auto [index, count] = target.getFrameInfo();

	m_ScreenDescriptorSet.lock()->map(index);

	for (auto& [handle, text] : m_Texts)
	{
		text->getDescriptorSet().map(index);
	}
}

void UIHandler::update(float dt)
{
	static float time = 0;
	time += dt;



}

UIText::UIText(UITextHandle handle, const Renderer& renderer, 
	DescriptorPool& descriptorPool, Ref<DescriptorSetLayout> descriptorSetLayout, 
	const Font& font) :
	m_Hndle(handle)
{
	const Device& device = renderer.getDevice();

	m_Uniform = renderer.makeUniform<TextBufferObject>();

	m_DescriptorSet = descriptorPool.getDescriptorSet(*descriptorSetLayout,
		{ font.getTexture(), m_Uniform });

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

} // namespace vulture
