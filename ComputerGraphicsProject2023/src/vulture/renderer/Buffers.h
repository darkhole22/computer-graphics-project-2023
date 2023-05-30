#pragma once

#include "vulture/core/Core.h"
#include "vulture/event/Event.h"
#include "Window.h"
#include "RenderPass.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace vulture {

class Pipeline;
class DescriptorSet;
class Buffer;
class SwapChain;

class CommandBuffer
{
public:
	static std::vector<CommandBuffer> getCommandBuffers(u32 count);

	CommandBuffer() = default;
	CommandBuffer(const CommandBuffer& other) = delete;
	CommandBuffer(CommandBuffer&& other) noexcept;
	explicit CommandBuffer(bool singleTime);

	CommandBuffer operator=(const CommandBuffer& other) = delete;
	CommandBuffer& operator=(CommandBuffer&& other) noexcept;

	inline VkCommandBuffer getHandle() const { return m_Handle; }

	inline void reset() { vkResetCommandBuffer(m_Handle, 0); }
	void begin();
	void beginRenderPass(const RenderPass& renderPass, VkFramebuffer frameBuffer, VkExtent2D extent);
	void bindPipeline(const Pipeline& pipeline, const SwapChain& swapChain);
	void bindDescriptorSet(const Pipeline& pipeline, VkDescriptorSet descriptorSet, u32 set);
	void bindVertexBuffer(const Buffer& buffer);
	void bindIndexBuffer(const Buffer& buffer);
	void drawIndexed(u32 indexCount);
	void endRenderPass();
	void end();

	~CommandBuffer();
private:
	VkCommandBuffer m_Handle = VK_NULL_HANDLE;
	bool m_SingleTime = false;

	void cleanup() noexcept;
};

struct ImageCreationInfo
{
	u32 mipLevels = 1;
	u8 arrayLayers = 1;
	VkImageCreateFlags flags = 0;
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
	VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
	VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
	VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

	static ImageCreationInfo defaultImageCreateInfo;
};

class Image
{
public:
	Image() = default;
	Image(const Image& other) = delete;
	Image(Image&& other) noexcept;
	Image(VkImage image, VkFormat format);
	Image(u32 width, u32 height, VkImageUsageFlags usage, const ImageCreationInfo& info = ImageCreationInfo::defaultImageCreateInfo);

	inline VkImage getHandle() const { return m_Handle; }
	inline VkImageView getView() const { return m_View; }
	inline VkFormat getFormat() const { return m_Format; }
	inline u32 getWidth() const { return static_cast<u32>(m_Width < 0 ? 0 : m_Width); }
	inline u32 getHeight() const { return static_cast<u32>(m_Height < 0 ? 0 : m_Height); }

	void transitionLayout(VkImageLayout newLayout, const ImageCreationInfo& info = ImageCreationInfo::defaultImageCreateInfo, u32 baseArrayLayer = 0);
	void copyFromBuffer(const Buffer& buffer, const ImageCreationInfo& info = ImageCreationInfo::defaultImageCreateInfo);
	void generateMipmaps(u32 mipLevels, u32 layerCount = 1);

	Image operator=(const Image& other) = delete;
	Image& operator=(Image&& other) noexcept;

	~Image();
private:
	VkImage m_Handle = VK_NULL_HANDLE;
	VkImageView m_View = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;
	VkImageLayout m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkFormat m_Format = VK_FORMAT_UNDEFINED;

	i64 m_Width = -1;
	i64 m_Height = -1;

	void cleanup() noexcept;
};

class Buffer
{
public:
	Buffer() = default;
	Buffer(const Buffer& other) = delete;
	Buffer(Buffer&& other) noexcept;

	Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* data = nullptr);

	inline VkBuffer getHandle() const { return m_Handle; }
	inline VkDeviceMemory getMemory() const { return m_Memory; }
	inline VkDeviceSize getSize() const { return m_Size; }

	void map(void* data) const;
	void map() const;
	void copyToBuffer(VkDeviceSize size, const Buffer& destination) const;

	Buffer& operator=(const Buffer& other) = delete;
	Buffer& operator=(Buffer&& other) noexcept;

	inline bool operator==(const Buffer& other) const { return other.m_Handle == m_Handle; }

	~Buffer();
private:
	VkBuffer m_Handle = VK_NULL_HANDLE;
	VkDeviceMemory m_Memory = VK_NULL_HANDLE;

	VkDeviceSize m_Size = 0;
	void* m_Data = nullptr;

	void cleanup() noexcept;
};

} // namespace vulture
