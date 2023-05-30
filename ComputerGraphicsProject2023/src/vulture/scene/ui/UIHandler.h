#pragma once

#include "vulture/renderer/FrameContext.h"
#include "UIText.h"
#include "UIImage.h"
#include "vulture/event/Event.h"

namespace vulture {

struct ScreenBufferObject
{
	f32 width;
	f32 height;
};

class UIModified
{};

class UIHandler
{
	EVENT(UIModified)

public:
	Ref<UIText> makeText(String text, glm::vec2 position = { 20, 20 }, f32 scale = 22.0f);
	void removeText(Ref<UIText> text);

	Ref<UIImage> makeImage(String image, glm::vec2 position = { 20, 20 }, f32 scale = 100.0f);
	void removeImage(Ref<UIImage> image);

	// I'm keeping this here for now, but the fact that this is not readily up-to-date with
	// Window resizes makes it uncomfortable to use.
	template<class UIElement>
	void centerElement(Ref<UIElement> element, f32 xOffset = 0.0f, f32 yOffset = 0.0f)
	{
		element->setPosition((m_ScreenUniform->width - element->getWidth()) / 2.0f + xOffset,
						  (m_ScreenUniform->height - element->getHeight()) / 2.0f + yOffset);
	}

	friend class Scene;
private:
	UIHandler(DescriptorPool& descriptorsPool);

	Ref<DescriptorSetLayout> m_ScreenDSLayout;

	Ref<DescriptorSetLayout> m_TextDSLayout;
	Ref<Pipeline> m_TextPipeline;
	Ref<Font> m_Font;
	std::unordered_set<Ref<UIText>> m_Texts;

	Ref<DescriptorSetLayout> m_ImageDSLayout;
	Ref<Pipeline> m_ImagePipeline;
	Buffer m_ImageVertexBuffer;
	Buffer m_ImageIndexBuffer;
	std::unordered_set<Ref<UIImage>> m_Images;

	DescriptorPool* m_DescriptorPool;
	Uniform<ScreenBufferObject> m_ScreenUniform;
	Ref<DescriptorSet> m_ScreenDescriptorSet;

	void update(f32 dt);
	void recordCommandBuffer(FrameContext& target);
	void updateUniforms(FrameContext& target);
};

} // namespace vulture
