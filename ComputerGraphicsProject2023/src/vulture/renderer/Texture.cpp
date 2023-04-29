#include "Texture.h"

#include "vulture/core/Logger.h"
#include "VulkanContext.h"

#include "stb_image.h"

namespace vulture {

extern VulkanContextData vulkanData;

const TextureSamplerConfig TextureSamplerConfig::defaultConfig = TextureSamplerConfig{};

Texture::Texture(const String& path)
{
	i32 texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(path.cString(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * 4LL * texHeight;

	if (!pixels)
	{
		VUERROR("Failed to load texture at %s!", path.cString());
		throw std::runtime_error(("Failed to load texture at " + path + "!").cString());
	}

	m_MipLevels = static_cast<u32>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	Buffer stagingBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.map(pixels);

	stbi_image_free(pixels);

	m_Image = Image(texWidth, texHeight, m_MipLevels, VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	m_Image.transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);

	m_Image.copyFromBuffer(stagingBuffer);
	m_Image.generateMipmaps(m_MipLevels);
}

Texture::~Texture()
{
	
}

TextureSampler::TextureSampler(const Texture& texture, const TextureSamplerConfig& config)
{
	m_View = texture.getView();
	m_Layout = texture.getLayout();

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = config.magFilter;
	samplerInfo.minFilter = config.minFilter;
	samplerInfo.addressModeU = config.addressModeU;
	samplerInfo.addressModeV = config.addressModeV;
	samplerInfo.addressModeW = config.addressModeW;
	samplerInfo.anisotropyEnable = (vulkanData.physicalDeviceFeatures.samplerAnisotropy && config.useAnisotropy) ? VK_TRUE : VK_FALSE;
	samplerInfo.maxAnisotropy = vulkanData.physicalDeviceProperties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = config.mipmapMode;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(texture.getMipLevels());
	samplerInfo.mipLodBias = 0.0f;

	VkResult result = vkCreateSampler(vulkanData.device, &samplerInfo, vulkanData.allocator, &m_Handle);

	if (result != VK_SUCCESS)
	{
		VUERROR("Failed to create texture sampler!");
	}
}

TextureSampler::~TextureSampler()
{
	if (m_Handle != VK_NULL_HANDLE)
	{
		vkDestroySampler(vulkanData.device, m_Handle, vulkanData.allocator);
	}
}

} // namespace vulture
