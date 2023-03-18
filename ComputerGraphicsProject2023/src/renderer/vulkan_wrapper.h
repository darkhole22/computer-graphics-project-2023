#pragma once

#include <string>
#include <vector>
#include <array>
#include <limits>
#include <cstring>

#include<renderer/Window.h>

#define NO_COPY(CLASS) CLASS(const CLASS& other) = delete; \
						const CLASS operator=(const CLASS& other) = delete;

namespace computergraphicsproject {

const int MAX_FRAMES_IN_FLIGHT = 2;

class DebugUtilMessanger
{
public:
	NO_COPY(DebugUtilMessanger)

	DebugUtilMessanger(const VkInstance& instance);

	inline const VkDebugUtilsMessengerEXT& getHandle() const { return m_Handle; }

	~DebugUtilMessanger();
private:
	VkDebugUtilsMessengerEXT m_Handle;
	VkInstance m_Instance;
};

class Instance
{
public:
	NO_COPY(Instance)

	Instance(const std::string& applicationName);

	inline const VkInstance& getHandle() const { return m_Handle; }

	~Instance();
private:
	VkInstance m_Handle;
	DebugUtilMessanger* m_DebugMessanger; // The debug messanger must be destroyed before the VkInstance, so manual memory management is required.

	void cleanup() noexcept;
};

class Surface
{
public:
	NO_COPY(Surface)

	Surface(const Instance& instance, const Window& window);

	inline const VkSurfaceKHR& getHandle() const { return m_Handle; }
	inline GLFWwindow* getWindow() const { return m_Window; }

	~Surface();
private:
	VkSurfaceKHR m_Handle;
	Instance const* m_Instance;
	GLFWwindow* m_Window;
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class PhysicalDevice
{
public:
	inline PhysicalDevice() :
		m_Handle(VK_NULL_HANDLE), m_GraphicsQueueFamilyIndex(-1),
		m_PresentQueueFamilyIndex(-1), m_SwapChainSupportDetails() {}
	inline PhysicalDevice(const PhysicalDevice& other) : 
		m_Handle(other.m_Handle), m_GraphicsQueueFamilyIndex(other.m_GraphicsQueueFamilyIndex), 
		m_PresentQueueFamilyIndex(other.m_PresentQueueFamilyIndex), m_MsaaSamples(other.m_MsaaSamples),
		m_SwapChainSupportDetails(other.m_SwapChainSupportDetails) {}

	static const PhysicalDevice pickDevice(const Instance& instance, const Surface& surface);

	inline const VkPhysicalDevice& getHandle() const { return m_Handle; }
	inline uint32_t getGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
	inline uint32_t getPresentQueueFamilyIndex() const { return m_PresentQueueFamilyIndex; }
	inline VkSampleCountFlagBits getMsaaSamples() const { return m_MsaaSamples; }
	inline const SwapChainSupportDetails& getSwapChainSupportDetails() const { return m_SwapChainSupportDetails; }

	inline ~PhysicalDevice() {};
private:
	VkPhysicalDevice m_Handle;
	uint32_t m_GraphicsQueueFamilyIndex;
	uint32_t m_PresentQueueFamilyIndex;
	VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
	SwapChainSupportDetails m_SwapChainSupportDetails;
};

class Device
{
public:
	NO_COPY(Device)

	Device(const Instance& instance, const Surface& surface);

	inline VkDevice getHandle() const { return m_Handle; };
	inline const PhysicalDevice& getPhysicalDevice() const { return m_PhysicalDevice; };
	inline VkQueue getGraphicsQueue() const { return m_GraphicsQueue; }
	inline VkCommandPool getCommandPool() const { return m_CommandPool; }

	inline void waitIdle() const { vkDeviceWaitIdle(m_Handle); }

	~Device();
private:
	VkDevice m_Handle;
	PhysicalDevice m_PhysicalDevice;

	VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
	VkQueue m_PresentQueue = VK_NULL_HANDLE;

	VkCommandPool m_CommandPool;

	void cleanup() noexcept;
};

class CommandBuffer
{
public:
	CommandBuffer();
	CommandBuffer(const CommandBuffer& other) = delete;
	CommandBuffer(CommandBuffer&& other) noexcept;
	CommandBuffer(const Device& device, bool singleTime = false);

	template<size_t SIZE>
	static std::array<CommandBuffer, SIZE>&& getCommandBuffers(const Device& device);
	
	const CommandBuffer operator=(const CommandBuffer& other) = delete;
	const CommandBuffer& operator=(CommandBuffer&& other) noexcept;

	inline VkCommandBuffer getHandle() const { return m_Handle; }

	~CommandBuffer();
private:
	VkCommandBuffer m_Handle;
	Device const* m_Device;
	bool m_SingleTime = false;

	void cleanup() noexcept;
};

class Image
{
public:
	Image();
	Image(const Image& other) = delete;
	Image(Image&& other) noexcept;
	Image(VkImage image, const Device& device, VkFormat format);
	Image(const Device& device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags);

	inline VkImage getHandle() const { return m_Handle; }
	inline VkImageView getView() const { return m_View; }

	void transitionLayout(VkImageLayout newLayout, uint32_t mipLevels);

	const Image operator=(const Image& other) = delete;
	const Image& operator=(Image&& other) noexcept;

	~Image();
private:
	VkImage m_Handle;
	VkImageView m_View;
	VkDeviceMemory m_Memory;
	VkImageLayout m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkFormat m_Format;

	Device const* m_Device;

	void cleanup() noexcept;
};

class RenderPass
{
public:
	RenderPass();
	RenderPass(const RenderPass& other) = delete;
	RenderPass(RenderPass&& other) noexcept;
	RenderPass(const Device& device, const Surface& surface);

	inline VkRenderPass getHandle() const { return m_Handle; }

	const RenderPass operator=(const RenderPass& other) = delete;
	const RenderPass& operator=(RenderPass&& other) noexcept;

	~RenderPass();
private:
	VkRenderPass m_Handle;
	Device const* m_Device;
};

class SwapChain
{
public:
	NO_COPY(SwapChain)

	SwapChain(const Device& device, const Surface& surface, const RenderPass& renderPass);

	~SwapChain();
private:
	VkSwapchainKHR m_Handle;
	std::vector<Image> m_Images;
	VkFormat m_ImageFormat;
	VkExtent2D m_Extent;
	Image m_ColorImage;
	Image m_DepthImage;
	std::vector<VkFramebuffer> m_Framebuffers;
	std::array<CommandBuffer, MAX_FRAMES_IN_FLIGHT> m_CommandBuffers;
	std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_ImageAvailableSemaphores;
	std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_RenderFinishedSemaphores;
	std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_InFlightFences;
	Device const* m_Device;
	Surface const* m_Surface;
	RenderPass const* m_RenderPass;

	void create();
	void recreate();
	void cleanup();
};

}

#undef NO_COPY
