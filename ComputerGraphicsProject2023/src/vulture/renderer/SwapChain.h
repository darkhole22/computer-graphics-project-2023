#pragma once

#include "Buffers.h"
#include "RenderPass.h"

#include <array>

namespace vulture {

const int MAX_FRAMES_IN_FLIGHT = 2; /**< Maximum number of frames in flight (double buffering). */

/**
 * @class SwapChain
 *
 * @brief Represents a Vulkan swap chain used for rendering images to the screen.
 */
class SwapChain
{
public:
	/**
	 * @brief Constructor for the SwapChain class.
	 *
	 * @param window The window associated with the swap chain.
	 */
	explicit SwapChain(const Window& window);

	/**
	 * @brief Attaches the specified RenderPass to the swap chain.
	 *
	 * @param renderPass The RenderPass to be attached.
	 * @return True if the RenderPass is successfully attached; otherwise, false.
	 */
	bool attachRenderPass(const RenderPass& renderPass);

	/**
	 * @brief Gets the image format used by the swap chain.
	 *
	 * @return The Vulkan format of the swap chain images.
	 */
	inline VkFormat getImageFormat() const { return m_ImageFormat; }

	/**
	 * @brief Gets the index of the image to be used for the current frame.
	 *
	 * @param currentFrame The index of the current frame.
	 * @return The index of the image to be used for rendering in the current frame.
	 */
	u32 getImageIndex(u32 currentFrame);


	/**
	 * @brief Gets the CommandBuffer to be used for rendering in the current frame.
	 *
	 * @param currentFrame The index of the current frame.
	 * @return The CommandBuffer to be used for rendering in the current frame.
	 */
	CommandBuffer& getCommandBuffer(u32 currentFrame);

	/**
	 * @brief Submits the current frame for rendering.
	 *
	 * @param currentFrame The index of the current frame.
	 * @param imageIndex The index of the image used for rendering in the current frame.
	 */
	void submit(u32 currentFrame, u32 imageIndex);

	/**
	 * @brief Gets the extent (width and height) of the swap chain images.
	 *
	 * @return The extent of the swap chain images.
	 */
	inline const VkExtent2D& getExtent() const { return m_Extent; }

	/**
	 * @brief Gets the total number of images in the swap chain.
	 *
	 * @return The total number of images in the swap chain.
	 */
	inline u32 getImageCount() const { return static_cast<u32>(m_Images.size()); }

	/**
	 * @brief Checks if the swap chain was recreated.
	 *
	 * @return True if the swap chain was recreated; otherwise, false.
	 */
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
	std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_ImageAvailableSemaphores; /**< Semaphores for image availability. */
	std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_RenderFinishedSemaphores; /**< Semaphores for rendering completion. */
	std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_InFlightFences; /**< Fences for synchronization between CPU and GPU. */
	std::vector<VkFence> m_ImageInFlightFences; /**< Fences for synchronization with each image in the swap chain. */
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
