#include "Buffers.h"

#include "VulkanContext.h"
#include "SwapChain.h"
#include "Pipeline.h"

// #define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Logger.h"

#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <optional>
#include <set>
#include <algorithm>


#ifdef VUDEBUG
// DEBUG
#define VALIDATION_LAYER true
#define VALIDATION_LAYER_IF(x) x
#else
// RELEASE
#define VALIDATION_LAYER false
#define VALIDATION_LAYER_IF(x)
#endif // !NDEBUG

#define ASSERT_VK_SUCCESS(func, message)   \
	if (func != VK_SUCCESS)                \
	{                                      \
		VUERROR(message);                  \
		throw std::runtime_error(message); \
	}

namespace vulture
{

extern VulkanContextData vulkanData;

CommandBuffer::CommandBuffer(CommandBuffer &&other) noexcept
{
	m_Handle = other.m_Handle;
	m_SingleTime = other.m_SingleTime;

	other.m_Handle = VK_NULL_HANDLE;
	other.m_SingleTime = false;
}

CommandBuffer::CommandBuffer(bool singleTime)
{
	m_SingleTime = singleTime;

	VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	allocInfo.pNext = nullptr;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vulkanData.commandPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(vulkanData.device, &allocInfo, &m_Handle);

	if (m_SingleTime)
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.pNext = nullptr;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(m_Handle, &beginInfo);
	}
}

std::vector<CommandBuffer> CommandBuffer::getCommandBuffers(u32 count)
{
	std::vector<CommandBuffer> buffers(count);
	std::vector<VkCommandBuffer> handles(count);

	VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	allocInfo.pNext = nullptr;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vulkanData.commandPool;
	allocInfo.commandBufferCount = count;

	ASSERT_VK_SUCCESS(vkAllocateCommandBuffers(vulkanData.device, &allocInfo, handles.data()), "Failed to allocate command buffers!");

	for (size_t i = 0; i < buffers.size(); i++)
	{
		buffers[i].m_Handle = handles[i];
	}

	return buffers;
}

CommandBuffer &CommandBuffer::operator=(CommandBuffer &&other) noexcept
{
	if (m_Handle != other.m_Handle)
	{
		cleanup();

		m_Handle = other.m_Handle;
		m_SingleTime = other.m_SingleTime;

		other.m_Handle = VK_NULL_HANDLE;
		other.m_SingleTime = false;
	}

	return *this;
}

void CommandBuffer::begin()
{
	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.pNext = nullptr;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	ASSERT_VK_SUCCESS(vkBeginCommandBuffer(m_Handle, &beginInfo), "Failed to begin recording command buffer!");
}

void CommandBuffer::beginRenderPass(const RenderPass &renderPass, VkFramebuffer frameBuffer, VkExtent2D extent)
{
	auto &clearValues = renderPass.getClearValues();

	VkRenderPassBeginInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	renderPassInfo.pNext = nullptr;
	renderPassInfo.renderPass = renderPass.getHandle();
	renderPassInfo.framebuffer = frameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;
	renderPassInfo.clearValueCount = static_cast<u32>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_Handle, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::bindPipeline(const Pipeline &pipeline, const SwapChain &swapChain)
{
	vkCmdBindPipeline(m_Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getHandle());

	auto &extent = swapChain.getExtent();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_Handle, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = extent;
	vkCmdSetScissor(m_Handle, 0, 1, &scissor);
}

void CommandBuffer::bindDescriptorSet(const Pipeline &pipeline, VkDescriptorSet descriptorSet, u32 set)
{
	vkCmdBindDescriptorSets(m_Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getLayout(), set, 1, &descriptorSet, 0, nullptr);
}

void CommandBuffer::bindVertexBuffer(const Buffer &buffer)
{
	VkBuffer vertexBuffers[] = {buffer.getHandle()};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(m_Handle, 0, 1, vertexBuffers, offsets);
}

void CommandBuffer::bindIndexBuffer(const Buffer &buffer)
{
	vkCmdBindIndexBuffer(m_Handle, buffer.getHandle(), 0, VK_INDEX_TYPE_UINT32);
}

void CommandBuffer::drawIndexed(u32 indexCount)
{
	vkCmdDrawIndexed(m_Handle, indexCount, 1, 0, 0, 0);
}

void CommandBuffer::endRenderPass()
{
	vkCmdEndRenderPass(m_Handle);
}

void CommandBuffer::end()
{
	ASSERT_VK_SUCCESS(vkEndCommandBuffer(m_Handle), "Failed to record command buffer!");
}

CommandBuffer::~CommandBuffer()
{
	cleanup();
}

void CommandBuffer::cleanup() noexcept
{
	if (m_Handle != VK_NULL_HANDLE)
	{
		if (m_SingleTime)
		{
			vkEndCommandBuffer(m_Handle);

			VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_Handle;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.pSignalSemaphores = nullptr;

			vkQueueSubmit(vulkanData.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		}
		vkQueueWaitIdle(vulkanData.graphicsQueue);
		vkFreeCommandBuffers(vulkanData.device, vulkanData.commandPool, 1, &m_Handle);
	}
}

Image::Image(Image &&other) noexcept
{
	m_Handle = other.m_Handle;
	m_Memory = other.m_Memory;
	m_View = other.m_View;
	m_Layout = other.m_Layout;
	m_Format = other.m_Format;
	m_Width = other.m_Width;
	m_Height = other.m_Height;

	other.m_Handle = VK_NULL_HANDLE;
	other.m_Memory = VK_NULL_HANDLE;
	other.m_View = VK_NULL_HANDLE;
	other.m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
	other.m_Format = VK_FORMAT_UNDEFINED;
	other.m_Width = -1;
	other.m_Height = -1;
}

VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageView view;

	VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.pNext = nullptr;
	viewInfo.flags = 0;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	// viewInfo.components;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	ASSERT_VK_SUCCESS(vkCreateImageView(vulkanData.device, &viewInfo, vulkanData.allocator, &view),
		"Failed to create texture image view!");

	return view;
}

Image::Image(VkImage image, VkFormat format) : m_Memory(VK_NULL_HANDLE), m_View(VK_NULL_HANDLE), m_Format(format)
{
	m_Handle = image;
	m_View = createImageView(image, format, VK_IMAGE_ASPECT_COLOR_BIT);
}

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	VUERROR("Failed to find suitable memory type!");
	throw std::runtime_error("Failed to find suitable memory type!");
}

Image::Image(u32 width, u32 height, u32 mipLevels,
	VkSampleCountFlagBits numSamples, VkFormat format,
	VkImageTiling tiling, VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags)
	: m_Format(format), m_Width(width), m_Height(height)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = numSamples;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	ASSERT_VK_SUCCESS(vkCreateImage(vulkanData.device, &imageInfo, vulkanData.allocator, &m_Handle), "Failed to create image!");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(vulkanData.device, m_Handle, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(vulkanData.physicalDevice, memRequirements.memoryTypeBits, properties);

	ASSERT_VK_SUCCESS(vkAllocateMemory(vulkanData.device, &allocInfo, vulkanData.allocator, &m_Memory),
		"Failed to allocate image memory!");

	vkBindImageMemory(vulkanData.device, m_Handle, m_Memory, 0);

	m_View = createImageView(m_Handle, format, aspectFlags);
}

inline bool hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Image::transitionLayout(VkImageLayout newLayout, u32 mipLevels)
{
	CommandBuffer commandBuffer(true);

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = m_Layout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_Handle;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (hasStencilComponent(m_Format))
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1; // TODO add layer parameter

	if (m_Layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (m_Layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (m_Layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		VUERROR("Trying to transition the layout of an image from %i to %i!", m_Layout, newLayout);
		return;
	}

	vkCmdPipelineBarrier(
		commandBuffer.getHandle(),
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	m_Layout = newLayout;
}

void Image::copyFromBuffer(const Buffer &buffer)
{
	CommandBuffer commandBuffer(true);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = {0, 0, 0};
	region.imageExtent = {
		static_cast<u32>(m_Width),
		static_cast<u32>(m_Height),
		1};

	vkCmdCopyBufferToImage(
		commandBuffer.getHandle(),
		buffer.getHandle(),
		m_Handle,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region);
}

void Image::generateMipmaps(u32 mipLevels)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(vulkanData.physicalDevice, m_Format, &formatProperties);
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		VUERROR("Texture image format (%i) does not support linear blitting!", m_Format);
		throw std::runtime_error("Texture image format does not support linear blitting!");
	}

	CommandBuffer commandBuffer(true);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = m_Handle;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	i32 mipWidth = static_cast<i32>(m_Width);
	i32 mipHeight = static_cast<i32>(m_Height);

	for (u32 i = 1; i < mipLevels; i++)
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer.getHandle(),
								VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
								0, nullptr,
								0, nullptr,
								1, &barrier);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer.getHandle(),
						m_Handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						m_Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1, &blit,
						VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer.getHandle(),
								VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
								0, nullptr,
								0, nullptr,
								1, &barrier);

		if (mipWidth > 1)
			mipWidth /= 2;
		if (mipHeight > 1)
			mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer.getHandle(),
							VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
							0, nullptr,
							0, nullptr,
							1, &barrier);
}

Image &Image::operator=(Image &&other) noexcept
{
	if (m_Handle != other.m_Handle)
	{
		cleanup();

		m_Handle = other.m_Handle;
		m_Memory = other.m_Memory;
		m_View = other.m_View;
		m_Layout = other.m_Layout;
		m_Format = other.m_Format;
		m_Width = other.m_Width;
		m_Height = other.m_Height;

		other.m_Handle = VK_NULL_HANDLE;
		other.m_Memory = VK_NULL_HANDLE;
		other.m_View = VK_NULL_HANDLE;
		other.m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
		other.m_Format = VK_FORMAT_UNDEFINED;
		other.m_Width = -1;
		other.m_Height = -1;
	}

	return *this;
}

Image::~Image()
{
	cleanup();
}

void Image::cleanup() noexcept
{
	if (m_Handle == VK_NULL_HANDLE)
		return;

	if (m_View != VK_NULL_HANDLE)
		vkDestroyImageView(vulkanData.device, m_View, vulkanData.allocator);
	if (m_Memory != VK_NULL_HANDLE)
	{
		vkDestroyImage(vulkanData.device, m_Handle, vulkanData.allocator);
		vkFreeMemory(vulkanData.device, m_Memory, vulkanData.allocator);
	}
}

Buffer::Buffer(Buffer &&other) noexcept
{
	m_Handle = other.m_Handle;
	m_Memory = other.m_Memory;
	m_Size = other.m_Size;
	m_Data = other.m_Data;

	other.m_Handle = VK_NULL_HANDLE;
	other.m_Memory = VK_NULL_HANDLE;
	other.m_Size = 0;
	other.m_Data = nullptr;
}

Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void *data)
	: m_Size(size), m_Data(data)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	ASSERT_VK_SUCCESS(vkCreateBuffer(vulkanData.device, &bufferInfo, vulkanData.allocator, &m_Handle),
						"Failed to create buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vulkanData.device, m_Handle, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(vulkanData.physicalDevice, memRequirements.memoryTypeBits, properties);

	ASSERT_VK_SUCCESS(vkAllocateMemory(vulkanData.device, &allocInfo, vulkanData.allocator, &m_Memory),
						"Failed to allocate buffer memory!");

	vkBindBufferMemory(vulkanData.device, m_Handle, m_Memory, 0);
}

void Buffer::map(void *data) const
{
	if (!data)
	{
		VUWARN("Trying to map a null pointer.");
		return;
	}

	void *tmp;

	vkMapMemory(vulkanData.device, m_Memory, 0, m_Size, 0, &tmp);
	memcpy(tmp, data, m_Size);
	vkUnmapMemory(vulkanData.device, m_Memory);
}

void Buffer::map() const
{
	map(m_Data);
}

void Buffer::copyToBuffer(VkDeviceSize size, const Buffer &destination) const
{
	CommandBuffer commandBuffer(true);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer.getHandle(), m_Handle, destination.m_Handle, 1, &copyRegion);
}

Buffer &Buffer::operator=(Buffer &&other) noexcept
{
	if (m_Handle != other.m_Handle)
	{
		cleanup();

		m_Handle = other.m_Handle;
		m_Memory = other.m_Memory;
		m_Size = other.m_Size;
		m_Data = other.m_Data;

		other.m_Handle = VK_NULL_HANDLE;
		other.m_Memory = VK_NULL_HANDLE;
		other.m_Size = 0;
		other.m_Data = nullptr;
	}

	return *this;
}

Buffer::~Buffer()
{
	cleanup();
}

void Buffer::cleanup() noexcept
{
	if (m_Handle != VK_NULL_HANDLE)
	{
		vkDeviceWaitIdle(vulkanData.device);

		vkFreeMemory(vulkanData.device, m_Memory, nullptr);
		vkDestroyBuffer(vulkanData.device, m_Handle, nullptr);
	}
}

} // namespace vulture
