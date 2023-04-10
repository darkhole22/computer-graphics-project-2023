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

	m_FontAtlas = renderer.makeTexture("res/textures/FiraCode.png");

	makeText("Test");
}

Ref<UIText> UIHandler::makeText(std::string text, glm::vec2 positon, float scale)
{
	UITextHandle handle = m_NextTextHandle++;
	auto uiText = Ref<UIText>(new UIText(handle, *m_Renderer, *m_DescriptorPool,
		m_TextDSLayout, m_FontAtlas));
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

} // namespace vulture
