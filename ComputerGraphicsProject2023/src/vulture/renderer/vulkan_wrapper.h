#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <cstring>
#include <memory>
#include <filesystem>

#include "Window.h"

#include "vulture/core/Core.h"
#include "vulture/event/Event.h"

namespace vulture {

const int MAX_FRAMES_IN_FLIGHT = 2;

class DebugUtilMessenger
{
public:
	NO_COPY(DebugUtilMessenger)

	explicit DebugUtilMessenger(const VkInstance& instance);

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

	explicit Instance(const std::string& applicationName);

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

	static PhysicalDevice pickDevice(const Instance& instance, const Surface& surface);

	inline const VkPhysicalDevice& getHandle() const { return m_Handle; }
	inline uint32_t getGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
	inline uint32_t getPresentQueueFamilyIndex() const { return m_PresentQueueFamilyIndex; }
	inline VkSampleCountFlagBits getMsaaSamples() const { return m_MsaaSamples; }
	const SwapChainSupportDetails& getSwapChainSupportDetails() const;

	inline ~PhysicalDevice() = default;
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
	explicit CommandBuffer(const Device& device, bool singleTime = false);
	
	CommandBuffer operator=(const CommandBuffer& other) = delete;
	CommandBuffer& operator=(CommandBuffer&& other) noexcept;

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

class Buffer;

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
	inline VkFormat getFormat() const { return m_Format; }

	void transitionLayout(VkImageLayout newLayout, uint32_t mipLevels);
	void copyFromBuffer(const Buffer& buffer);
	void generateMipmaps(uint32_t mipLevels);

	Image operator=(const Image& other) = delete;
	Image& operator=(Image&& other) noexcept;

	~Image();
private:
	VkImage m_Handle;
	VkImageView m_View;
	VkDeviceMemory m_Memory;
	VkImageLayout m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkFormat m_Format;

	int64_t m_Width = -1;
	int64_t m_Height = -1;

	Device const* m_Device;

	void cleanup() noexcept;
};

struct SwapChainRecreatedEvent
{
};

class SwapChain
{
	EVENT(SwapChainRecreatedEvent)

public:
	NO_COPY(SwapChain)

	SwapChain(const Device& device, const Surface& surface, const RenderPass& renderPass);

	inline const VkExtent2D& getExtent() const { return m_Extent; }
	inline uint32_t getImageCount() const { return static_cast<uint32_t>(m_Images.size()); }

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
};

class DescriptorSetLayout
{
public:
	NO_COPY(DescriptorSetLayout)

	explicit DescriptorSetLayout(const Device& device);

	void addBinding(VkDescriptorType type, VkShaderStageFlags target, uint32_t count = 1);
	void create();

	inline VkDescriptorSetLayout getHandle() const { return m_Handle; }
	inline const auto& getBindings() const { return m_Bindings; }

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

class DescriptorPool;

class Buffer
{
public:
	NO_COPY(Buffer)

	Buffer(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

	inline VkBuffer getHandle() const { return m_Handle; }
	inline VkDeviceMemory getMemory() const { return m_Memory; }

	void map(VkDeviceSize size, void* data);

	~Buffer();
private:
	VkBuffer m_Handle;
	VkDeviceMemory m_Memory;
	Device const* m_Device;
};

template <class _Type>
class Uniform
{
public:
	NO_COPY(Uniform)

	Uniform(const Device& device, uint32_t count) :
		m_Device(&device)
	{
		m_Buffers.reserve(count);

		for (size_t i = 0; i < count; i++)
		{
			m_Buffers.emplace_back(m_Device, sizeof(_Type), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}

	inline const std::vector<Buffer>& getBuffers() const { return m_Buffers; }

	inline void map(uint32_t index)
	{
		m_Buffers[index].map(sizeof(_Type), &m_LocalData);
	}

	inline _Type* operator->() noexcept { return &m_LocalData; }

	~Uniform() = default;
private:
	std::vector<Buffer> m_Buffers;
	Device const* m_Device;

	_Type m_LocalData;
};

class Texture
{
public:
	NO_COPY(Texture)

	Texture(const Device& device, const std::string& path);

	inline VkImageView getView() const { return m_Image.getView(); }
	inline VkSampler getSampler() const { return m_Sampler; };
	inline VkImageLayout getLayout() const { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }
	
	~Texture() = default;
private:
	Image m_Image;
	VkSampler m_Sampler;
	uint32_t m_MipLevels;
	Device const* m_Device;
};

class DescriptorWrite
{
public:
	template <class T>
	inline DescriptorWrite(const Uniform<T>& uniform) :
		m_Uniforms(&uniform.getBuffers()), m_UniformSize(sizeof(T)), m_Texture(nullptr) {}
	inline DescriptorWrite(const Texture& texture) :
		m_Uniforms(nullptr), m_UniformSize(0), m_Texture(&texture) {}

	VkWriteDescriptorSet getWriteDescriptorSet(VkDescriptorSet descriptorSet, uint32_t index, uint32_t binding) const;

	~DescriptorWrite() = default;
private:
	std::vector<Buffer> const* m_Uniforms;
	size_t m_UniformSize;
	Texture const* m_Texture;
};

class DescriptorSet
{
public:
	NO_COPY(DescriptorSet)

	inline const DescriptorSetLayout& getLayout() const { return *m_Layout; }

	~DescriptorSet();
	friend class DescriptorPool;
private:
	DescriptorSet(const DescriptorPool& pool, const DescriptorSetLayout& layout, const std::vector<DescriptorWrite>& descriptorWrites);
	
	void create();
	void cleanup();
	void recreate();

	std::vector<VkDescriptorSet> m_Handles;
	DescriptorPool const* m_Pool;
	DescriptorSetLayout const* m_Layout;
	std::vector<DescriptorWrite> m_DescriptorWrites;
};

/*
*/
class DescriptorPool
{
public:
	NO_COPY(DescriptorPool)

	/*
	* Constructor of a DescriptorPool
	* 
	* @param device - The logical device on witch the pool will be allocated.
	* @param frameCount - The number of framebuffers.
	*/
	DescriptorPool(const Device& device, uint32_t frameCount);

	inline uint32_t getFrameCount() const { return m_FrameCount; }
	void setFrameCount(uint32_t frameCount);

	inline VkDescriptorPool getHandle() const { return m_Handle; }

	inline const Device& getDevice() const { return *m_Device; }

	std::weak_ptr<DescriptorSet> getDescriptorSet(const DescriptorSetLayout& layout, const std::vector<DescriptorWrite>& descriptorWrites);
	void freeDescriptorSet(std::weak_ptr<DescriptorSet> descriptorSet);

	~DescriptorPool();

	struct DescriptorTypePoolInfo
	{
		uint32_t size;
		uint32_t count;
	};
private:
	VkDescriptorPool m_Handle = VK_NULL_HANDLE;
	Device const* m_Device;
	uint32_t m_FrameCount;
	uint32_t m_Size = 0;
	std::unordered_map<VkDescriptorType, DescriptorTypePoolInfo> m_TypeInfos;
	std::unordered_set<std::shared_ptr<DescriptorSet>> m_Sets;

	void cleanup();
	void recreate();
};

class Pipeline
{
public:
	NO_COPY(Pipeline)

	Pipeline(const RenderPass& renderPass, const std::string& vertexShader, const std::string& fragmentShader, 
		const std::vector<DescriptorSetLayout>& descriptorSetLayouts, const VertexLayout& vertexLayout);

	inline VkPipeline getHandle() const { return m_Handle; }

	~Pipeline();
private:
	VkPipeline m_Handle;
	VkPipelineLayout m_Layout;
	Device const* m_Device;
};

} // namespace vulture
