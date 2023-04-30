#include "SwapChain.h"

#include "VulkanContext.h"
#include "vulture/core/Logger.h"

#include <algorithm>
#include <limits>

namespace vulture {
	
extern VulkanContextData vulkanData;

SwapChain::SwapChain(const Window& window)
	: m_Window(&window)
{
	create();

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkCreateSemaphore(vulkanData.device, &semaphoreInfo, vulkanData.allocator, &m_ImageAvailableSemaphores[i]);//, "Failed to create image semaphores!");
		vkCreateSemaphore(vulkanData.device, &semaphoreInfo, vulkanData.allocator, &m_RenderFinishedSemaphores[i]);//, "Failed to create render semaphores!");
		vkCreateFence(vulkanData.device, &fenceInfo, vulkanData.allocator, &m_InFlightFences[i]);//  , "Failed to create Fences!");
	}
}

bool SwapChain::attachRenderPass(const RenderPass& renderPass)
{
	m_RenderPass = &renderPass;
	return createFrameBuffers();
}

CommandBuffer& SwapChain::getCommandBuffer(u32 currentFrame)
{
	return m_CommandBuffers[currentFrame];
}

u32 SwapChain::getImageIndex(u32 currentFrame)
{
	if (!m_RenderPass)
	{
		VUERROR("Trying to get an image index without an attached RenderPass!");
		throw std::runtime_error("Trying to get an image index without an attached RenderPass!");
	}
	m_Recreated = false;

	u32 imageIndex;
	VkResult result;
	do
	{
		vkWaitForFences(vulkanData.device, 1, &m_InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		result = vkAcquireNextImageKHR(vulkanData.device, m_Handle, UINT64_MAX, m_ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreate();
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			VUERROR("Failed to acquire swap chain image!");
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

	} while (result == VK_ERROR_OUT_OF_DATE_KHR);

	if (m_ImageInFlightFences[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(vulkanData.device, 1, &m_ImageInFlightFences[imageIndex], VK_TRUE, UINT64_MAX);
	}
	m_ImageInFlightFences[imageIndex] = m_InFlightFences[currentFrame];

	return imageIndex;
}


void SwapChain::submit(u32 currentFrame, u32 imageIndex)
{
	vkResetFences(vulkanData.device, 1, &m_InFlightFences[currentFrame]);

	VkCommandBuffer commandBuffer = m_CommandBuffers[imageIndex].getHandle();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(vulkanData.graphicsQueue, 1, &submitInfo, m_InFlightFences[currentFrame]) != VK_SUCCESS)
	{
		VUERROR("Failed to submit draw command buffer!");
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_Handle };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	VkResult result = vkQueuePresentKHR(vulkanData.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->getFrameBufferResized())
	{
		recreate();
	}
	else if (result != VK_SUCCESS)
	{
		VUERROR("Failed to present swap chain image!");
		throw std::runtime_error("Failed to present swap chain image!");
	}
}

SwapChain::~SwapChain()
{
	if (m_Handle == VK_NULL_HANDLE)
		return;

	cleanup();

	for (i64 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(vulkanData.device, m_ImageAvailableSemaphores[i], vulkanData.allocator);
		vkDestroySemaphore(vulkanData.device, m_RenderFinishedSemaphores[i], vulkanData.allocator);
		vkDestroyFence(vulkanData.device, m_InFlightFences[i], vulkanData.allocator);
	}

	m_Handle = VK_NULL_HANDLE;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat()
{
	u32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanData.physicalDevice, vulkanData.surface, &formatCount, nullptr);

	std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanData.physicalDevice, vulkanData.surface, &formatCount, availableFormats.data());

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}

	return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode()
{
	// TODO add configurations (vSync, power saving mode).
	u32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanData.physicalDevice, vulkanData.surface, &presentModeCount, nullptr);

	std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanData.physicalDevice, vulkanData.surface, &presentModeCount, availablePresentModes.data());
	
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;

	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

void SwapChain::create()
{
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanData.physicalDevice, vulkanData.surface, &capabilities);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat();
	VkPresentModeKHR presentMode = chooseSwapPresentMode();
	if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
	{
		m_Extent = capabilities.currentExtent;
	}
	else
	{
		auto [width, height] = m_Window->getFramebufferSize();

		VkExtent2D actualExtent = {
			std::clamp(static_cast<u32>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp(static_cast<u32>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height) };

		m_Extent = actualExtent;
	}
	m_ImageFormat = surfaceFormat.format;

	u32 imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		imageCount = capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = vulkanData.surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = m_Extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	u32 queueFamilyIndices[] = { vulkanData.graphicsQueueFamily, vulkanData.presentQueueFamily };

	if (queueFamilyIndices[0] != queueFamilyIndices[1])
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(vulkanData.device, &createInfo, vulkanData.allocator, &m_Handle);
	if (result != VK_SUCCESS)
	{
		VUERROR("Failed to create swap chain!");
	}

	std::vector<VkImage> vkImages;
	vkGetSwapchainImagesKHR(vulkanData.device, m_Handle, &imageCount, nullptr);
	vkImages.resize(imageCount);
	vkGetSwapchainImagesKHR(vulkanData.device, m_Handle, &imageCount, vkImages.data());

	VkFormat m_ImageFormat = surfaceFormat.format; // TODO rename

	m_Images.reserve(imageCount);
	for (u64 i = 0; i < imageCount; i++)
	{
		m_Images.emplace_back(vkImages[i], m_ImageFormat);
	}

	m_ColorImage = Image(m_Extent.width, m_Extent.height, 1, vulkanData.msaaSamples,
		m_ImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

	VkFormat depthFormat = vulkanData.depthFormat;

	m_DepthImage = Image(m_Extent.width, m_Extent.height, 1, vulkanData.msaaSamples,
		depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	m_DepthImage.transitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

	m_ImageInFlightFences.clear();
	m_ImageInFlightFences.resize(imageCount, VK_NULL_HANDLE);

	m_CommandBuffers = CommandBuffer::getCommandBuffers(imageCount);
}

bool SwapChain::createFrameBuffers()
{
	if (!m_RenderPass)
	{
		VUERROR("Trying to create frame buffers without an attached RenderPass!");
		return false;
	}

	u64 imageCount = m_Images.size();
	m_Framebuffers.resize(imageCount);

	for (size_t i = 0; i < imageCount; i++)
	{
		std::array<VkImageView, 3> attachments = {
			m_ColorImage.getView(),
			m_DepthImage.getView(),
			m_Images[i].getView() };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass->getHandle();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_Extent.width;
		framebufferInfo.height = m_Extent.height;
		framebufferInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(vulkanData.device, &framebufferInfo, vulkanData.allocator, &m_Framebuffers[i]);
		if (result != VK_SUCCESS)
		{
			VUERROR("Failed to create framebuffer!");
			return false;
		}
	}
	return true;
}

void SwapChain::recreate()
{
	vkDeviceWaitIdle(vulkanData.device);

	cleanup();
	create();
	createFrameBuffers(); // TODO handle error

	m_Recreated = true;
}

void SwapChain::cleanup()
{
	m_Images.clear();

	for (auto& m_Framebuffer : m_Framebuffers)
	{
		vkDestroyFramebuffer(vulkanData.device, m_Framebuffer, vulkanData.allocator);
	}
	m_Framebuffers.clear();

	vkDestroySwapchainKHR(vulkanData.device, m_Handle, vulkanData.allocator);
}

} // namespace vulture
