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

#define ASSERT_VK_SUCCESS(func, message)   \
	if (func != VK_SUCCESS)                \
	{                                      \
		VUERROR(message);                  \
		return false;                      \
	}

namespace vulture {

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
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkCommandPool commandPool;

};

class VulkanContext
{
public:
	static bool init(const String& applicationName, const Window& window);

	static void cleanup();
};

} // namespace vulture
