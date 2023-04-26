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

struct TextVertexBufferObject
{
	alignas(8) glm::vec2 position = { 0 , 0 };
	alignas(8) float scale = 1.0f;
};

struct TextFragmentBufferObject
{
	alignas(16) glm::vec3 color = { 1 , 1 , 1 };
	alignas(16) glm::vec3 borderColor = { 1 , 0 , 0};
	alignas(16) float width = 0.4f;
	alignas(4) float edge = 0.1f;
	alignas(4) float borderWidth = 0.4f;
	alignas(4) float visibility = 1.0f;
};

using UITextHandle = int64_t;

class UITextRecreated {};

class UIText
{
	EVENT(UITextRecreated)

public:
	UIText(UITextHandle handle, const Renderer& renderer, DescriptorPool& descriptorPool,
		Ref<DescriptorSetLayout> descriptorSetLayout, Ref<Font> font, 
		const std::string& text, glm::vec2 position, float scale);

	void setText(const std::string& text);
	void setPosition(glm::vec2 position) { m_VertexUniform->position = position; } // TODO boundary check
	void setSize(float size) { m_VertexUniform->scale = size; } // TODO boundary check
	void setColor(glm::vec3 color) { m_FragmentUniform->color = color; }
	void setColor(float r, float g, float b) { m_FragmentUniform->color = glm::vec3(r, g, b); }
	void setBorderColor(glm::vec3 color) { m_FragmentUniform->borderColor = color; }
	void setBorderColor(float r, float g, float b) { m_FragmentUniform->borderColor = glm::vec3(r, g, b); }
	void setStroke(float stroke) { m_FragmentUniform->width = stroke; }
	void setBorder(bool border) { m_FragmentUniform->borderWidth = border ? m_FragmentUniform->width * 1.5f : m_FragmentUniform->width; }
	void setVisible(bool visible);

	inline const std::string& getText() const { return m_Text; }
	inline glm::vec2 getPosition() const { return m_VertexUniform->position; }
	inline float getSize() const { return m_VertexUniform->scale; }
	inline glm::vec3 getColor() const { return m_FragmentUniform->color; }
	inline glm::vec3 getBorderColor() const { return m_FragmentUniform->borderColor; }
	inline float getStroke() const { return m_FragmentUniform->width; }
	inline bool isBorder() const { return m_FragmentUniform->borderWidth != m_FragmentUniform->width; }
	inline bool isVisible() const { return m_Visible; }

	friend class UIHandler;
private:
	const UITextHandle m_Handle;
	Device const* m_Device;
	Ref<Font> m_Font;

	std::string m_Text;
	bool m_Modified = false;
	bool m_Visible = true;

	std::vector<UIVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	Buffer m_VertexStagingBuffer; // TODO consider implementing a staging buffer pool
	Buffer m_IndexStagingBuffer;
	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	size_t m_IndexCount = 0;

	Uniform<TextVertexBufferObject> m_VertexUniform;
	Uniform<TextFragmentBufferObject> m_FragmentUniform;

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
	Ref<UIText> makeText(std::string text, glm::vec2 position = {20, 20}, float scale = 22.0f);
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
