#pragma once

#include "vulture/renderer/RenderTarget.h"
#include "Font.h"
#include "vulture/event/Event.h"

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

class UITextRecreated {};

class UIText
{
	EVENT(UITextRecreated)

public:
	UIText(UITextHandle handle, const Renderer& renderer, DescriptorPool& descriptorPool,
		Ref<DescriptorSetLayout> descriptorSetLayout, const Font& font);

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

class UIModified {};

class UIHandler
{
	EVENT(UIModified)

public:
	Ref<UIText> makeText(std::string text, glm::vec2 position = {0.5, 0.5}, float scale = 1.0f);

	friend class Scene;
private:
	UIHandler(const Renderer& renderer, DescriptorPool& descriptorsPool);

	Ref<DescriptorSetLayout> m_TextDSLayout;
	Ref<DescriptorSetLayout> m_ScreenDSLayout;
	Ref<Pipeline> m_Pipeline;

	Ref<Font> m_Font;

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
