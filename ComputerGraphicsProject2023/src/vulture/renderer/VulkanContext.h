#pragma once

#include "vulture/util/String.h"
#include "Window.h"

#ifdef VU_DEBUG_BUILD
// DEBUG
#define VALIDATION_LAYER true
#define VALIDATION_LAYER_IF(x) x
#else
// RELEASE
#define VALIDATION_LAYER false
#define VALIDATION_LAYER_IF(x)
#endif // !NDEBUG

namespace vulture {

/**
* @struct VulkanContextData
*
* @brief This struct holds all Vulkan-related info needed by Vulture's backend components.
*
*/
struct VulkanContextData
{
	VkAllocationCallbacks* allocator = nullptr;

	VkInstance instance = VK_NULL_HANDLE;
#if VALIDATION_LAYER
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
#endif
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties physicalDeviceProperties = {};
	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkFormat depthFormat = VK_FORMAT_UNDEFINED;
	VkDevice device = VK_NULL_HANDLE;
	u32 graphicsQueueFamily = 0;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	u32 presentQueueFamily = 0;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkCommandPool commandPool;

};

/**
* @class VulkanContext
*
* @brief Provides a simple way to initialize and cleanup Vulkan resources.
*/
class VulkanContext
{
public:
	/**
	* @brief Initializes the Vulkan context for the application.
	*
	* @param applicationName The name of the application.
	* @param window The window instance to associate the Vulkan context with.
	* @return true if initialization is successful; otherwise, false.
	*/
	static bool init(const String& applicationName, const Window& window);

	/**
	* @brief Cleans up and releases resources used by the Vulkan context.
	*/
	static void cleanup();
};

} // namespace vulture
