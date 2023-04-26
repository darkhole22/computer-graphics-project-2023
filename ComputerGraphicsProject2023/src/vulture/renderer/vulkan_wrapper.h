#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <cstring>
#include <memory>
#include <filesystem>
#include <optional>

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
class DescriptorSet;
class Buffer;

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
	void bindDescriptorSet(const Pipeline& pipeline, VkDescriptorSet descriptorSet, uint32_t set);
	void bindVertexBuffer(const Buffer& buffer);
	void bindIndexBuffer(const Buffer& buffer);
	void drawIndexed(uint32_t indexCount);
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
	Buffer();
	Buffer(const Buffer& other) = delete;
	Buffer(Buffer&& other) noexcept;

	Buffer(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	Buffer(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, void* data);

	inline VkBuffer getHandle() const { return m_Handle; }
	inline VkDeviceMemory getMemory() const { return m_Memory; }
	inline VkDeviceSize getSize() const { return m_Size; }

	void map(void* data) const;
	void map() const;
	void copyToBuffer(VkDeviceSize size, const Buffer& destination);

	Buffer& operator=(const Buffer& other) = delete;
	Buffer& operator=(Buffer&& other) noexcept;

	bool operator==(const Buffer& other)
	{
		return other.m_Handle == m_Handle;
	}

	~Buffer();
private:
	VkBuffer m_Handle;
	VkDeviceMemory m_Memory;
	Device const* m_Device;
	VkDeviceSize m_Size;
	void* m_Data;

	void cleanup() noexcept;
};

template <class _Type>
class Uniform
{
public:
	Uniform() = default;
	Uniform(const Uniform& other) = delete;
	Uniform(Uniform&& other)
	{
		m_Buffers = std::move(other.m_Buffers);
		m_Device = other.m_Device;
		m_LocalData = other.m_LocalData;

		other.m_Device = nullptr;
		other.m_LocalData = nullptr;
	}

	Uniform(const Device& device, uint32_t count) :
		m_Device(&device)
	{
		m_LocalData = new _Type();
		m_Buffers.reserve(count);

		for (size_t i = 0; i < count; i++)
		{
			m_Buffers.emplace_back(*m_Device,
				sizeof(_Type),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_LocalData);
		}
	}

	inline const std::vector<Buffer>* getBuffers() const { return &m_Buffers; }

	inline _Type* operator->() noexcept { return m_LocalData; }
	inline const _Type* operator->() const noexcept { return m_LocalData; }

	Uniform& operator=(const Uniform& other) = delete;
	Uniform& operator=(Uniform&& other) noexcept {
		if (m_Buffers.size() != other.m_Buffers.size())
		{
			delete m_LocalData;

			m_Buffers = std::move(other.m_Buffers);
			m_Device = other.m_Device;
			m_LocalData = other.m_LocalData;

			other.m_Device = nullptr;
			other.m_LocalData = nullptr;
		}
		return *this;
	}

	~Uniform()
	{
		delete m_LocalData;
	}
private:
	std::vector<Buffer> m_Buffers;
	Device const* m_Device = nullptr;

	_Type * m_LocalData = nullptr;
};

class Texture
{
public:
	NO_COPY(Texture)

	Texture(const Device& device, const std::string& path);

	inline VkImageView getView() const { return m_Image.getView(); }
	inline VkSampler getSampler() const { return m_Sampler; };
	inline VkImageLayout getLayout() const { return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; }
	
	~Texture();
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
	inline DescriptorWrite(const Uniform<T>& uniform)
	{
		m_UniformBuffers = uniform.getBuffers();
		
		m_UniformInfos.reserve(m_UniformBuffers->size());
		for (auto& uniform : (*m_UniformBuffers))
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniform.getHandle();
			bufferInfo.offset = 0;
			bufferInfo.range = uniform.getSize();
			m_UniformInfos.push_back(bufferInfo);
		}
	}

	inline DescriptorWrite(const Texture& texture) :
		m_UniformBuffers(nullptr)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = texture.getLayout();
		imageInfo.imageView = texture.getView();
		imageInfo.sampler = texture.getSampler();

		m_TextureInfo = imageInfo;
	}

	VkWriteDescriptorSet getWriteDescriptorSet(VkDescriptorSet descriptorSet, uint32_t index, uint32_t binding) const;

	void map(uint32_t index) const;

	~DescriptorWrite() = default;
private:
	std::vector<Buffer> const* m_UniformBuffers;
	std::vector<VkDescriptorBufferInfo> m_UniformInfos;
	std::optional<VkDescriptorImageInfo> m_TextureInfo;
};

class DescriptorSet
{
public:
	NO_COPY(DescriptorSet)

	inline const DescriptorSetLayout& getLayout() const { return *m_Layout; }

	inline VkDescriptorSet getHandle(uint32_t index) const { return m_Handles[index]; }

	void map(uint32_t index) const;

	~DescriptorSet();
	friend class DescriptorPool;
private:
	DescriptorSet(DescriptorPool& pool, const DescriptorSetLayout& layout, const std::vector<DescriptorWrite>& descriptorWrites);
	
	void create();
	void cleanup();
	void recreate(bool clean = true);

	std::vector<VkDescriptorSet> m_Handles;
	DescriptorPool* m_Pool;
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
	* @brief Constructor of a DescriptorPool
	* 
	* @param device - The logical device on witch the pool will be allocated.
	* @param frameCount - The number of framebuffers.
	*/
	DescriptorPool(const Device& device, uint32_t frameCount);

	inline uint32_t getFrameCount() const { return m_FrameCount; }
	void setFrameCount(uint32_t frameCount);

	inline VkDescriptorPool getHandle() const { return m_Handle; }

	inline const Device& getDevice() const { return *m_Device; }

	void reserveSpace(uint32_t count, const DescriptorSetLayout& layout);

	Ref<DescriptorSet> getDescriptorSet(const DescriptorSetLayout& layout, const std::vector<DescriptorWrite>& descriptorWrites);

	~DescriptorPool();

	struct DescriptorTypePoolInfo
	{
		uint32_t size;
		uint32_t count;
	};

	friend class DescriptorSet;
private:
	VkDescriptorPool m_Handle = VK_NULL_HANDLE;
	Device const* m_Device;
	uint32_t m_FrameCount;
	uint32_t m_Size = 0;
	std::unordered_map<VkDescriptorType, DescriptorTypePoolInfo> m_TypeInfos;
	std::unordered_set<Ref<DescriptorSet>> m_Sets;

	void cleanupDescriptorSet(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	void cleanup();
	void recreate();
};

struct PipelineAdvancedConfig
{
	VkCompareOp compareOperator = VK_COMPARE_OP_LESS;
	bool useAlpha = false;

	static const PipelineAdvancedConfig defaultConfig;
};

class Pipeline
{
public:
	NO_COPY(Pipeline)

	Pipeline(const RenderPass& renderPass, const std::string& vertexShader, const std::string& fragmentShader, 
		const std::vector<DescriptorSetLayout*>& descriptorSetLayouts, const VertexLayout& vertexLayout, const PipelineAdvancedConfig& config = PipelineAdvancedConfig::defaultConfig);

	inline VkPipeline getHandle() const { return m_Handle; }
	inline VkPipelineLayout getLayout() const { return m_Layout; }

	~Pipeline();
private:
	VkPipeline m_Handle;
	VkPipelineLayout m_Layout;
	Device const* m_Device;
};

} // namespace vulture
