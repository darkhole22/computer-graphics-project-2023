#pragma once

#include "Buffers.h"

namespace vulture {

class Texture
{
public:
	NO_COPY(Texture)

	Texture(const String& path);

	inline VkImageView getView() const { return m_Image.getView(); }
	inline u32 getMipLevels() const { return m_MipLevels; };
	inline VkImageLayout getLayout() const { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }

	~Texture();
private:
	Image m_Image;
	u32 m_MipLevels = 0;
};

struct TextureSamplerConfig
{
	VkFilter magFilter = VK_FILTER_LINEAR;
	VkFilter minFilter = VK_FILTER_LINEAR;
	VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	bool useAnisotropy = true;

	inline void setFilters(VkFilter filter)
	{
		magFilter = filter;
		minFilter = filter;
	}

	inline void setAddressMode(VkSamplerAddressMode mode)
	{
		addressModeU = mode;
		addressModeV = mode;
		addressModeW = mode;
	}

	static const TextureSamplerConfig defaultConfig;
};

class TextureSampler
{
public:
	NO_COPY(TextureSampler)

	TextureSampler(const Texture& texture, const TextureSamplerConfig& config = TextureSamplerConfig::defaultConfig);

	inline VkSampler getHandle() const { return m_Handle; };
	inline VkImageView getView() const { return m_View; }
	inline VkImageLayout getLayout() const { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }

	~TextureSampler();
private:
	VkSampler m_Handle;
	VkImageView m_View;
	VkImageLayout m_Layout;
};

} // namespace vulture
