#include "UIHandler.h"

// #define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Logger.h"

namespace vulture {

UIHandler::UIHandler(DescriptorPool& descriptorsPool)
	: m_DescriptorPool(&descriptorsPool)
{
	m_TextDSLayout = makeRef<DescriptorSetLayout>();
	m_TextDSLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_TextDSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_TextDSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_TextDSLayout->create();

	m_ScreenDSLayout = makeRef<DescriptorSetLayout>();
	m_ScreenDSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_ScreenDSLayout->create();

	m_ScreenUniform = Renderer::makeUniform<ScreenBufferObject>();
	m_ScreenDescriptorSet = m_DescriptorPool->getDescriptorSet(m_ScreenDSLayout, { m_ScreenUniform });

	PipelineAdvancedConfig pipelineConfig{};
	pipelineConfig.compareOperator = VK_COMPARE_OP_ALWAYS;
	pipelineConfig.useAlpha = true;

	m_Pipeline.reset(new Pipeline(
		Renderer::getRenderPass(),
		"res/shaders/UItextSDF_vert.spv", "res/shaders/UItextSDF_frag.spv",
		{ m_TextDSLayout.get(), m_ScreenDSLayout.get() },
		VertexLayout(sizeof(UIVertex),
		{ {VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(UIVertex, position))},
		{VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(UIVertex, textureCoordinate))} }),
		pipelineConfig));

	m_Font = Font::getDefault();
}

Ref<UIText> UIHandler::makeText(String text, glm::vec2 position, f32 scale)
{
	UITextHandle handle = m_NextTextHandle++;
	auto uiText = Ref<UIText>(new UIText(handle, *m_DescriptorPool,
							  m_TextDSLayout, m_Font, text, position, scale));
	m_Texts.insert({ handle, uiText });

	uiText->addCallback([this](const UITextRecreated& event)
	{ emit(UIModified()); });

	emit(UIModified());
	return uiText;
}

void UIHandler::removeText(Ref<UIText> text)
{
	auto it = m_Texts.find(text->m_Handle);
	m_Texts.erase(it);
}

void UIHandler::centerText(Ref<UIText> text)
{
	text->setPosition((m_ScreenUniform->width + text->getWidth()) / 2.0f,
					  (m_ScreenUniform->height + text->getHeight()) / 2.0f);
}

void UIHandler::recordCommandBuffer(FrameContext& target)
{
	target.bindPipeline(*m_Pipeline);

	target.bindDescriptorSet(*m_Pipeline, *m_ScreenDescriptorSet, 1);

	for (auto& [handle, text] : m_Texts)
	{
		if (!text->m_Visible)
			continue;
		target.bindDescriptorSet(*m_Pipeline, text->getDescriptorSet(), 0);

		target.bindVertexBuffer(text->m_VertexBuffer);
		target.bindIndexBuffer(text->m_IndexBuffer);
		target.drawIndexed(static_cast<uint32_t>(text->m_IndexCount));
	}
}

void UIHandler::updateUniforms(FrameContext& target)
{
	auto [index, count] = target.getFrameInfo();

	m_ScreenDescriptorSet->map(index);

	for (auto& [handle, text] : m_Texts)
	{
		text->getDescriptorSet().map(index);
	}
}

void UIHandler::update(f32 dt)
{
	for (auto& [handle, text] : m_Texts)
	{
		text->update(dt);
	}
}

} // namespace vulture
