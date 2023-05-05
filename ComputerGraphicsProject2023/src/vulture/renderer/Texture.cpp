#include <cmath>
#include "Texture.h"

#include "VulkanContext.h"
#include "Renderer.h"

#include "vulture/core/Logger.h"

#include "stb_image.h"

namespace vulture {

extern VulkanContextData vulkanData;
extern RendererData rendererData;

const TextureSamplerConfig TextureSamplerConfig::defaultConfig = TextureSamplerConfig{};

Ref<Texture> Texture::get(const String& name, TextureType type)
{
	auto it = s_Textures.find(name);
	if (it != s_Textures.end())
	{
		auto& wref = it->second;
		if (!wref.expired())
			return wref.lock();
		else
			s_Textures.erase(it);
	}
	
	switch (type)
	{
	case TextureType::CUBE_MAP:
		break;
	case TextureType::TEXTURE_2D:
	default:
	{
		// TODO try other extentions.
		try
		{
			auto ref = makeRef<Texture>(rendererData.resourceInfo.path + "textures/" + name + ".png");
			return ref;
		}
		catch (const std::exception& exception)
		{
			VUERROR("%s", exception.what());
		}
	} break;
	}

	return s_Default;
}

Texture::Texture(const String& path)
{
	i32 texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(path.cString(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * 4LL * texHeight;

	if (!pixels)
	{
		throw std::runtime_error(("Failed to load texture at " + path + "!").cString());
	}

	loadFromPixelArray(texWidth, texHeight, pixels);

	stbi_image_free(pixels);
}

void Texture::loadFromPixelArray(u32 width, u32 heigth, u8* pixels)
{
	VkDeviceSize imageSize = width * 4LL * heigth;
	m_MipLevels = static_cast<u32>(std::floor(std::log2(std::max(width, heigth)))) + 1;

	Buffer stagingBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.map(pixels);

	m_Image = Image(width, heigth, m_MipLevels, VK_SAMPLE_COUNT_1_BIT,
		VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	m_Image.transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);

	m_Image.copyFromBuffer(stagingBuffer);
	m_Image.generateMipmaps(m_MipLevels);
}

Texture::Texture(u32 width, u32 heigth, u8* pixels)
{
	loadFromPixelArray(width, heigth, pixels);
}

Texture::~Texture() = default;

std::unordered_map<String, WRef<Texture>> Texture::s_Textures = {};
Ref<Texture> Texture::s_Default;

bool Texture::init()
{
	// s_Textures.insert({ DEFAULT_TEXTURE_NAME, getTexture(DEFAULT_TEXTURE_NAME) });
	// TODO create default texture

	const u32 width = 256;
	const u32 height = 256;
	// u8 pixels[width * height];
	auto pixels = std::vector<u8>(width * height * 4);

	const u32 box = 64;

	for (u64 y = 0; y < height; y++)
	{
		for (u64 x = 0; x < width; x++)
		{
			if (((x / box) + (y / box)) % 2)
			{
				// purple
				pixels[(y + x * height) * 4 + 0] = static_cast<u8>(0xff); // r
				pixels[(y + x * height) * 4 + 1] = static_cast<u8>(0x00); // g
				pixels[(y + x * height) * 4 + 2] = static_cast<u8>(0xff); // b
				pixels[(y + x * height) * 4 + 3] = static_cast<u8>(0xff); // a
			}
			else
			{
				// green
				pixels[(y + x * height) * 4 + 0] = static_cast<u8>(0x00); // r
				pixels[(y + x * height) * 4 + 1] = static_cast<u8>(0xff); // g
				pixels[(y + x * height) * 4 + 2] = static_cast<u8>(0x00); // b
				pixels[(y + x * height) * 4 + 3] = static_cast<u8>(0xff); // a
			}
		}
	}

	s_Default = Ref<Texture>(new Texture(width, height, pixels.data()));

	return true;
}

void Texture::cleanup()
{
	s_Default.reset();
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
