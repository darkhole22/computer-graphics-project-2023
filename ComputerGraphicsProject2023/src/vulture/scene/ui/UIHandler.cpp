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
			{ {VK_FORMAT_R32G32_SFLOAT , static_cast<uint32_t>(offsetof(UIVertex, position))},
			{VK_FORMAT_R32G32_SFLOAT , static_cast<uint32_t>(offsetof(UIVertex, textureCoordinate))} }),
		pipelineConfig));

	m_Font = Font::getDefault(renderer);
}

Ref<UIText> UIHandler::makeText(std::string text, glm::vec2 position, float scale)
{
	UITextHandle handle = m_NextTextHandle++;
	auto uiText = Ref<UIText>(new UIText(handle, *m_Renderer, *m_DescriptorPool,
		m_TextDSLayout, m_Font, text, position, scale));
	m_Texts.insert({ handle, uiText });

	uiText->addCallback([this](const UITextRecreated& event) {
		emit(UIModified());
	});

	emit(UIModified());
	return uiText;
}

void UIHandler::removeText(Ref<UIText> text)
{
	auto& it = m_Texts.find(text->m_Hndle);
	m_Texts.erase(it);
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
	for (auto& [handle, text] : m_Texts)
	{
		text->update(dt);
	}
}

UIText::UIText(UITextHandle handle, const Renderer& renderer, 
	DescriptorPool& descriptorPool, Ref<DescriptorSetLayout> descriptorSetLayout, 
	Ref<Font> font, const std::string& text, glm::vec2 position, float scale) :
	m_Hndle(handle), m_Device(&renderer.getDevice()), m_Font(font), m_Text(text)
{
	m_Uniform = renderer.makeUniform<TextBufferObject>();

	m_DescriptorSet = descriptorPool.getDescriptorSet(*descriptorSetLayout,
		{ font->getTexture(), m_Uniform });

	m_Uniform->position = position;
	m_Uniform->scale = scale;

	recreate();
}

void UIText::setText(const std::string& text)
{
	if (!m_Modified && m_Text == text) return;

	m_Text = text;
	m_Modified = true;
}

void UIText::recreate()
{
	std::vector<UIVertex> vertices{};
	std::vector<uint32_t> indecies{};
	vertices.reserve(m_Text.length() * 4);
	indecies.reserve(m_Text.length() * 6);

	const float cSize = m_Font->getCharacterSize();

	float x = 0;
	float y = 0;

	for (int i = 0; i < m_Text.length(); i++)
	{
		int32_t codepoint = m_Text[i]; // TODO handle unicode

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

		float minx = x + c.xOffset;
		float miny = y + c.yOffset;
		float maxx = minx + c.width;
		float maxy = miny + c.height;

		float tminx = static_cast<float>(c.x) / m_Font->getAtlasWidth();
		float tmaxx = static_cast<float>(c.x + c.width) / m_Font->getAtlasWidth();
		float tminy = static_cast<float>(c.y) / m_Font->getAtlasHeight();
		float tmaxy = static_cast<float>(c.y + c.height) / m_Font->getAtlasHeight();

		uint32_t indexBase = static_cast<uint32_t>(vertices.size());
		vertices.push_back({ { minx / cSize, miny / cSize }, { tminx, tminy } });
		vertices.push_back({ { maxx / cSize, miny / cSize }, { tmaxx, tminy } });
		vertices.push_back({ { minx / cSize, maxy / cSize }, { tminx, tmaxy } });
		vertices.push_back({ { maxx / cSize, maxy / cSize }, { tmaxx, tmaxy } });

		float advance = c.xAdvance;
		if (i < m_Text.length() - 1)
		{
			int32_t nextCodepoint = m_Text[i + 1LL];
			auto* k = m_Font->getKerning(codepoint, nextCodepoint);
			if (k) advance += k->amount;
		}
		x += advance;

		indecies.push_back(indexBase + 0);
		indecies.push_back(indexBase + 2);
		indecies.push_back(indexBase + 1);

		indecies.push_back(indexBase + 1);
		indecies.push_back(indexBase + 2);
		indecies.push_back(indexBase + 3);
	}

	VkDeviceSize vertexBufferSize = sizeof(UIVertex) * vertices.size();
	Buffer vertexStagingBuffer(*m_Device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vertexStagingBuffer.map(vertices.data());
	m_VertexBuffer = Buffer(*m_Device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vertexStagingBuffer.copyToBuffer(vertexBufferSize, m_VertexBuffer);

	VkDeviceSize indexBufferSize = sizeof(uint32_t) * indecies.size();
	Buffer indexStagingBuffer(*m_Device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	indexStagingBuffer.map(indecies.data());
	m_IndexBuffer = Buffer(*m_Device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	indexStagingBuffer.copyToBuffer(indexBufferSize, m_IndexBuffer);

	m_IndexCount = static_cast<uint32_t>(indecies.size());

	emit(UITextRecreated());
}

void UIText::update(float dt)
{
	if (m_Modified)
	{
		m_Modified = false;
		recreate();
	}
}

} // namespace vulture
