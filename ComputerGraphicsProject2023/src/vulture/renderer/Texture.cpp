#include "Texture.h"

#include "VulkanContext.h"
#include "Renderer.h"
#include "vulture/core/Logger.h"
#include "vulture/core/Job.h"

#include "stb_image.h"

#include <cmath> // std::floor, std::log2, std::max
#include <cstring> // std::memcpy
#include <filesystem>

namespace vulture {

extern VulkanContextData vulkanData;
extern RendererData rendererData;

const TextureSamplerConfig TextureSamplerConfig::defaultConfig = TextureSamplerConfig{};

static const std::array<String, 8> supportedExtensions = {
	".png",
	".jpeg",
	".jpg",
	".tga",
	".bmp",
	".gif",
	".pic",
	".hdr"
};

bool loadCubemapPixels(const String& name, u8** pixels, u32& width, u32& height);
bool loadTexture2DPixels(const String& name, u8** pixels, u32& width, u32& height);

void populateArrayGenerator(f32* pixels, u32 width, u32 height, glm::vec2 position, glm::vec2 dimension, std::function<glm::vec4(f32, f32)> generator)
{
	for (u64 y = 0; y < height; y++)
	{
		for (u64 x = 0; x < width; x++)
		{
			f32 noiseX = dimension.x * static_cast<f32>(x) / (width - 1);
			f32 noiseY = dimension.y * static_cast<f32>(y) / (height - 1);

			auto color = generator(position.x + noiseX, position.y + noiseY);

			pixels[(y * width + x) * 4 + 3] = color.a; // a
			pixels[(y * width + x) * 4 + 2] = color.r; // r
			pixels[(y * width + x) * 4 + 1] = color.g; // g
			pixels[(y * width + x) * 4 + 0] = color.b; // b
		}
	}
}

Ref<Texture> Texture::get(const String& name)
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

	u8* pixels = nullptr;
	u32 width = 0, height = 0;
	Ref<Texture> result;

	if (loadTexture2DPixels(name, &pixels, width, height))
	{
		result = Ref<Texture>(new Texture(width, height, pixels, false));
		s_Textures.insert({ name, result });
	}
	else
	{
		result = s_Default2D;
	}

	delete pixels;
	return result;
}

Ref<Texture> Texture::getCubemap(const String& name)
{
	auto it = s_CubemapTextures.find(name);
	if (it != s_CubemapTextures.end())
	{
		auto& wref = it->second;
		if (!wref.expired())
			return wref.lock();
		else
			s_CubemapTextures.erase(it);
	}

	u8* pixels = nullptr;
	u32 width = 0, height = 0;
	Ref<Texture> result;

	if (loadCubemapPixels(name, &pixels, width, height))
	{
		result = Ref<Texture>(new Texture(width, height, pixels, true));
		s_CubemapTextures.insert({ name, result });
	}
	else
	{
		result = s_DefaultCubemap;
	}

	delete pixels;
	return result;
}

Ref<Texture> Texture::make(u32 width, u32 height, glm::vec2 position, glm::vec2 dimension, std::function<glm::vec4(f32, f32)> generator)
{
	Ref<Texture> result;
	f32* pixels = new f32[width * 4LL * height];

	populateArrayGenerator(pixels, width, height, position, dimension, generator);

	result = Ref<Texture>(new Texture(width, height, pixels));
	delete[] pixels;
	return result;
}

struct AsyncTextureLoadingData
{
	u8* pixels = nullptr;
	u32 width = 0;
	u32 height = 0;
};

void Texture::getAsync(const String& name, std::function<void(Ref<Texture>)> callback)
{
	auto it = s_Textures.find(name);
	if (it != s_Textures.end())
	{
		auto& wref = it->second;
		if (!wref.expired())
		{
			callback(wref.lock());
			return;
		}
		else
			s_Textures.erase(it);
	}

	AsyncTextureLoadingData* data = new AsyncTextureLoadingData;
	Job::submit([name](void* _data) -> bool
	{
		AsyncTextureLoadingData* loadingData = reinterpret_cast<AsyncTextureLoadingData*>(_data);
		return loadTexture2DPixels(name, &loadingData->pixels, loadingData->width, loadingData->height);
	}, data, [name, callback](bool result, void* _data)
	{
		AsyncTextureLoadingData* loadingData = reinterpret_cast<AsyncTextureLoadingData*>(_data);
		Ref<Texture> texture;
		if (result)
		{
			texture = Ref<Texture>(new Texture(loadingData->width, loadingData->height, loadingData->pixels, false));
			s_Textures.insert({ name, texture });
		}
		else
		{
			texture = s_Default2D;
		}

		callback(texture);
		delete[] loadingData->pixels;
		delete loadingData;
	}
	);
}

void Texture::getCubemapAsync(const String& name, std::function<void(Ref<Texture>)> callback)
{
	auto it = s_CubemapTextures.find(name);
	if (it != s_CubemapTextures.end())
	{
		auto& wref = it->second;
		if (!wref.expired())
		{
			callback(wref.lock());
			return;
		}
		else
			s_CubemapTextures.erase(it);
	}

	AsyncTextureLoadingData* data = new AsyncTextureLoadingData;
	Job::submit([name](void* _data) -> bool
	{
		AsyncTextureLoadingData* loadingData = reinterpret_cast<AsyncTextureLoadingData*>(_data);
		return loadCubemapPixels(name, &loadingData->pixels, loadingData->width, loadingData->height);
	}, data, [name, callback](bool result, void* _data)
	{
		AsyncTextureLoadingData* loadingData = reinterpret_cast<AsyncTextureLoadingData*>(_data);
		Ref<Texture> texture;
		if (result)
		{
			texture = Ref<Texture>(new Texture(loadingData->width, loadingData->height, loadingData->pixels, true));
			s_CubemapTextures.insert({ name, texture });
		}
		else
		{
			texture = s_DefaultCubemap;
		}

		callback(texture);
		delete[] loadingData->pixels;
		delete loadingData;
	}
	);
}

void Texture::makeAsync(u32 width, u32 height, glm::vec2 position, glm::vec2 dimension, std::function<glm::vec4(f32, f32)> generator, std::function<void(Ref<Texture>)> callback)
{
	f32* data = new f32[width * 4LL * height];
	Job::submit([width, height, position, dimension, generator](void* _data) -> bool
	{
		f32* pixels = reinterpret_cast<f32*>(_data);
		populateArrayGenerator(pixels, width, height, position, dimension, generator);
		return true;
	}, data, [width, height, callback](bool result, void* _data)
	{
		f32* pixels = reinterpret_cast<f32*>(_data);
		Ref<Texture> texture;
		if (result)
		{
			texture = Ref<Texture>(new Texture(width, height, pixels));
		}
		else
		{
			texture = s_DefaultCubemap;
		}

		callback(texture);
		delete[] pixels;
	}
	);
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

void Texture::loadFromPixelArray(u32 width, u32 height, u8* pixels, bool isCubeMap)
{
	VkDeviceSize imageSize = width * 4LL * height * (isCubeMap ? 6 : 1);
	if (!isCubeMap)
		m_MipLevels = static_cast<u32>(std::floor(std::log2(std::max(width, height)))) + 1;
	else
		m_MipLevels = 1;

	Buffer stagingBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.map(pixels);

	ImageCreationInfo info{};
	info.mipLevels = m_MipLevels;
	if (isCubeMap)
	{
		info.arrayLayers = 6;
		info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	}

	m_Image = Image(width, height,
					VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					info);

	m_Image.transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, info);

	m_Image.copyFromBuffer(stagingBuffer, info);
	if (m_MipLevels > 1)
		m_Image.generateMipmaps(m_MipLevels);
	else
		m_Image.transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, info);
}

Texture::Texture(u32 width, u32 height, u8* pixels, bool isCubeMap)
{
	loadFromPixelArray(width, height, pixels, isCubeMap);
}

Texture::Texture(u32 width, u32 height, f32* pixels)
{
	VkDeviceSize imageSize = width * 4LL * height * sizeof(f32);
	m_MipLevels = static_cast<u32>(std::floor(std::log2(std::max(width, height)))) + 1;

	Buffer stagingBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.map(pixels);

	ImageCreationInfo info{};
	info.mipLevels = m_MipLevels;
	info.format = VK_FORMAT_R32G32B32A32_SFLOAT;

	m_Image = Image(width, height,
					VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					info);

	m_Image.transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, info);

	m_Image.copyFromBuffer(stagingBuffer, info);
	m_Image.generateMipmaps(m_MipLevels);
}

Texture::~Texture() = default;

std::unordered_map<String, WRef<Texture>> Texture::s_Textures = {};
std::unordered_map<String, WRef<Texture>> Texture::s_CubemapTextures = {};
Ref<Texture> Texture::s_Default2D;
Ref<Texture> Texture::s_DefaultCubemap;

void Texture::makeDefaultTexture2D()
{
	const u32 width = 256;
	const u32 height = 256;
	auto pixels = std::vector<u8>(width * height * 4);

	const u32 box = 64;

	for (u64 y = 0; y < height; y++)
	{
		for (u64 x = 0; x < width; x++)
		{
			if (((x / box) + (y / box)) % 2)
			{
				// purple
				pixels[(y + x * height) * 4 + 0] = static_cast<u8>(0xff); // b
				pixels[(y + x * height) * 4 + 1] = static_cast<u8>(0x00); // g
				pixels[(y + x * height) * 4 + 2] = static_cast<u8>(0xff); // r
				pixels[(y + x * height) * 4 + 3] = static_cast<u8>(0xff); // a
			}
			else
			{
				// green
				pixels[(y + x * height) * 4 + 0] = static_cast<u8>(0x00); // b
				pixels[(y + x * height) * 4 + 1] = static_cast<u8>(0xff); // g
				pixels[(y + x * height) * 4 + 2] = static_cast<u8>(0x00); // r
				pixels[(y + x * height) * 4 + 3] = static_cast<u8>(0xff); // a
			}
		}
	}

	s_Default2D = Ref<Texture>(new Texture(width, height, pixels.data()));
}

void Texture::makeDefaultCubemap()
{
	const u32 width = 256;
	const u32 height = 256;
	auto pixels = std::vector<u8>(width * height * 4 * 6);

	const u32 box = 64;
	const u64 imgOffset = width * height * 4;

	for (u64 n = 0; n < 6; n++)
	{
		for (u64 y = 0; y < height; y++)
		{
			for (u64 x = 0; x < width; x++)
			{
				if (((x / box) + (y / box)) % 2)
				{
					// purple
					pixels[imgOffset * n + (y + x * height) * 4 + 0] = static_cast<u8>(0xff); // b
					pixels[imgOffset * n + (y + x * height) * 4 + 1] = static_cast<u8>(0x00); // g
					pixels[imgOffset * n + (y + x * height) * 4 + 2] = static_cast<u8>(0xff); // r
					pixels[imgOffset * n + (y + x * height) * 4 + 3] = static_cast<u8>(0xff); // a
				}
				else
				{
					// green
					pixels[imgOffset * n + (y + x * height) * 4 + 0] = static_cast<u8>(0x00); // b
					pixels[imgOffset * n + (y + x * height) * 4 + 1] = static_cast<u8>(0xff); // g
					pixels[imgOffset * n + (y + x * height) * 4 + 2] = static_cast<u8>(0x00); // r
					pixels[imgOffset * n + (y + x * height) * 4 + 3] = static_cast<u8>(0xff); // a
				}
			}
		}
	}

	s_DefaultCubemap = Ref<Texture>(new Texture(width, height, pixels.data(), true));
}

bool Texture::init()
{
	makeDefaultTexture2D();
	makeDefaultCubemap();

	return true;
}

void Texture::cleanup()
{
	s_Textures.clear();
	s_CubemapTextures.clear();

	s_Default2D.reset();
	s_DefaultCubemap.reset();
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
		vkDeviceWaitIdle(vulkanData.device);
		vkDestroySampler(vulkanData.device, m_Handle, vulkanData.allocator);
	}
}

bool loadTexture2DPixels(const String& name, u8** outPixels, u32& width, u32& height)
{
	String namePrefix = rendererData.resourceInfo.path + "textures/" + name;

	String format;
	for (auto& ext : supportedExtensions)
	{
		if (std::filesystem::exists((namePrefix + ext).cString()))
		{
			format = ext;
			break;
		}
	}
	if (format.isEmpty())
	{
		VUERROR("Failed to load texture for %s!", name.cString());
		return false;
	}

	String path = namePrefix + format;
	i32 texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(path.cString(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * 4LL * texHeight;

	if (!pixels)
	{
		VUERROR("Failed to load texture at %s!", path.cString());
		return false;
	}

	u8* pix = new u8[imageSize];
	std::memcpy(pix, pixels, imageSize);

	width = texWidth;
	height = texHeight;
	*outPixels = pix;

	stbi_image_free(pixels);

	return true;
}

bool loadCubemapPixels(const String& name, u8** pixelsPointer, u32& width, u32& height)
{
	String namePrefix = rendererData.resourceInfo.path + "textures/" + name;

	// r l u d f b
	String fileNames[6] = {
		namePrefix + "_r",
		namePrefix + "_l",
		namePrefix + "_u",
		namePrefix + "_d",
		namePrefix + "_f",
		namePrefix + "_b"
	};

	u8* pixels = nullptr;
	u64 imgSize = 0, offset = 0;
	for (u64 i = 0; i < 6; ++i)
	{
		i32 texWidth, texHeight, texChannels;
		String format;
		for (auto& ext : supportedExtensions)
		{
			if (std::filesystem::exists((fileNames[i] + ext).cString()))
			{
				format = ext;
				break;
			}
		}
		if (format.isEmpty())
		{
			VUERROR("Failed to load cubemap texture for %s!", name.cString());
			break;
		}

		stbi_uc* img = stbi_load((fileNames[i] + format).cString(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!img)
		{
			VUERROR("Failed to load cubemap texture at %s!", fileNames[i].cString());
			break;
		}

		if (i == 0)
		{
			width = texWidth;
			height = texHeight;
			imgSize = width * 4LL * height * 6;
			pixels = new u8[imgSize];
		}
		else
		{
			if (width != texWidth || height != texHeight)
			{
				VUERROR("Cubemap textures must have all the same size!");
				stbi_image_free(img);
				break;
			}
		}

		u64 loadedImgSize = texWidth * 4LL * texHeight;

		std::memcpy(pixels + offset, img, loadedImgSize);
		offset += loadedImgSize;

		stbi_image_free(img);
	}

	*pixelsPointer = pixels;

	return pixels && imgSize == offset;
}

} // namespace vulture
