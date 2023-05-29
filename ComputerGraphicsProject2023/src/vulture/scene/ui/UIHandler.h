#pragma once

#include "vulture/renderer/FrameContext.h"
#include "UIText.h"
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

	void centerText(Ref<UIText> text);

	friend class Scene;
private:
	UIHandler(DescriptorPool& descriptorsPool);

	Ref<DescriptorSetLayout> m_TextDSLayout;
	Ref<DescriptorSetLayout> m_ScreenDSLayout;
	Ref<Pipeline> m_Pipeline;

	Ref<Font> m_Font;

	UITextHandle m_NextTextHandle = 0;
	std::unordered_map<UITextHandle, Ref<UIText>> m_Texts;

	DescriptorPool* m_DescriptorPool;
	Uniform<ScreenBufferObject> m_ScreenUniform;
	Ref<DescriptorSet> m_ScreenDescriptorSet;

	void update(f32 dt);
	void recordCommandBuffer(FrameContext& target);
	void updateUniforms(FrameContext& target);
};

} // namespace vulture
