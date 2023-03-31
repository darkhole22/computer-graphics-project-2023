#include "vulkan_wrapper.h"

#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <optional>
#include <set>
#include <algorithm>
#include <fstream>


#ifndef NDEBUG
// DEBUG
#define VALIDATION_LAYER true
#define VALIDATION_LAYER_IF(x) x
#else
// RELEASE
#define VALIDATION_LAYER false
#define VALIDATION_LAYER_IF(x)
#endif // !NDEBUG

#define ASSERT_VK_SUCCESS(func, message) if (func != VK_SUCCESS) { throw std::runtime_error(message); }

namespace vulture {

static const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

bool checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers) {
			if (std::strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) return false;
	}

	return true;
}

bool checkIfItHasExtension(const char* ext) {
	uint32_t extCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);

	std::vector<VkExtensionProperties> availableExt(extCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount,
		availableExt.data());

	for (const auto& extProp : availableExt) {
		if (std::strcmp(ext, extProp.extensionName) == 0) {
			return true;
		}
	}

	return false;
}

std::vector<const char*> getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	VALIDATION_LAYER_IF(extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));

	if (checkIfItHasExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
		extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	}
	if (checkIfItHasExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
		extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
	}

	return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		// VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		if (std::strcmp(pCallbackData->pMessageIdName, "Loader Message") == 0) // Skip message caused by other application
		{
			return VK_FALSE;
		}
		std::cerr << "[Validation layer]: [ID]: " << pCallbackData->pMessageIdName << "\n\t[message]: " <<
			pCallbackData->pMessage << std::endl;
	}
	return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = 
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
}

Instance::Instance(const std::string& applicationName)
{
	VALIDATION_LAYER_IF(
		if (!checkValidationLayerSupport()) {
			throw std::runtime_error("Validation layers requested, but not available!");
		}
	)
	
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = applicationName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;
	VALIDATION_LAYER_IF(
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	)

	ASSERT_VK_SUCCESS(vkCreateInstance(&createInfo, nullptr, &m_Handle), "Failed to create instance!");

	m_DebugMessenger = new DebugUtilMessenger(m_Handle);
}

Instance::~Instance()
{
	cleanup();
}

void Instance::cleanup() noexcept
{
	delete m_DebugMessenger;

	if (m_Handle != VK_NULL_HANDLE)
		vkDestroyInstance(m_Handle, nullptr);
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

DebugUtilMessenger::DebugUtilMessenger(const VkInstance& instance)
	: m_Instance(instance)
{
	m_Handle = VK_NULL_HANDLE;
	VALIDATION_LAYER_IF(
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(createInfo);

		ASSERT_VK_SUCCESS(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &m_Handle), "Failed to set up debug messenger!")
	)
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

DebugUtilMessenger::~DebugUtilMessenger()
{
	VALIDATION_LAYER_IF(
		if (m_Instance != VK_NULL_HANDLE && m_Handle != VK_NULL_HANDLE)
		{
			DestroyDebugUtilsMessengerEXT(m_Instance, m_Handle, nullptr);
		}
	)
}

Surface::Surface(const Instance& instance, const Window& window) :
	m_Handle(VK_NULL_HANDLE), m_Instance(&instance), m_Window(&window)
{
	ASSERT_VK_SUCCESS(glfwCreateWindowSurface(instance.getHandle(), m_Window->getHandle(), nullptr, &m_Handle), "Failed to create window surface!")
}

Surface::~Surface()
{
	if (m_Instance && m_Handle != VK_NULL_HANDLE)
		vkDestroySurfaceKHR(m_Instance->getHandle(), m_Handle, nullptr);
}

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() const {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, const Surface& surface) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.getHandle(), &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}

static const std::vector<const char*> deviceRequiredExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static const std::vector<const char*> deviceRequiredIfPresentExtensions = {
	"VK_KHR_portability_subset"
};

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, const Surface& surface) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.getHandle(), &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getHandle(), &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getHandle(), &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getHandle(), &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getHandle(), &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool isDeviceSuitable(VkPhysicalDevice device, const Surface& surface) {
	// VkPhysicalDeviceProperties deviceProperties;
	// vkGetPhysicalDeviceProperties(device, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indices = findQueueFamilies(device, surface);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate && deviceFeatures.samplerAnisotropy;
}

VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice) {
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

PhysicalDevice PhysicalDevice::pickDevice(const Instance& instance, const Surface& surface)
{
	PhysicalDevice physicalDevice;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance.getHandle(), &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance.getHandle(), &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device, surface)) {
			QueueFamilyIndices indices = findQueueFamilies(device, surface);

			physicalDevice.m_Handle = device;
			physicalDevice.m_Surface = &surface;
			physicalDevice.m_GraphicsQueueFamilyIndex = indices.graphicsFamily.value();
			physicalDevice.m_PresentQueueFamilyIndex = indices.presentFamily.value();
			physicalDevice.m_MsaaSamples = getMaxUsableSampleCount(device);
			physicalDevice.m_SwapChainSupportDetails = querySwapChainSupport(device, surface);

			break;
		}
	}

	if (physicalDevice.m_Handle == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find a suitable GPU!");
	}

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice.m_Handle, &deviceProperties);
	std::cout << "[Device name]: " << deviceProperties.deviceName << std::endl;

	return physicalDevice;
}

const SwapChainSupportDetails& PhysicalDevice::getSwapChainSupportDetails() const
{
	m_SwapChainSupportDetails = querySwapChainSupport(m_Handle, *m_Surface);
	return m_SwapChainSupportDetails;
}

Device::Device(const Instance& instance, const Surface& surface) :
	m_PhysicalDevice(PhysicalDevice::pickDevice(instance, surface))
{
	uint32_t queueGraphicsFamily = m_PhysicalDevice.getGraphicsQueueFamilyIndex();
	uint32_t queuePresentFamily = m_PhysicalDevice.getPresentQueueFamilyIndex();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { queueGraphicsFamily, queuePresentFamily };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.sampleRateShading = VK_TRUE;

	std::vector<const char*> deviceExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.getHandle(), nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.getHandle(), nullptr, &extensionCount, availableExtensions.data());

	for (auto ext : deviceRequiredIfPresentExtensions)
	{
		for (const auto& extProp : availableExtensions) {
			if (std::strcmp(ext, extProp.extensionName) == 0) {
				deviceExtensions.push_back(ext);
				break;
			}
		}
	}

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	createInfo.enabledLayerCount = 0;
	VALIDATION_LAYER_IF(
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	);

	ASSERT_VK_SUCCESS(vkCreateDevice(m_PhysicalDevice.getHandle(), &createInfo, nullptr, &m_Handle), "Failed to create logical device!")

	vkGetDeviceQueue(m_Handle, queueGraphicsFamily, 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_Handle, queuePresentFamily, 0, &m_PresentQueue);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueGraphicsFamily;

	ASSERT_VK_SUCCESS(vkCreateCommandPool(m_Handle, &poolInfo, nullptr, &m_CommandPool), "Failed to create command pool!");
}

Device::~Device()
{
	cleanup();
}

void Device::cleanup() noexcept
{
	if (m_Handle != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(m_Handle, m_CommandPool, nullptr);

		vkDestroyDevice(m_Handle, nullptr);
	}
}

CommandBuffer::CommandBuffer() :
	m_Handle(VK_NULL_HANDLE), m_Device(nullptr)
{
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
{
	m_Handle = other.m_Handle;
	m_Device = other.m_Device;

	other.m_Handle = VK_NULL_HANDLE;
	other.m_Device = nullptr;
}

CommandBuffer::CommandBuffer(const Device& device, bool singleTime)
{
	m_SingleTime = singleTime;
	m_Device = &device;

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_Device->getCommandPool();
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(m_Device->getHandle(), &allocInfo, &m_Handle);

	if (m_SingleTime)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(m_Handle, &beginInfo);
	}
}

std::vector<CommandBuffer> CommandBuffer::getCommandBuffers(const Device& device, size_t count)
{
	std::vector<CommandBuffer> buffers(count);
	std::vector<VkCommandBuffer> handles(count);
	
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(count);

	ASSERT_VK_SUCCESS(vkAllocateCommandBuffers(device.getHandle(), &allocInfo, handles.data()), "Failed to allocate command buffers!");

	for (size_t i = 0; i < buffers.size(); i++)
	{
		buffers[i].m_Handle = handles[i];
		buffers[i].m_Device = &device;
	}

	return buffers;
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept
{
	if (m_Handle != other.m_Handle)
	{
		cleanup();

		m_Handle = other.m_Handle;
		m_Device = other.m_Device;

		other.m_Handle = VK_NULL_HANDLE;
		other.m_Device = nullptr;
	}

	return *this;
}

void CommandBuffer::begin()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	ASSERT_VK_SUCCESS(vkBeginCommandBuffer(m_Handle, &beginInfo), "Failed to begin recording command buffer!");
}

void CommandBuffer::beginRenderPass(const RenderPass& renderPass, VkFramebuffer frameBuffer, VkExtent2D extent)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass.getHandle();
	renderPassInfo.framebuffer = frameBuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;

	auto& clearValues = renderPass.getClearValues();

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_Handle, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::bindPipeline(const Pipeline& pipeline, const SwapChain& swapChain)
{
	vkCmdBindPipeline(m_Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getHandle());
	
	auto& extent = swapChain.getExtent();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_Handle, 0, 1, &viewport);
	
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor(m_Handle, 0, 1, &scissor);
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

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_Handle;

			vkQueueSubmit(m_Device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(m_Device->getGraphicsQueue());
		
			vkFreeCommandBuffers(m_Device->getHandle(), m_Device->getCommandPool(), 1, &m_Handle);
		}
		else
		{
			vkQueueWaitIdle(m_Device->getGraphicsQueue());
			vkFreeCommandBuffers(m_Device->getHandle(), m_Device->getCommandPool(), 1, &m_Handle);
		} 
	}
}

Image::Image() :
	m_Handle(VK_NULL_HANDLE), m_Memory(VK_NULL_HANDLE), m_View(VK_NULL_HANDLE), m_Device(nullptr), m_Format(VK_FORMAT_UNDEFINED)
{
}

Image::Image(Image&& other) noexcept
{
	m_Handle = other.m_Handle;
	m_Memory = other.m_Memory;
	m_View = other.m_View;
	m_Device = other.m_Device;
	m_Layout = other.m_Layout;
	m_Format = other.m_Format;

	other.m_Handle = VK_NULL_HANDLE;
	other.m_Memory = VK_NULL_HANDLE;
	other.m_View = VK_NULL_HANDLE;
	other.m_Device = nullptr;
}

VkImageView createImageView(VkImage image, const Device& device, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageView view;

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.pNext = nullptr;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	ASSERT_VK_SUCCESS(vkCreateImageView(device.getHandle(), &viewInfo, nullptr, &view), "Failed to create texture image view!");

	return view;
}

Image::Image(VkImage image, const Device& device, VkFormat format) :
	m_Memory(VK_NULL_HANDLE), m_View(VK_NULL_HANDLE), m_Format(format)
{
	m_Handle = image;
	m_Device = &device;
	m_View = createImageView(image, device, format, VK_IMAGE_ASPECT_COLOR_BIT);
}

uint32_t findMemoryType(const PhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice.getHandle(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type!");
}

Image::Image(const Device& device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags) :
	m_Format(format)
{
	m_Device = &device;

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

	ASSERT_VK_SUCCESS(vkCreateImage(m_Device->getHandle(), &imageInfo, nullptr, &m_Handle), "Failed to create image!");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_Device->getHandle(), m_Handle, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(m_Device->getPhysicalDevice(), memRequirements.memoryTypeBits, properties);

	ASSERT_VK_SUCCESS(vkAllocateMemory(m_Device->getHandle(), &allocInfo, nullptr, &m_Memory), "Failed to allocate image memory!");

	vkBindImageMemory(m_Device->getHandle(), m_Handle, m_Memory, 0);

	m_View = createImageView(m_Handle, device, format, aspectFlags);
}

inline bool hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Image::transitionLayout(VkImageLayout newLayout, uint32_t mipLevels)
{
	CommandBuffer commandBuffer(*m_Device, true);

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = m_Layout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = m_Handle;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(m_Format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1; // TODO add layer parameter

	if (m_Layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (m_Layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (m_Layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("Unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer.getHandle(),
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	m_Layout = newLayout;
}

Image& Image::operator=(Image&& other) noexcept
{
	if (m_Handle != other.m_Handle)
	{
		cleanup();

		m_Handle = other.m_Handle;
		m_Memory = other.m_Memory;
		m_View = other.m_View;
		m_Device = other.m_Device;
		m_Layout = other.m_Layout;
		m_Format = other.m_Format;

		other.m_Handle = VK_NULL_HANDLE;
		other.m_Memory = VK_NULL_HANDLE;
		other.m_View = VK_NULL_HANDLE;
		other.m_Device = nullptr;
	}

	return *this;
}

Image::~Image()
{
	cleanup();
}

void Image::cleanup() noexcept
{
	if (m_Handle == VK_NULL_HANDLE) return;

	vkDestroyImageView(m_Device->getHandle(), m_View, nullptr);
	if (m_Memory != VK_NULL_HANDLE)
	{
		vkDestroyImage(m_Device->getHandle(), m_Handle, nullptr);
		vkFreeMemory(m_Device->getHandle(), m_Memory, nullptr);
	}
}

VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("Failed to find supported format!");
}

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice) {
	return findSupportedFormat(physicalDevice,
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT
	);
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

RenderPass::RenderPass(const Device& device, const Surface& surface) :
	m_Device(&device)
{
	const SwapChainSupportDetails swapChainSupport = device.getPhysicalDevice().getSwapChainSupportDetails();

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkFormat swapChainImageFormat = surfaceFormat.format;

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = device.getPhysicalDevice().getMsaaSamples();
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat(device.getPhysicalDevice().getHandle());
	depthAttachment.samples = device.getPhysicalDevice().getMsaaSamples();
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = swapChainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcAccessMask = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	ASSERT_VK_SUCCESS(vkCreateRenderPass(device.getHandle(), &renderPassInfo, nullptr, &m_Handle), "Failed to create render pass!");
	
	VkClearValue cc{};
	VkClearValue cd{};
	cc.color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	cd.depthStencil = { 1.0f, 0 };

	m_ClearValues.reserve(2);

	m_ClearValues.push_back(cc);
	m_ClearValues.push_back(cd);
}

RenderPass::~RenderPass()
{
	if (m_Handle != VK_NULL_HANDLE)
		vkDestroyRenderPass(m_Device->getHandle(), m_Handle, nullptr);
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		auto [width, height] = window.getFramebufferSize();

		VkExtent2D actualExtent = {
			std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};

		return actualExtent;
	}
}

SwapChain::SwapChain(const Device& device, const Surface& surface, const RenderPass& renderPass) :
	m_Device(&device), m_Surface(&surface), m_RenderPass(&renderPass)
{
	create();

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		ASSERT_VK_SUCCESS(vkCreateSemaphore(m_Device->getHandle(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]), "Failed to create image semaphores!");
		ASSERT_VK_SUCCESS(vkCreateSemaphore(m_Device->getHandle(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]), "Failed to create render semaphores!");
		ASSERT_VK_SUCCESS(vkCreateFence(m_Device->getHandle(), &fenceInfo, nullptr, &m_InFlightFences[i]), "Failed to create Fences!");
	}
}

void SwapChain::create()
{
	const PhysicalDevice& physicalDevice = m_Device->getPhysicalDevice();
	const SwapChainSupportDetails& swapChainSupport = physicalDevice.getSwapChainSupportDetails();

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	m_Extent = chooseSwapExtent(swapChainSupport.capabilities, m_Surface->getWindow());

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface->getHandle();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = m_Extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { physicalDevice.getGraphicsQueueFamilyIndex(), physicalDevice.getPresentQueueFamilyIndex() };

	if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	ASSERT_VK_SUCCESS(vkCreateSwapchainKHR(m_Device->getHandle(), &createInfo, nullptr, &m_Handle), "Failed to create swap chain!")

	std::vector<VkImage> vkImages;

	vkGetSwapchainImagesKHR(m_Device->getHandle(), m_Handle, &imageCount, nullptr);
	vkImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_Device->getHandle(), m_Handle, &imageCount, vkImages.data());

	m_ImageFormat = surfaceFormat.format;

	m_Images.reserve(imageCount);
	for (size_t i = 0; i < imageCount; i++)
	{
		m_Images.emplace_back(vkImages[i], *m_Device, m_ImageFormat);
	}

	m_ColorImage = Image(*m_Device, m_Extent.width, m_Extent.height, 1, m_Device->getPhysicalDevice().getMsaaSamples(),
		m_ImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	
	VkFormat depthFormat = findDepthFormat(m_Device->getPhysicalDevice().getHandle());

	m_DepthImage = Image(*m_Device, m_Extent.width, m_Extent.height, 1, m_Device->getPhysicalDevice().getMsaaSamples(),
		depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	m_DepthImage.transitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

	m_Framebuffers.resize(imageCount);

	for (size_t i = 0; i < imageCount; i++) {
		std::array<VkImageView, 3> attachments = {
			m_ColorImage.getView(),
			m_DepthImage.getView(),
			m_Images[i].getView()
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass->getHandle();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_Extent.width;
		framebufferInfo.height = m_Extent.height;
		framebufferInfo.layers = 1;

		ASSERT_VK_SUCCESS(vkCreateFramebuffer(m_Device->getHandle(), &framebufferInfo, nullptr, &m_Framebuffers[i]), "Failed to create framebuffer!");
	}

	m_ImageInFlightFences.clear();
	m_ImageInFlightFences.resize(imageCount, VK_NULL_HANDLE);

	m_CommandBuffers = CommandBuffer::getCommandBuffers(*m_Device, imageCount);
}

void SwapChain::recreate()
{
	m_Device->waitIdle();

	onRecreateCleanup();
	create();

	emit(SwapChainRecreatedEvent{});
}

uint32_t SwapChain::getImageIndex(uint32_t currentFrame)
{
	uint32_t imageIndex;
	VkResult result;
	do
	{
		vkWaitForFences(m_Device->getHandle(), 1, &m_InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		result = vkAcquireNextImageKHR(m_Device->getHandle(), m_Handle, UINT64_MAX, m_ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreate();
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

	} while (result == VK_ERROR_OUT_OF_DATE_KHR);

	if (m_ImageInFlightFences[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(m_Device->getHandle(), 1, &m_ImageInFlightFences[imageIndex], VK_TRUE, UINT64_MAX);
	}
	m_ImageInFlightFences[imageIndex] = m_InFlightFences[currentFrame];

	return imageIndex;
}

void SwapChain::submit(uint32_t currentFrame, uint32_t imageIndex)
{
	vkResetFences(m_Device->getHandle(), 1, &m_InFlightFences[currentFrame]);
	
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

	if (vkQueueSubmit(m_Device->getGraphicsQueue(), 1, &submitInfo, m_InFlightFences[currentFrame]) != VK_SUCCESS) {
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

	VkResult result = vkQueuePresentKHR(m_Device->getPresentQueue(), &presentInfo);

	const Window& window = m_Surface->getWindow();

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.getFrameBufferResized()) {
		recreate();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swap chain image!");
	}
}

CommandBuffer& SwapChain::getCommandBuffer(uint32_t currentFrame)
{
	return m_CommandBuffers[currentFrame];
}

void SwapChain::onRecreateCleanup()
{
	m_Images.clear();

	for (auto & m_Framebuffer : m_Framebuffers) {
		vkDestroyFramebuffer(m_Device->getHandle(), m_Framebuffer, nullptr);
	}

	vkDestroySwapchainKHR(m_Device->getHandle(), m_Handle, nullptr);
}

void SwapChain::cleanup()
{
	if (m_Handle == VK_NULL_HANDLE) return;

	onRecreateCleanup();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(m_Device->getHandle(), m_ImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(m_Device->getHandle(), m_RenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(m_Device->getHandle(), m_InFlightFences[i], nullptr);
	}

	m_Handle = VK_NULL_HANDLE;
}

SwapChain::~SwapChain()
{
	cleanup();
}

VertexLayout::VertexLayout(uint32_t size, const std::vector<std::pair<VkFormat, uint32_t>>& descriptors)
{
	m_Bindings.binding = 0;
	m_Bindings.stride = size;
	m_Bindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	m_Attributes.reserve(descriptors.size());
	for (auto& [format, offset] : descriptors)
	{
		VkVertexInputAttributeDescription descriptor{};
		descriptor.binding = 0;
		descriptor.location = 0;
		descriptor.format = format;
		descriptor.offset = offset;

		m_Attributes.push_back(descriptor);
	}
}

DescriptorSetLayout::DescriptorSetLayout(const Device& device) :
	m_Handle(VK_NULL_HANDLE), m_Device(&device)
{
}

void DescriptorSetLayout::addBinding(VkDescriptorType type, VkShaderStageFlags target, uint32_t count)
{
	VkDescriptorSetLayoutBinding binding{};
	binding.binding = static_cast<uint32_t>(m_Bindings.size());
	binding.descriptorType = type;
	binding.descriptorCount = count;
	binding.stageFlags = target;
	binding.pImmutableSamplers = nullptr; // Optional
	m_Bindings.push_back(binding);
}

void DescriptorSetLayout::create()
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(m_Bindings.size());
	layoutInfo.pBindings = m_Bindings.data();

	ASSERT_VK_SUCCESS(vkCreateDescriptorSetLayout(m_Device->getHandle(), &layoutInfo, nullptr, &m_Handle), "Failed to create descriptor set layout!");
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	if (m_Handle != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(m_Device->getHandle(), m_Handle, nullptr);
	}
}

Shader::Shader(const Device& device, const std::string& name)
{
	std::ifstream file(name, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file!");
	}

	auto fileSize = file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

	file.close();

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = buffer.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	ASSERT_VK_SUCCESS(vkCreateShaderModule(m_Device->getHandle(), &createInfo, nullptr, &m_Handle),
		"Failed to create shader module!");
}

VkPipelineShaderStageCreateInfo Shader::getStage(VkShaderStageFlagBits stageType, const char* mainName) const
{
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = stageType;
	vertShaderStageInfo.module = m_Handle;
	vertShaderStageInfo.pName = mainName;

	return vertShaderStageInfo;
}

Shader::~Shader()
{
	vkDestroyShaderModule(m_Device->getHandle(), m_Handle, nullptr);
}

Pipeline::Pipeline(const RenderPass& renderPass, const std::string& vertexShader, const std::string& fragmentShader, const DescriptorSetLayout& descriptorSetLayout, const VertexLayout& vertexLayout)
{
	m_Device = &renderPass.getDevice();

	Shader vertShader(*m_Device, vertexShader);
	Shader fragShader(*m_Device, fragmentShader);

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShader.getStage(VK_SHADER_STAGE_VERTEX_BIT),
		fragShader.getStage(VK_SHADER_STAGE_FRAGMENT_BIT) };

	auto& attributeDescriptions = vertexLayout.getAttributes();
	auto vertexBindingDescriptions = vertexLayout.getBinding();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDescriptions;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = m_Device->getPhysicalDevice().getMsaaSamples();
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	auto dsl = descriptorSetLayout.getHandle();
	pipelineLayoutInfo.pSetLayouts = &dsl; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	ASSERT_VK_SUCCESS(vkCreatePipelineLayout(m_Device->getHandle(), &pipelineLayoutInfo, nullptr, &m_Layout),
		"Failed to create pipeline layout!");

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_Layout;
	pipelineInfo.renderPass = renderPass.getHandle();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	ASSERT_VK_SUCCESS(vkCreateGraphicsPipelines(m_Device->getHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Handle),
		"Failed to create graphics pipeline!");
}

Pipeline::~Pipeline()
{
	vkDestroyPipeline(m_Device->getHandle(), m_Handle, nullptr);
	vkDestroyPipelineLayout(m_Device->getHandle(), m_Layout, nullptr);
}

} // namespace vulture