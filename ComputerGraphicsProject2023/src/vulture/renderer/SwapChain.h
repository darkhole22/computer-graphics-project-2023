#pragma once

#include "Buffers.h"
#include "RenderPass.h"

#include <array>

namespace vulture {

const int MAX_FRAMES_IN_FLIGHT = 2;

class SwapChain
{
public:
	SwapChain(const Window& window);

	bool attachRenderPass(const RenderPass& renderPass);

	inline VkFormat getImageFormat() const { return m_ImageFormat; }

	u32 getImageIndex(u32 currentFrame);
	CommandBuffer& getCommandBuffer(u32 currentFrame);
	void submit(u32 currentFrame, u32 imageIndex);

	inline const VkExtent2D& getExtent() const { return m_Extent; }
	inline u32 getImageCount() const { return static_cast<u32>(m_Images.size()); }
	inline bool wasRecreated() const { return m_Recreated; }

	~SwapChain();

	friend class FrameContext;
private:
	VkSwapchainKHR m_Handle = VK_NULL_HANDLE;
	VkFormat m_ImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D m_Extent = {600, 400};
	std::vector<Image> m_Images;
	Image m_ColorImage;
	Image m_DepthImage;
	std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_ImageAvailableSemaphores;
	std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_RenderFinishedSemaphores;
	std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_InFlightFences;
	std::vector<VkFence> m_ImageInFlightFences;
	std::vector<CommandBuffer> m_CommandBuffers;

	RenderPass const* m_RenderPass = nullptr;
	std::vector<VkFramebuffer> m_Framebuffers;

	bool m_Recreated = false;
	Window const* m_Window = nullptr;

	void create();
	bool createFrameBuffers();
	void recreate();
	void cleanup();
};

} // namespace vulture
