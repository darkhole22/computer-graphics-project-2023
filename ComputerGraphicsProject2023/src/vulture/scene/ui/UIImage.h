#pragma once

#include "UIVertex.h"
#include "vulture/renderer/Renderer.h"

namespace vulture {

struct UIImageVertexBufferObject
{
	alignas(8) glm::vec2 position = { 0, 0 };
	alignas(8) f32 scale = 1.0f;
	alignas(4) f32 ratio = 1.0f;
};

class UIImageRecreated
{};

class UIImage
{
	EVENT(UIImageRecreated)
public:
	UIImage(DescriptorPool& descriptorPool, Ref<DescriptorSetLayout> descriptorSetLayout,
			const String& image, glm::vec2 position, f32 scale);

	void setImage(const String& image);
	inline void setPosition(glm::vec2 position) { m_Uniform->position = position; }
	inline void setPosition(f32 x, f32 y) { setPosition({ x, y }); }
	inline void setWidth(f32 size) { m_Uniform->scale = size / m_Uniform->ratio; }
	inline void setHeight(f32 size) { m_Uniform->scale = size; }
	void setVisible(bool visible);

	inline glm::vec2 getPosition() const { return m_Uniform->position; }
	inline f32 getAspectRatio() const { return m_Uniform->ratio; }

	/**
	 * @brief Returns the image width in pixels.
	 */
	inline f32 getWidth() const { return m_Uniform->scale * m_Uniform->ratio; }

	/**
	 * @brief Returns the image height in pixels.
	 */
	inline f32 getHeight() const { return m_Uniform->scale; }

	inline bool isVisible() const { return m_Visible; }

	friend class UIHandler;
private:
	String m_TextureName;
	Ref<Texture> m_Texture;
	Ref<TextureSampler> m_TextureSampler;
	Uniform<UIImageVertexBufferObject> m_Uniform;
	Ref<DescriptorSet> m_DescriptorSet;

	DescriptorPool* m_DescriptorPool;
	Ref<DescriptorSetLayout> m_DescriptorSetLayout;

	bool m_Visible = true;
	bool m_Modified = true;

	inline const DescriptorSet& getDescriptorSet() const { return *m_DescriptorSet; }

	void update(f32);
};

} // namespace vulture
