#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

class DebugUtilMessenger
{
public:
	NO_COPY(DebugUtilMessenger)

	DebugUtilMessenger(const VkInstance& instance);

	inline const VkDebugUtilsMessengerEXT& getHandle() const { return m_Handle; }

	~DebugUtilMessenger();
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
	DebugUtilMessenger* m_DebugMessenger; // The debug messenger must be destroyed before the VkInstance, so manual memory management is required.

	void cleanup() noexcept;
};

class Surface
{
public:
	NO_COPY(Surface)

	Surface(const Instance& instance, const Window& window);

	inline const VkSurfaceKHR& getHandle() const { return m_Handle; }
	inline const Window& getWindow() const { return *m_Window; }

	~Surface();
private:
	VkSurfaceKHR m_Handle;
	Instance const* m_Instance;
	Window const* m_Window;
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
		m_Handle(VK_NULL_HANDLE), m_Surface(nullptr),
		m_GraphicsQueueFamilyIndex(-1),	m_PresentQueueFamilyIndex(-1),
		m_SwapChainSupportDetails() {}

	static const PhysicalDevice pickDevice(const Instance& instance, const Surface& surface);

	inline const VkPhysicalDevice& getHandle() const { return m_Handle; }
	inline uint32_t getGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
	inline uint32_t getPresentQueueFamilyIndex() const { return m_PresentQueueFamilyIndex; }
	inline VkSampleCountFlagBits getMsaaSamples() const { return m_MsaaSamples; }
	const SwapChainSupportDetails& getSwapChainSupportDetails() const;

	inline ~PhysicalDevice() {};
private:
	VkPhysicalDevice m_Handle;
	Surface const* m_Surface;

	uint32_t m_GraphicsQueueFamilyIndex;
	uint32_t m_PresentQueueFamilyIndex;
	VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
	mutable SwapChainSupportDetails m_SwapChainSupportDetails;
};

class Device
{
public:
	NO_COPY(Device)

	Device(const Instance& instance, const Surface& surface);

	inline VkDevice getHandle() const { return m_Handle; };
	inline const PhysicalDevice& getPhysicalDevice() const { return m_PhysicalDevice; };
	inline VkQueue getGraphicsQueue() const { return m_GraphicsQueue; }
	inline VkQueue getPresentQueue() const { return m_PresentQueue; }
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

class RenderPass
{
public:
	NO_COPY(RenderPass)

	RenderPass(const Device& device, const Surface& surface);

	inline VkRenderPass getHandle() const { return m_Handle; }
	inline const Device& getDevice() const { return *m_Device; }

	inline const std::vector<VkClearValue>& getClearValues() const { return m_ClearValues; }

	~RenderPass();
private:
	VkRenderPass m_Handle;
	std::vector<VkClearValue> m_ClearValues;
	Device const* m_Device;

};

class Pipeline;
class SwapChain;

class CommandBuffer
{
public:

	static std::vector<CommandBuffer> getCommandBuffers(const Device& device, size_t count);

	CommandBuffer();
	CommandBuffer(const CommandBuffer& other) = delete;
	CommandBuffer(CommandBuffer&& other) noexcept;
	CommandBuffer(const Device& device, bool singleTime = false);
	
	const CommandBuffer operator=(const CommandBuffer& other) = delete;
	const CommandBuffer& operator=(CommandBuffer&& other) noexcept;

	inline VkCommandBuffer getHandle() const { return m_Handle; }

	inline void reset() { vkResetCommandBuffer(m_Handle, 0); }
	void begin();
	void beginRenderPass(const RenderPass& renderPass, VkFramebuffer frameBuffer, VkExtent2D extent);
	void bindPipeline(const Pipeline& pipeline, const SwapChain& swapChain);
	void endRenderPass();
	void end();

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

class SwapChain
{
public:
	NO_COPY(SwapChain)

	SwapChain(const Device& device, const Surface& surface, const RenderPass& renderPass);

	inline const VkExtent2D& getExtent() const { return m_Extent; }
	inline uint32_t getImageCount() const { return static_cast<uint32_t>(m_Images.size()); }

	// TODO replace with a proper event system
	bool wasRecreated() const { 
		return m_Modified__tmp || m_Modified2__tmp;
	}

	~SwapChain();

	friend class RenderTarget;
private:
	VkSwapchainKHR m_Handle;
	std::vector<Image> m_Images;
	VkFormat m_ImageFormat;
	VkExtent2D m_Extent;
	Image m_ColorImage;
	Image m_DepthImage;
	std::vector<VkFramebuffer> m_Framebuffers;
	std::vector<CommandBuffer> m_CommandBuffers;
	std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_ImageAvailableSemaphores;
	std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> m_RenderFinishedSemaphores;
	std::array<VkFence, MAX_FRAMES_IN_FLIGHT> m_InFlightFences;
	std::vector<VkFence> m_ImageInFlightFences;
	Device const* m_Device;
	Surface const* m_Surface;
	RenderPass const* m_RenderPass;

	void create();
	void recreate();
	void onRecreateCleanup();
	void cleanup();

	uint32_t getImageIndex(uint32_t currentFrame);
	CommandBuffer& getCommandBuffer(uint32_t currentFrame);
	void submit(uint32_t currentFrame, uint32_t imageIndex);

	// TODO replace with a proper event system
	bool m_Modified__tmp = false;
	bool m_Modified2__tmp = false;
};

class DescriptorSetLayout
{
public:
	NO_COPY(DescriptorSetLayout)

	DescriptorSetLayout(const Device& device);

	void addBinding(VkDescriptorType type, VkShaderStageFlags target, uint32_t count = 1);
	void create();

	inline VkDescriptorSetLayout getHandle() const { return m_Handle; }

	~DescriptorSetLayout();
private:
	VkDescriptorSetLayout m_Handle;
	std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
	Device const* m_Device;
};

class VertexLayout
{
public:
	NO_COPY(VertexLayout)

	VertexLayout(uint32_t size, const std::vector<std::pair<VkFormat, uint32_t>>& descriptors);

	inline VkVertexInputBindingDescription getBinding() const { return m_Bindings; }
	inline const std::vector<VkVertexInputAttributeDescription>& getAttributes() const { return m_Attributes; }
private:
	VkVertexInputBindingDescription m_Bindings;
	std::vector<VkVertexInputAttributeDescription> m_Attributes;
};

class Shader
{
public:
	NO_COPY(Shader)

	Shader(const Device& device, const std::string& name);

	VkPipelineShaderStageCreateInfo getStage(VkShaderStageFlagBits stageType, const char* mainName = "main") const;

	~Shader();
private:
	VkShaderModule m_Handle;
	Device const* m_Device;
};

class Pipeline
{
public:
	NO_COPY(Pipeline)

	Pipeline(const RenderPass& renderPass, const std::string& vertexShader, const std::string& fragmentShader, 
		const DescriptorSetLayout& descriptorSetLayout, const VertexLayout& vertexLayout);

	inline VkPipeline getHandle() const { return m_Handle; }

	~Pipeline();
private:
	VkPipeline m_Handle;
	VkPipelineLayout m_Layout;
	Device const* m_Device;
};

} // namespace computergraphicsproject

#undef NO_COPY
