#pragma once

#include "Buffers.h"

namespace vulture {

class Texture
{
public:
	NO_COPY(Texture)

	Texture(const String& path);

	inline VkImageView getView() const { return m_Image.getView(); }
	inline VkSampler getSampler() const { return m_Sampler; };
	inline VkImageLayout getLayout() const { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }

	~Texture();
private:
	Image m_Image;
	VkSampler m_Sampler = VK_NULL_HANDLE;
	u32 m_MipLevels = 0;
};

} // namespace vulture
