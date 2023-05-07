#pragma once

#include "Buffers.h"

namespace vulture {

constexpr const String DEFAULT_TEXTURE_NAME = "__default__texture2D__";

enum class TextureType
{
	TEXTURE_2D,
	CUBE_MAP
};

class Texture
{
public:
	NO_COPY(Texture)

	/**
	* @brief Returns a referance to the specified texture or to the default texture if `name` is invalid.
	* 
	* @param name: the texture unique identifier (not the file name).
	* 
	* @return a referance to the texture.
	*/
	static Ref<Texture> get(const String& name, TextureType type = TextureType::TEXTURE_2D);

	inline VkImageView getView() const { return m_Image.getView(); }
	inline u32 getMipLevels() const { return m_MipLevels; };
	inline VkImageLayout getLayout() const { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }

	~Texture();

	friend class Renderer;
private:
	Texture(const String& path);
	Texture(u32 width, u32 heigth, u8* pixels, bool isCubeMap = false);

	void loadFromPixelArray(u32 width, u32 heigth, u8* pixels, bool isCubeMap = false);

	Image m_Image;
	u32 m_MipLevels = 0;

	static bool init();
	static void cleanup();

	static std::unordered_map<String, WRef<Texture>> s_Textures;
	static Ref<Texture> s_Default2D;
	static Ref<Texture> s_DefaultCubemap;

	static void makeDefaultTexture2D();
	static void makeDefaultCubemap();
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
