#include "VulkanContext.h"

// #define VU_LOGGER_TRACE_ENABLED
// #define VU_LOGGER_DISABLE_INFO
#include "vulture/core/Logger.h"

#include <array>
#include <vector>
#include <optional>
#include <set>

#define ASSERT_VK_SUCCESS(func, message)   \
	if (func != VK_SUCCESS)                \
	{                                      \
		VUERROR(message);                  \
		return false;                      \
	}

namespace vulture {

VulkanContextData vulkanData = {};

static const std::array<const char*, 1> validationLayers = { "VK_LAYER_KHRONOS_validation" };
static const std::vector<const char*> deviceRequiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
static const std::vector<const char*> deviceRequiredIfPresentExtensions = { "VK_KHR_portability_subset" };

inline bool areValidationLayersSupported();
inline bool checkIfItHasExtension(const char* ext, const std::vector<VkExtensionProperties>& availableExt);

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);

static bool createInstace(const char* name);
static bool pickPhysicalDevice();
static bool createDevice();

bool VulkanContext::init(const String& applicationName, const Window& window)
{
	if (!createInstace(applicationName.cString()))
		return false;

	ASSERT_VK_SUCCESS(glfwCreateWindowSurface(vulkanData.instance, window.getHandle(), vulkanData.allocator, &vulkanData.surface),
		"Failed to create the Window surface!");
	VUTRACE("Vulkan Surface created.");

	if (!pickPhysicalDevice())
		return false;

	if (!createDevice())
		return false;

	return true;
}

void VulkanContext::cleanup()
{
	if (vulkanData.device != VK_NULL_HANDLE && vulkanData.commandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(vulkanData.device, vulkanData.commandPool, vulkanData.allocator);
		vulkanData.commandPool = VK_NULL_HANDLE;
	}

	if (vulkanData.device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(vulkanData.device, vulkanData.allocator);
		vulkanData.device = VK_NULL_HANDLE;
		vulkanData.physicalDevice = VK_NULL_HANDLE;
		vulkanData.physicalDeviceProperties = {};
		vulkanData.physicalDeviceFeatures = {};
		vulkanData.msaaSamples = VK_SAMPLE_COUNT_1_BIT;
		vulkanData.graphicsQueue = VK_NULL_HANDLE;
		vulkanData.presentQueue = VK_NULL_HANDLE;
	}

	if (vulkanData.instance != VK_NULL_HANDLE && vulkanData.surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(vulkanData.instance, vulkanData.surface, vulkanData.allocator);
		vulkanData.surface = VK_NULL_HANDLE;
	}

#if VALIDATION_LAYER
	if (vulkanData.debugMessenger != VK_NULL_HANDLE)
	{
		auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanData.instance, "vkDestroyDebugUtilsMessengerEXT");
		if (vkDestroyDebugUtilsMessengerEXT != nullptr)
			vkDestroyDebugUtilsMessengerEXT(vulkanData.instance, vulkanData.debugMessenger, vulkanData.allocator);
		vulkanData.debugMessenger = VK_NULL_HANDLE;
	}
#endif

	if (vulkanData.instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(vulkanData.instance, vulkanData.allocator);
		vulkanData.instance = VK_NULL_HANDLE;
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
	{
		VUTRACE("[Validation layer|VERBOSE]: [ID]: %s\n\t[message]: %s", pCallbackData->pMessageIdName, pCallbackData->pMessage);
	}
	break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
	{
		VUTRACE("[Validation layer|INFO]: [ID]: %s\n\t[message]: %s", pCallbackData->pMessageIdName, pCallbackData->pMessage);
	}
	break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
	{
		VUWARN("[Validation layer|WARNING]: [ID]: %s\n\t[message]: %s", pCallbackData->pMessageIdName, pCallbackData->pMessage);
	}
	break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
	{
		if (strcmp(pCallbackData->pMessageIdName, "Loader Message") == 0) // Skip message caused by other application
		{
			return VK_FALSE;
		}
		VUERROR("[Validation layer|ERROR]: [ID]: %s\n\t[message]: %s", pCallbackData->pMessageIdName, pCallbackData->pMessage);
	}
	break;
	default:
		break;
	}
	return VK_FALSE;
}

inline bool areValidationLayersSupported()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

#ifndef VU_LOGGER_DISABLE_INFO
	String instaceLayerMessage = "";
	for (const auto& layerProperties : availableLayers)
	{
		instaceLayerMessage = instaceLayerMessage + "\n\t" + layerProperties.layerName + ": " + layerProperties.description;
	}
	VUINFO("Instance Layers: %s.", instaceLayerMessage.cString());
#endif

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
			return false;
	}

	return true;
}

inline bool checkIfItHasExtension(const char* extension, const std::vector<VkExtensionProperties>& availableExt)
{
	for (const auto& extProp : availableExt)
	{
		if (strcmp(extension, extProp.extensionName) == 0)
			return true;
	}

	return false;
}

bool createInstace(const char* name)
{
	VUTRACE("Begin creating Vulkan instance.");
#if VALIDATION_LAYER
	if (!areValidationLayersSupported())
	{
		VUERROR("Validation layers required, but not available!");
		return false;
	}
#endif

	VkApplicationInfo appInfo = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = name;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Vulture";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	u32 extensionPropertiesCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensionProperties(extensionPropertiesCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertiesCount, availableExtensionProperties.data());

	u32 glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	VALIDATION_LAYER_IF(extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));

	if (checkIfItHasExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, availableExtensionProperties))
	{
		extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	}
	if (checkIfItHasExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, availableExtensionProperties))
	{
		extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
	}

	VkInstanceCreateInfo instanceCreateInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.ppEnabledLayerNames = nullptr;
	instanceCreateInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

#if VALIDATION_LAYER
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	debugCreateInfo.pNext = nullptr;
	debugCreateInfo.flags = 0;
	debugCreateInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = debugCallback;
	debugCreateInfo.pUserData = nullptr;

	instanceCreateInfo.pNext = &debugCreateInfo;
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#endif

	ASSERT_VK_SUCCESS(vkCreateInstance(&instanceCreateInfo, vulkanData.allocator, &vulkanData.instance),
		"Failed to create the instance!");

#if VALIDATION_LAYER
	{
		VkResult result = VK_ERROR_EXTENSION_NOT_PRESENT;
		auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanData.instance, "vkCreateDebugUtilsMessengerEXT");
		if (vkCreateDebugUtilsMessengerEXT != nullptr)
		{
			result = vkCreateDebugUtilsMessengerEXT(vulkanData.instance, &debugCreateInfo, vulkanData.allocator, &vulkanData.debugMessenger);
		}

		ASSERT_VK_SUCCESS(result, "Failed to set up the debug messenger!");
	}
#endif

	VUTRACE("Vulkan instance created.");
	return true;
}

struct QueueFamilyIndices
{
	std::optional<u32> graphicsFamily;
	std::optional<u32> presentFamily;

	bool isComplete() const
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (u32 i = 0; i < queueFamilyCount && !indices.isComplete(); ++i)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;
		
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vulkanData.surface, &presentSupport);
		if (presentSupport)
			indices.presentFamily = i;
	}

	return indices;
}

static bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	u32 extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<String> requiredExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkanData.surface, &details.capabilities);

	u32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkanData.surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkanData.surface, &formatCount, details.formats.data());
	}

	u32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkanData.surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkanData.surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool isDeviceSuitable(VkPhysicalDevice device)
{
	// VkPhysicalDeviceProperties deviceProperties;
	// vkGetPhysicalDeviceProperties(device, &deviceProperties);
	// VkPhysicalDeviceFeatures deviceFeatures;
	// vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate; // && deviceFeatures.samplerAnisotropy;
}

static VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT)
		return VK_SAMPLE_COUNT_64_BIT;
	if (counts & VK_SAMPLE_COUNT_32_BIT)
		return VK_SAMPLE_COUNT_32_BIT;
	if (counts & VK_SAMPLE_COUNT_16_BIT)
		return VK_SAMPLE_COUNT_16_BIT;
	if (counts & VK_SAMPLE_COUNT_8_BIT)
		return VK_SAMPLE_COUNT_8_BIT;
	if (counts & VK_SAMPLE_COUNT_4_BIT)
		return VK_SAMPLE_COUNT_4_BIT;
	if (counts & VK_SAMPLE_COUNT_2_BIT)
		return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;
}

static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice)
{
	std::array<VkFormat, 3> candidates = {
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D24_UNORM_S8_UINT };

	VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;

	VkFormatFeatureFlags features =	
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | 
		VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
	
	for (VkFormat format : candidates)
	{
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
			return format;
		
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
			return format;
	}

	return VK_FORMAT_UNDEFINED;
}

bool pickPhysicalDevice()
{
	u32 deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanData.instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		VUERROR("Failed to find GPUs with Vulkan support!");
		return false;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanData.instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			vulkanData.physicalDevice = device;
			vulkanData.msaaSamples = getMaxUsableSampleCount(device);
			vulkanData.depthFormat = findDepthFormat(device);
			// physicalDevice.m_SwapChainSupportDetails = querySwapChainSupport(device, surface);

			break;
		}
	}

	if (vulkanData.physicalDevice == VK_NULL_HANDLE)
	{
		VUERROR("Failed to find a suitable GPU!");
		return false;
	}
	if (vulkanData.depthFormat == VK_FORMAT_UNDEFINED)
	{
		VUERROR("Failed to find supported depth format!");
		return false;
	}

	vkGetPhysicalDeviceProperties(vulkanData.physicalDevice, &vulkanData.physicalDeviceProperties);
	vkGetPhysicalDeviceFeatures(vulkanData.physicalDevice, &vulkanData.physicalDeviceFeatures);

	VUINFO("Using \"%s\" as device.", vulkanData.physicalDeviceProperties.deviceName);

	return true;
}

bool createDevice()
{
	VUTRACE("Begin creating Vulkan logical Device.");

	QueueFamilyIndices queueIndices = findQueueFamilies(vulkanData.physicalDevice);
	u32 queueGraphicsFamily = queueIndices.graphicsFamily.value();
	u32 queuePresentFamily = queueIndices.presentFamily.value();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<u32> uniqueQueueFamilies = { queueGraphicsFamily, queuePresentFamily };

	float queuePriority = 1.0f;
	for (u32 queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = vulkanData.physicalDeviceFeatures.samplerAnisotropy ? VK_TRUE : VK_FALSE;
	deviceFeatures.sampleRateShading = VK_TRUE;

	std::vector<const char*> deviceExtensions(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());

	u32 extensionCount;
	vkEnumerateDeviceExtensionProperties(vulkanData.physicalDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(vulkanData.physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	for (auto extension : deviceRequiredIfPresentExtensions)
	{
		for (const auto& extProp : availableExtensions)
		{
			if (strcmp(extension, extProp.extensionName) == 0)
			{
				deviceExtensions.push_back(extension);
				break;
			}
		}
	}

	VkDeviceCreateInfo createInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
#if VALIDATION_LAYER
	createInfo.enabledLayerCount = static_cast<u32>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();
#else
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
#endif
	createInfo.enabledExtensionCount = static_cast<u32>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	ASSERT_VK_SUCCESS(vkCreateDevice(vulkanData.physicalDevice, &createInfo, vulkanData.allocator, &vulkanData.device),
		"Failed to create logical device!");
	
	vulkanData.graphicsQueueFamily = queueGraphicsFamily;
	vkGetDeviceQueue(vulkanData.device, queueGraphicsFamily, 0, &vulkanData.graphicsQueue);
	vulkanData.presentQueueFamily = queuePresentFamily;
	vkGetDeviceQueue(vulkanData.device, queuePresentFamily, 0, &vulkanData.presentQueue);

	VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	poolInfo.pNext = nullptr;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueGraphicsFamily;

	ASSERT_VK_SUCCESS(vkCreateCommandPool(vulkanData.device, &poolInfo, vulkanData.allocator, &vulkanData.commandPool),
		"Failed to create command pool!");

	VUTRACE("Vulkan logical Device created.");
	return true;
}

} // namespace vulture
