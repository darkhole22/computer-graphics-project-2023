#pragma once

#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace computergraphicsproject {

class DebugUtilMessanger
{
public:
	DebugUtilMessanger();
	DebugUtilMessanger(const DebugUtilMessanger& other) = delete;
	DebugUtilMessanger(DebugUtilMessanger&& other) noexcept;
	DebugUtilMessanger(const VkInstance& instance);

	inline const VkDebugUtilsMessengerEXT& getHandle() const { return m_Handle; }

	const DebugUtilMessanger operator=(const DebugUtilMessanger& other) = delete;
	const DebugUtilMessanger& operator=(DebugUtilMessanger&& other) noexcept;

	~DebugUtilMessanger();
private:
	VkDebugUtilsMessengerEXT m_Handle;
	VkInstance m_Instance;
};

class Instance
{
public:
	Instance();
	Instance(const Instance& other) = delete;
	Instance(Instance&& other) noexcept;
	Instance(const std::string& applicationName);

	inline const VkInstance& getHandle() const { return m_Handle; }

	const Instance operator=(const Instance& other) = delete;
	const Instance& operator=(Instance&& other) noexcept;

	~Instance();
private:
	VkInstance m_Handle;
	DebugUtilMessanger* m_DebugMessanger; // The debug messanger must be destroyed before the VkInstance, so manual memory management is required.

	void cleanup() noexcept;
};

class Surface
{
public:
	Surface();
	Surface(const Surface& other) = delete;
	Surface(Surface&& other) noexcept;
	Surface(const Instance& instance, GLFWwindow* window);

	inline const VkSurfaceKHR& getHandle() const { return m_Handle; }
	inline GLFWwindow* getWindow() const { return m_Window; }

	const Surface operator=(const Surface& other) = delete;
	const Surface& operator=(Surface&& other) noexcept;

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
		m_Handle(VK_NULL_HANDLE), m_GraphicsQueueFamilyIndex(0), m_PresentQueueFamilyIndex(0) {}
	inline PhysicalDevice(const PhysicalDevice& other) : 
		m_Handle(other.m_Handle), m_GraphicsQueueFamilyIndex(other.m_GraphicsQueueFamilyIndex), 
		m_PresentQueueFamilyIndex(other.m_PresentQueueFamilyIndex) {}

	static const PhysicalDevice pickDevice(const Instance& instance, const Surface& surface);

	inline const VkPhysicalDevice& getHandle() const { return m_Handle; }
	inline uint32_t getGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
	inline uint32_t getPresentQueueFamilyIndex() const { return m_PresentQueueFamilyIndex; }
	inline VkSampleCountFlagBits getMsaaSamples() const { return m_MsaaSamples; }
	inline const SwapChainSupportDetails& getSwapChainSupportDetails() const { return m_SwapChainSupportDetails; }

	inline const PhysicalDevice& operator=(const PhysicalDevice& other) noexcept
	{
		m_Handle = other.m_Handle;
		m_GraphicsQueueFamilyIndex = other.m_GraphicsQueueFamilyIndex;
		m_PresentQueueFamilyIndex = other.m_PresentQueueFamilyIndex;
		m_MsaaSamples = other.m_MsaaSamples;
		m_SwapChainSupportDetails = other.m_SwapChainSupportDetails;

		return *this;
	}

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
	Device();
	Device(const Device& other) = delete;
	Device(Device&& other) noexcept;
	Device(const PhysicalDevice& physicalDevice);

	const Device operator=(const Device& other) = delete;
	const Device& operator=(Device&& other) noexcept;

	inline VkDevice getHandle() const { return m_Handle; };
	inline const PhysicalDevice& getPhysicalDevice() const { return *m_PhysicalDevice; };
	inline VkQueue getGraphicsQueue() const { return m_GraphicsQueue; }
	inline VkCommandPool getCommandPool() const { return m_CommandPool; }

	inline void waitIdle() const { vkDeviceWaitIdle(m_Handle); }

	~Device();
private:
	VkDevice m_Handle;
	PhysicalDevice const* m_PhysicalDevice;

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

	const Image operator=(const Image& other) = delete;
	const Image& operator=(Image&& other) noexcept;

	~Image();
private:
	VkImage m_Handle;
	VkImageView m_View;
	VkDeviceMemory m_Memory;
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
	SwapChain();
	SwapChain(const SwapChain& other) = delete;
	SwapChain(SwapChain&& other) noexcept;
	SwapChain(const Device& device, const Surface& surface, const RenderPass& renderPass);

	const SwapChain operator=(const SwapChain& other) = delete;
	const SwapChain& operator=(SwapChain&& other) noexcept;

	~SwapChain();
private:
	VkSwapchainKHR m_Handle;
	std::vector<Image> m_Images;
	VkFormat m_ImageFormat;
	VkExtent2D m_Extent;
	Image m_ColorImage;
	Image m_DepthImage;
	std::vector<VkFramebuffer> m_Framebuffers;
	Device const* m_Device;
	Surface const* m_Surface;
	RenderPass const* m_RenderPass;

	void create();
	void recreate();
	void cleanup();
};

}
