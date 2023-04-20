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
	glm::vec2 position = { 0 , 0 };
	float scale = 1.0f;
};

using UITextHandle = int64_t;

class UITextRecreated {};

class UIText
{
	EVENT(UITextRecreated)

public:
	UIText(UITextHandle handle, const Renderer& renderer, DescriptorPool& descriptorPool,
		Ref<DescriptorSetLayout> descriptorSetLayout, Ref<Font> font, 
		const String& text, glm::vec2 position, float scale);

	void setText(const String& text);
	void setPosition(glm::vec2 position) { m_Uniform->position = position; } // TODO boundary check
	void setSize(float size) { m_Uniform->scale = size; } // TODO boundary check

	inline const String& getText() const { return m_Text; }
	inline glm::vec2 getPosition() const { return m_Uniform->position; }
	inline float getSize() const { return m_Uniform->scale; }

	friend class UIHandler;
private:
	const UITextHandle m_Hndle;
	Device const* m_Device;
	Ref<Font> m_Font;

	String m_Text;
	bool m_Modified = false;

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	uint32_t m_IndexCount = 0;

	Uniform<TextBufferObject> m_Uniform;

	Ref<DescriptorSet> m_DescriptorSet;
	inline const DescriptorSet& getDescriptorSet() const { return *m_DescriptorSet; }

	void recreate();
	void update(float dt);
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
	Ref<UIText> makeText(String text, glm::vec2 position = {20, 0.5}, float scale = 22.0f);
	void removeText(Ref<UIText> text);

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
	Ref<DescriptorSet> m_ScreenDescriptorSet;

	void update(float dt);
	void recordCommandBuffer(RenderTarget& target);
	void updateUniforms(RenderTarget& target);
};

} // namespace vulture
