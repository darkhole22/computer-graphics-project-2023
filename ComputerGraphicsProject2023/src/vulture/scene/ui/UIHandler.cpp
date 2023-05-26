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

UIText::UIText(UITextHandle handle, DescriptorPool& descriptorPool,
			   Ref<DescriptorSetLayout> descriptorSetLayout,
			   Ref<Font> font, const String& text, glm::vec2 position, f32 scale) :
	m_Handle(handle), m_Font(font), m_Text(text)
{
	m_VertexUniform = Renderer::makeUniform<TextVertexBufferObject>();
	m_FragmentUniform = Renderer::makeUniform<TextFragmentBufferObject>();

	m_DescriptorSet = descriptorPool.getDescriptorSet(descriptorSetLayout,
													  { font->getTextureSampler(), m_VertexUniform, m_FragmentUniform });

	m_VertexUniform->position = position;
	m_VertexUniform->scale = scale;

	recreate();
}

void UIText::setText(const String& text)
{
	if (!m_Modified && m_Text == text)
		return;

	m_Text = text;
	m_Modified = true;
}

void UIText::setVisible(bool visible)
{
	bool wasVisible = m_Visible;
	m_Visible = visible;
	m_FragmentUniform->visibility = static_cast<float>(visible);
	if (visible && !wasVisible)
	{
		emit(UITextRecreated());
	}
}

void UIText::recreate()
{
	m_Width = 0.0f;
	m_Height = 0.0f;

	u64 textLength = m_Text.length();
	if (textLength == 0)
	{
		m_IndexCount = 0;
		setVisible(false);
		return;
	}

	if (textLength * 4 > m_Vertices.size())
	{
		m_Vertices.resize(textLength * 4);
		m_Indices.resize(textLength * 6);
	}

	const f32 cSize = m_Font->getCharacterSize();

	f32 x = 0;
	f32 y = 0;

	u64 vertexCount = 0;
	m_IndexCount = 0;

	for (u64 i = 0; i < textLength; i++)
	{
		i32 codepoint = m_Text[i]; // TODO handle unicode

		if (codepoint == '\n')
		{
			x = 0;
			y += m_Font->getLineHeight();
			continue;
		}

		if (codepoint == '\t')
		{
			x += m_Font->getTabXAdvance();
			continue;
		}

		auto& c = m_Font->getCharacterMapping(codepoint);

		f32 minx = x + c.xOffset;
		f32 miny = y + c.yOffset;
		f32 maxx = minx + c.width;
		f32 maxy = miny + c.height;

		f32 tminx = static_cast<f32>(c.x) / m_Font->getAtlasWidth();
		f32 tmaxx = static_cast<f32>(c.x + c.width) / m_Font->getAtlasWidth();
		f32 tminy = static_cast<f32>(c.y) / m_Font->getAtlasHeight();
		f32 tmaxy = static_cast<f32>(c.y + c.height) / m_Font->getAtlasHeight();

		m_Vertices[vertexCount + 0] = UIVertex{ {minx / cSize, miny / cSize}, {tminx, tminy} };
		m_Vertices[vertexCount + 1] = UIVertex{ {maxx / cSize, miny / cSize}, {tmaxx, tminy} };
		m_Vertices[vertexCount + 2] = UIVertex{ {minx / cSize, maxy / cSize}, {tminx, tmaxy} };
		m_Vertices[vertexCount + 3] = UIVertex{ {maxx / cSize, maxy / cSize}, {tmaxx, tmaxy} };

		m_Width = std::max(m_Width, maxx / cSize);
		m_Height = std::max(m_Height, maxy / cSize);

		f32 advance = c.xAdvance;
		if (i < textLength - 1)
		{
			i32 nextCodepoint = m_Text[i + 1LL];
			auto* k = m_Font->getKerning(codepoint, nextCodepoint);
			if (k)
				advance += k->amount;
		}
		x += advance;

		m_Indices[m_IndexCount + 0] = static_cast<u32>(vertexCount + 0);
		m_Indices[m_IndexCount + 1] = static_cast<u32>(vertexCount + 2);
		m_Indices[m_IndexCount + 2] = static_cast<u32>(vertexCount + 1);

		m_Indices[m_IndexCount + 3] = static_cast<u32>(vertexCount + 1);
		m_Indices[m_IndexCount + 4] = static_cast<u32>(vertexCount + 2);
		m_Indices[m_IndexCount + 5] = static_cast<u32>(vertexCount + 3);

		vertexCount += 4;
		m_IndexCount += 6;
	}

	VUTRACE("Recreating text: [%s]| Size: [%f / %f].", m_Text.cString(), getWidth(), getHeight());

	VkDeviceSize vertexBufferSize = sizeof(UIVertex) * vertexCount;
	if (vertexBufferSize > m_VertexStagingBuffer.getSize())
	{
		m_VertexStagingBuffer = Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
	m_VertexStagingBuffer.map(m_Vertices.data());
	if (vertexBufferSize > m_VertexBuffer.getSize())
	{
		m_VertexBuffer = Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}
	m_VertexStagingBuffer.copyToBuffer(vertexBufferSize, m_VertexBuffer);

	VkDeviceSize indexBufferSize = sizeof(u32) * m_IndexCount;
	if (indexBufferSize > m_IndexStagingBuffer.getSize())
	{
		m_IndexStagingBuffer = Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
	m_IndexStagingBuffer.map(m_Indices.data());
	if (indexBufferSize > m_IndexBuffer.getSize())
	{
		m_IndexBuffer = Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}
	m_IndexStagingBuffer.copyToBuffer(indexBufferSize, m_IndexBuffer);

	emit(UITextRecreated());
}

void UIText::update(f32 dt)
{
	if (m_Modified)
	{
		m_Modified = false;
		recreate();
	}
}

} // namespace vulture
