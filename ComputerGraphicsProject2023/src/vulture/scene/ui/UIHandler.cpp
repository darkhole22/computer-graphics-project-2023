#include "UIHandler.h"

// #define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Logger.h"

namespace vulture {

UIHandler::UIHandler(DescriptorPool& descriptorsPool)
	: m_DescriptorPool(&descriptorsPool)
{
	m_ScreenDSLayout = makeRef<DescriptorSetLayout>();
	m_ScreenDSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_ScreenDSLayout->create();

	m_TextDSLayout = makeRef<DescriptorSetLayout>();
	m_TextDSLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_TextDSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_TextDSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_TextDSLayout->create();

	m_ImageDSLayout = makeRef<DescriptorSetLayout>();
	m_ImageDSLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_ImageDSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_ImageDSLayout->create();

	m_ScreenUniform = Renderer::makeUniform<ScreenBufferObject>();
	m_ScreenDescriptorSet = m_DescriptorPool->getDescriptorSet(m_ScreenDSLayout, { m_ScreenUniform });

	PipelineAdvancedConfig pipelineConfig{};
	// We want the HUD elements to be drawn on top of everything else, regardless of the depth value of the new fragment.
	// By specifying VK_COMPARE_OP_ALWAYS, we effectively disable depth testing for the HUD,
	// ensuring that it is always drawn on top of other geometry.
	pipelineConfig.compareOperator = VK_COMPARE_OP_ALWAYS;
	// Enable color blending to allow characters to be drawn correctly over other fragments.
	pipelineConfig.useAlpha = false;

	auto vertexLayout = VertexLayout(sizeof(UIVertex), {
		{VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(UIVertex, position))},
		{VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(UIVertex, textureCoordinate))} });

	m_TextPipeline.reset(new Pipeline(
		Renderer::getRenderPass(),
		"res/shaders/UItextSDF_vert.spv", "res/shaders/UItextSDF_frag.spv",
		{ m_TextDSLayout.get(), m_ScreenDSLayout.get() },
		vertexLayout, pipelineConfig));

	m_ImagePipeline.reset(new Pipeline(
		Renderer::getRenderPass(),
		"res/shaders/UIImage_vert.spv", "res/shaders/UIImage_frag.spv",
		{ m_ImageDSLayout.get(), m_ScreenDSLayout.get() },
		vertexLayout, pipelineConfig));

	m_Font = Font::getDefault();

	UIVertex imageVertecies[4] = {
		{ {0, 0}, {0, 0} },
		{ {1, 0}, {1, 0} },
		{ {0, 1}, {0, 1} },
		{ {1, 1}, {1, 1} }
	};

	VkDeviceSize vertexBufferSize = sizeof(UIVertex) * 4;
	Buffer m_ImageVertexStagingBuffer = Buffer(
		vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	m_ImageVertexStagingBuffer.map(imageVertecies);
	m_ImageVertexBuffer = Buffer(
		vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	m_ImageVertexStagingBuffer.copyToBuffer(vertexBufferSize, m_ImageVertexBuffer);

	u32 imageIndecies[6] = {
		0, 2, 1,
		3, 1, 2
	};

	VkDeviceSize indexBufferSize = sizeof(u32) * 6;
	Buffer m_ImageIndexStagingBuffer = Buffer(
		indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	m_ImageIndexStagingBuffer.map(imageIndecies);
	m_ImageIndexBuffer = Buffer(
		indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	m_ImageIndexStagingBuffer.copyToBuffer(indexBufferSize, m_ImageIndexBuffer);
}

Ref<UIText> UIHandler::makeText(String text, glm::vec2 position, f32 scale)
{
	auto uiText = Ref<UIText>(new UIText(*m_DescriptorPool,
							  m_TextDSLayout, m_Font, text, position, scale));
	m_Texts.insert(uiText);

	uiText->addCallback([this](const UITextRecreated& event)
	{ emit(UIModified{}); });

	emit(UIModified{});
	return uiText;
}

void UIHandler::removeText(Ref<UIText> text)
{
	m_Texts.erase(text);
}

Ref<UIImage> UIHandler::makeImage(String image, glm::vec2 position, f32 scale)
{
	auto uiImage = makeRef<UIImage>(*m_DescriptorPool, m_ImageDSLayout, image, position, scale);

	m_Images.insert(uiImage);

	uiImage->addCallback([this](const UIImageRecreated& event)
	{ emit(UIModified{}); });

	emit(UIModified{});
	return uiImage;
}

void UIHandler::removeImage(Ref<UIImage> image)
{
	m_Images.erase(image);
}

void UIHandler::recordCommandBuffer(FrameContext& target)
{
	target.bindPipeline(*m_ImagePipeline);
	target.bindDescriptorSet(*m_ImagePipeline, *m_ScreenDescriptorSet, 1);
	for (auto& image : m_Images)
	{
		if (!image->m_Visible)
			continue;
		target.bindDescriptorSet(*m_ImagePipeline, image->getDescriptorSet(), 0);

		target.bindVertexBuffer(m_ImageVertexBuffer);
		target.bindIndexBuffer(m_ImageIndexBuffer);
		target.drawIndexed(static_cast<u32>(6));
	}

	target.bindPipeline(*m_TextPipeline);
	target.bindDescriptorSet(*m_TextPipeline, *m_ScreenDescriptorSet, 1);
	for (auto& text : m_Texts)
	{
		if (!text->m_Visible)
			continue;
		target.bindDescriptorSet(*m_TextPipeline, text->getDescriptorSet(), 0);

		target.bindVertexBuffer(text->m_VertexBuffer);
		target.bindIndexBuffer(text->m_IndexBuffer);
		target.drawIndexed(static_cast<u32>(text->m_IndexCount));
	}
}

void UIHandler::updateUniforms(FrameContext& target)
{
	auto [index, count] = target.getFrameInfo();

	m_ScreenDescriptorSet->map(index);

	for (auto& image : m_Images)
	{
		image->getDescriptorSet().map(index);
	}

	for (auto& text : m_Texts)
	{
		text->getDescriptorSet().map(index);
	}
}

void UIHandler::update(f32 dt)
{
	for (auto& image : m_Images)
	{
		image->update(dt);
	}

	for (auto& text : m_Texts)
	{
		text->update(dt);
	}
}

} // namespace vulture
