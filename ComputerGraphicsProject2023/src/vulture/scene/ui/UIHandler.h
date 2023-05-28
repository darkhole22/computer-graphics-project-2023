#pragma once

#include "vulture/renderer/FrameContext.h"
#include "Font.h"
#include "vulture/event/Event.h"

namespace vulture {

struct UIVertex
{
	glm::vec2 position = { 0, 0 };
	glm::vec2 textureCoordinate = { 0, 0 };
};

struct TextVertexBufferObject
{
	alignas(8) glm::vec2 position = { 0, 0 };
	alignas(8) f32 scale = 1.0f;
};

struct TextFragmentBufferObject
{
	alignas(16) glm::vec3 color = { 1, 1, 1 };
	alignas(16) glm::vec3 borderColor = { 1, 0, 0 };
	alignas(16) f32 width = 0.4f;
	alignas(4) f32 edge = 0.1f;
	alignas(4) f32 borderWidth = 0.4f;
	alignas(4) f32 visibility = 1.0f;
};

using UITextHandle = int64_t;

class UITextRecreated
{};

class UIText
{
	EVENT(UITextRecreated)

public:
	UIText(UITextHandle handle, DescriptorPool& descriptorPool,
		   Ref<DescriptorSetLayout> descriptorSetLayout, Ref<Font> font,
		   const String& text, glm::vec2 position, f32 scale);

	void setText(const String& text);
	void setPosition(glm::vec2 position) { m_VertexUniform->position = position; } // TODO boundary check
	void setPosition(f32 x, f32 y) { setPosition({ x, y }); }                      // TODO boundary check
	void setSize(f32 size) { m_VertexUniform->scale = size; }					   // TODO boundary check
	void setColor(glm::vec3 color) { m_FragmentUniform->color = color; }
	void setColor(f32 r, f32 g, f32 b) { m_FragmentUniform->color = glm::vec3(r, g, b); }
	void setBorderColor(glm::vec3 color) { m_FragmentUniform->borderColor = color; }
	void setBorderColor(f32 r, f32 g, f32 b) { m_FragmentUniform->borderColor = glm::vec3(r, g, b); }
	void setStroke(f32 stroke) { m_FragmentUniform->width = stroke; }
	void setBorder(bool border) { m_FragmentUniform->borderWidth = border ? m_FragmentUniform->width * 1.5f : m_FragmentUniform->width; }
	void setVisible(bool visible);

	inline const String& getText() const { return m_Text; }
	inline glm::vec2 getPosition() const { return m_VertexUniform->position; }
	inline f32 getSize() const { return m_VertexUniform->scale; }
	inline glm::vec3 getColor() const { return m_FragmentUniform->color; }
	inline glm::vec3 getBorderColor() const { return m_FragmentUniform->borderColor; }
	inline f32 getStroke() const { return m_FragmentUniform->width; }
	inline bool isBorder() const { return m_FragmentUniform->borderWidth != m_FragmentUniform->width; }
	inline bool isVisible() const { return m_Visible; }

	/**
	 * @brief Returns the text width in pixels.
	 * For performance reason the width is recomputed only when the text is recreated.
	 */
	inline f32 getWidth() const { return m_Width * getSize(); }

	/**
	 * @brief Returns the text height in pixels.
	 * For performance reason the height is recomputed only when the text is recreated.
	 */
	inline f32 getHeight() const { return m_Height * getSize(); }

	friend class UIHandler;

private:
	const UITextHandle m_Handle;
	Ref<Font> m_Font;

	String m_Text;
	bool m_Modified = false;
	bool m_Visible = true;

	f32 m_Width = 0.0f;
	f32 m_Height = 0.0f;

	std::vector<UIVertex> m_Vertices;
	std::vector<u32> m_Indices;
	Buffer m_VertexStagingBuffer; // TODO consider implementing a staging buffer pool
	Buffer m_IndexStagingBuffer;
	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	u64 m_IndexCount = 0;

	Uniform<TextVertexBufferObject> m_VertexUniform;
	Uniform<TextFragmentBufferObject> m_FragmentUniform;

	Ref<DescriptorSet> m_DescriptorSet;
	inline const DescriptorSet& getDescriptorSet() const { return *m_DescriptorSet; }

	void recreate();
	void update(f32 dt);
};

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
