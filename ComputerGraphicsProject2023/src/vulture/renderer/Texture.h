#pragma once

#include "Buffers.h"

namespace vulture {

/**
 * @brief Enumeration representing the type of texture.
 */
enum class TextureType
{
	TEXTURE_2D,
	CUBE_MAP
};

/**
 * @brief Class representing a texture.
 *
 *  This class also provides static methods to load textures from files or to construct them algorithmically
 */
class Texture
{
public:
	NO_COPY(Texture)
public:
	/**
	* @brief Static function to retrieve a reference to the specified 2D texture or the default texture if `name` is invalid.
	*
	* @param name The unique identifier (not the file name) of the 2D texture to retrieve.
	*
	* @return A reference to the 2D texture.
	*/
	static Ref<Texture> get(const String& name);

	/**
	 * @brief Static function to retrieve a reference to the specified cube map texture or the default cube map texture if `name` is invalid.
	 *
	 * The files to be loaded must follow this naming convention:
	 * - `name_l`: Left side texture
	 * - `name_r`: Right side texture
	 * - `name_u`: Top side texture
	 * - `name_d`: Bottom side texture
	 * - `name_b`: Back side texture
	 * - `name_f`: Front side texture
	 *
	 * @param name The unique identifier of the cube map texture to retrieve.
	 * @return A reference to the cube map texture.
	 */
	static Ref<Texture> getCubemap(const String& name);

	/**
	 * @brief Static function to create a new texture with the specified parameters and a user-provided pixel data generator.
	 *
	 * @param width The width of the texture.
	 * @param height The height of the texture.
	 * @param position The position of the texture in 2D space.
	 * @param dimension The dimension of the texture in 2D space.
	 * @param generator The user-provided function to generate pixel data for the texture.
	 * @return A reference to the newly created texture.
	 */
	static Ref<Texture> make(u32 width, u32 height, glm::vec2 position, glm::vec2 dimension, std::function<glm::vec4(f32, f32)> generator);

	/**
	 * @brief Static function to asynchronously retrieve a reference to the specified 2D texture and call a user-provided callback function.
	 *
	 * @param name The unique identifier of the 2D texture to retrieve.
	 * @param callback The user-provided callback function to be called with the reference to the texture.
	 */
	static void getAsync(const String& name, std::function<void(Ref<Texture>)> callback);

	/**
	 * @brief Static function to asynchronously retrieve a reference to the specified cube map texture and call a user-provided callback function.
	 *
	 * @param name The unique identifier of the cube map texture to retrieve.
	 * @param callback The user-provided callback function to be called with the reference to the texture.
	 */
	static void getCubemapAsync(const String& name, std::function<void(Ref<Texture>)> callback);

	/**
	 * @brief Static function to asynchronously create a new texture with the specified parameters and a user-provided pixel data generator,
	 *        and call a user-provided callback function with the reference to the newly created texture.
	 *
	 * @param width The width of the texture.
	 * @param height The height of the texture.
	 * @param position The position of the texture in 2D space.
	 * @param dimension The dimension of the texture in 2D space.
	 * @param generator The user-provided function to generate pixel data for the texture.
	 * @param callback The user-provided callback function to be called with the reference to the texture.
	 */
	static void makeAsync(u32 width, u32 height, glm::vec2 position, glm::vec2 dimension, std::function<glm::vec4(f32, f32)> generator, std::function<void(Ref<Texture>)> callback);

	/**
	 * @brief Gets the ImageView associated with the texture.
	 *
	 * @return The image view of the texture.
	 */
	inline VkImageView getView() const { return m_Image.getView(); }

	/**
	 * @brief Gets the number of mip levels in the texture.
	 *
	 * @return The number of mip levels.
	 */
	inline u32 getMipLevels() const { return m_MipLevels; };

	/**
	 * @brief Gets the image layout of the texture.
	 *
	 * @return The image layout.
	 */
	inline VkImageLayout getLayout() const { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }

	/**
	 * @brief Gets the width of the texture.
	 *
	 * @return The width of the texture.
	 */
	inline u32 getWidth() const { return m_Image.getWidth(); }

	/**
	 * @brief Gets the height of the texture.
	 *
	 * @return The height of the texture.
	 */
	inline u32 getHeight() const { return m_Image.getHeight(); }

	~Texture();

	friend class Renderer;
private:
	Texture(const String& path);

	/**
	 * @brief Constructor for the `Texture` class, creating the texture from pixel data.
	 *
	 * @param width The width of the texture.
	 * @param height The height of the texture.
	 * @param pixels A pointer to the pixel data of the texture.
	 * @param isCubeMap Flag indicating whether the texture is a cube map.
	 */
	Texture(u32 width, u32 height, u8* pixels, bool isCubeMap = false);

	/**
	 * @brief Constructor for the `Texture` class, creating the texture from floating-point pixel data.
	 *
	 * @param width The width of the texture.
	 * @param height The height of the texture.
	 * @param pixels A pointer to the floating-point pixel data of the texture.
	 */
	Texture(u32 width, u32 height, f32* pixels);

	/**
	 * @brief Helper function to load the texture from a pixel array.
	 *
	 * @param width The width of the texture.
	 * @param height The height of the texture.
	 * @param pixels A pointer to the pixel data of the texture.
	 * @param isCubeMap Flag indicating whether the texture is a cube map.
	 */
	void loadFromPixelArray(u32 width, u32 height, u8* pixels, bool isCubeMap = false);

	Image m_Image;
	u32 m_MipLevels = 0;

	/**
	 * @brief Static function to initialize resources and prepare for loading textures.
	 *
	 * @return True if initialization is successful; otherwise, false.
	 */
	static bool init();

	/**
	 * @brief Static function to clean up and release resources used by the `Texture` class.
	 */
	static void cleanup();

	static std::unordered_map<String, WRef<Texture>> s_Textures;
	static std::unordered_map<String, WRef<Texture>> s_CubemapTextures;
	static Ref<Texture> s_Default2D;
	static Ref<Texture> s_DefaultCubemap;

	static void makeDefaultTexture2D();
	static void makeDefaultCubemap();
};

/**
 * @brief Structure representing the configuration of a texture sampler.
 */
struct TextureSamplerConfig
{
	VkFilter magFilter = VK_FILTER_LINEAR; /**< The magnification filter mode. */
	VkFilter minFilter = VK_FILTER_LINEAR; /**< The minification filter mode. */
	VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; /**< The addressing mode for the U (x) texture coordinate. */
	VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; /**< The addressing mode for the V (y) texture coordinate. */
	VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; /**< The addressing mode for the W (z) texture coordinate. */
	VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; /**< The mipmap filter mode. */
	bool useAnisotropy = true; /**< Flag indicating whether to use anisotropic filtering. */

	/**
	 * @brief Helper function to set the same filter for both magnification and minification.
	 *
	 * @param filter The filter to set for both magnification and minification.
	 */
	inline void setFilters(VkFilter filter)
	{
		magFilter = filter;
		minFilter = filter;
	}

	/**
	 * @brief Helper function to set the same address mode for all texture coordinate axes.
	 *
	 * @param mode The address mode to set for all texture coordinate axes.
	 */
	inline void setAddressMode(VkSamplerAddressMode mode)
	{
		addressModeU = mode;
		addressModeV = mode;
		addressModeW = mode;
	}

	/**
	 * @brief Default configuration for the texture sampler.
	 */
	static const TextureSamplerConfig defaultConfig;
};

/**
 * @brief Class representing a texture sampler.
 */
class TextureSampler
{
public:
	NO_COPY(TextureSampler)
public:
	/**
	 * @brief Constructor for the `TextureSampler` class.
	 *
	 * @param texture The texture to create a sampler for.
	 * @param config The configuration for the texture sampler.
	 */
	TextureSampler(const Texture& texture, const TextureSamplerConfig& config = TextureSamplerConfig::defaultConfig);

	/**
	 * @brief Gets the handle to the Vulkan sampler object.
	 *
	 * @return The handle to the Vulkan sampler.
	 */
	inline VkSampler getHandle() const { return m_Handle; };


	/**
	 * @brief Gets the image view associated with the texture sampler.
	 *
	 * @return The image view associated with the texture sampler.
	 */
	inline VkImageView getView() const { return m_View; }

	/**
	 * @brief Gets the image layout of the texture sampler.
	 *
	 * @return The image layout of the texture sampler.
	 */
	inline VkImageLayout getLayout() const { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }

	~TextureSampler();
private:
	VkSampler m_Handle;
	VkImageView m_View;
	VkImageLayout m_Layout;
};

} // namespace vulture
