#include "vulkan_wrapper.h"

#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <optional>
#include <set>
#include <algorithm>
#include <array>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifndef NDEBUG
// DEBUG
#define VALIDATION_LAYER true
#define VALIDATION_LAYER_IF(x) x
#else
// RELESE
#define VALIDATION_LAYER false
#define VALIDATION_LAYER_IF(x)
#endif // !NDEBUG

#define ASSERT_VK_SUCCESS(func, message) if (func != VK_SUCCESS) { throw std::runtime_error(message); }

namespace computergraphicsproject {

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

Instance::Instance() :
    m_Handle(VK_NULL_HANDLE)
{
}

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

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    VALIDATION_LAYER_IF(extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));

    return extensions;
}

static void checkRequiredExtensions(std::vector<std::string> const& requiredExtensions, std::unordered_set<std::string> const& aviableExtensions)
{
    for (const auto& requiredExstention : requiredExtensions)
        if (aviableExtensions.count(requiredExstention) == 0)
            std::cerr << "Missing extension: " << requiredExstention << "\n";
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT

        // Message is important enough to show
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

Instance::Instance(const std::string& applicationName)
{
    VALIDATION_LAYER_IF(
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
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

    createInfo.enabledLayerCount = 0;
    VALIDATION_LAYER_IF(
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    )

    uint32_t availableExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

    std::vector<VkExtensionProperties> avilableExtensions(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, avilableExtensions.data());

    std::cout << "Available extensions:\n";

    std::unordered_set<std::string> availableExtensionsSet{};

    for (const auto& extension : avilableExtensions) {
        availableExtensionsSet.insert(std::string(extension.extensionName));
        std::cout << '\t' << extension.extensionName << '\n';
    }

    std::vector<std::string> requiredExstention(extensions.size());
    for (size_t i = 0; i < extensions.size(); i++)
    {
        requiredExstention[i] = extensions[i];
    }

    checkRequiredExtensions(requiredExstention, availableExtensionsSet);

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    createInfo.enabledLayerCount = 0;

    createInfo.pNext = nullptr;
    VALIDATION_LAYER_IF(
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        )

    ASSERT_VK_SUCCESS(vkCreateInstance(&createInfo, nullptr, &m_Handle), "Failed to create instance!");

    m_DebugMessanger = std::move(DebugUtilMessanger(m_Handle));
}

Instance::Instance(Instance&& other) noexcept
{
    m_Handle = other.m_Handle;
    m_DebugMessanger = std::move(other.m_DebugMessanger);

    other.m_Handle = VK_NULL_HANDLE;
}

const Instance& Instance::operator=(Instance&& other) noexcept
{
    if (other.m_Handle != m_Handle)
    {
        if (m_Handle != VK_NULL_HANDLE)
            vkDestroyInstance(m_Handle, nullptr);
        
        m_Handle = other.m_Handle;
        m_DebugMessanger = std::move(other.m_DebugMessanger);

        other.m_Handle = VK_NULL_HANDLE;
    }

    return *this;
}

Instance::~Instance()
{
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

DebugUtilMessanger::DebugUtilMessanger() :
    m_Handle(VK_NULL_HANDLE), m_Instance(VK_NULL_HANDLE)
{
}

DebugUtilMessanger::DebugUtilMessanger(const VkInstance& instance)
    : m_Instance(instance)
{
    m_Handle = VK_NULL_HANDLE;
    VALIDATION_LAYER_IF(
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        ASSERT_VK_SUCCESS(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &m_Handle), "Failed to set up debug messenger!")
    )
}

DebugUtilMessanger::DebugUtilMessanger(DebugUtilMessanger&& other) noexcept
{
    m_Instance = other.m_Instance;
    m_Handle = other.m_Handle;

    other.m_Instance = VK_NULL_HANDLE;
    other.m_Handle = VK_NULL_HANDLE;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

const DebugUtilMessanger& DebugUtilMessanger::operator=(DebugUtilMessanger&& other) noexcept
{
    if (m_Handle != other.m_Handle)
    {
        VALIDATION_LAYER_IF(
            if (m_Instance != VK_NULL_HANDLE && m_Handle != VK_NULL_HANDLE)
            {
                DestroyDebugUtilsMessengerEXT(m_Instance, m_Handle, nullptr);
            }
        )

        m_Instance = other.m_Instance;
        m_Handle = other.m_Handle;

        other.m_Instance = VK_NULL_HANDLE;
        other.m_Handle = VK_NULL_HANDLE;
    }

    return *this;
}

DebugUtilMessanger::~DebugUtilMessanger()
{
    VALIDATION_LAYER_IF(
        if (m_Instance != VK_NULL_HANDLE && m_Handle != VK_NULL_HANDLE)
        {
            DestroyDebugUtilsMessengerEXT(m_Instance, m_Handle, nullptr);
        }
    )
}

Surface::Surface() :
    m_Handle(VK_NULL_HANDLE), m_Instance(nullptr), m_Window(nullptr)
{
}

Surface::Surface(Surface&& other) noexcept
{
    m_Instance = other.m_Instance;
    m_Handle = other.m_Handle;
    m_Window = other.m_Window;

    other.m_Instance = nullptr;
    other.m_Window = nullptr;
    other.m_Handle = VK_NULL_HANDLE;
}

Surface::Surface(const Instance& instance, GLFWwindow* window) :
    m_Handle(VK_NULL_HANDLE), m_Instance(&instance), m_Window(window)
{
    ASSERT_VK_SUCCESS(glfwCreateWindowSurface(instance.getHandle(), window, nullptr, &m_Handle), "Failed to create window surface!")
}

const Surface& Surface::operator=(Surface&& other) noexcept
{
    if (other.m_Handle != m_Handle)
    {
        if (m_Instance && m_Handle != VK_NULL_HANDLE)
            vkDestroySurfaceKHR(m_Instance->getHandle(), m_Handle, nullptr);

        m_Instance = other.m_Instance;
        m_Handle = other.m_Handle;

        other.m_Instance = nullptr;
        other.m_Handle = VK_NULL_HANDLE;
    }

    return *this;
}

Surface::~Surface()
{
    if (m_Instance && m_Handle != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(m_Instance->getHandle(), m_Handle, nullptr);
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
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

static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

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
        std::cout << swapChainSupport.formats.size() << "\n";
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

const PhysicalDevice PhysicalDevice::pickDevice(const Instance& instance, const Surface& surface)
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
    std::cout << "Device name: " << deviceProperties.deviceName << "\n";

    return physicalDevice;
}

Device::Device() :
    m_Handle(VK_NULL_HANDLE), m_PhysicalDevice(nullptr)
{
}

Device::Device(Device&& other) noexcept
{
    m_Handle = other.m_Handle;
    m_GraphicsQueue = other.m_GraphicsQueue;
    m_PresentQueue = other.m_PresentQueue;
    m_PhysicalDevice = other.m_PhysicalDevice;

    other.m_Handle = VK_NULL_HANDLE;
    other.m_PhysicalDevice = nullptr;
    other.m_GraphicsQueue = VK_NULL_HANDLE;
    other.m_PresentQueue = VK_NULL_HANDLE;
}

Device::Device(const PhysicalDevice& physicalDevice)
{
    m_PhysicalDevice = &physicalDevice;

    uint32_t queueGraphicsFamily = physicalDevice.getGraphicsQueueFamilyIndex();
    uint32_t queuePresentFamily = physicalDevice.getPresentQueueFamilyIndex();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { queueGraphicsFamily, queuePresentFamily };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
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

    ASSERT_VK_SUCCESS(vkCreateDevice(physicalDevice.getHandle(), &createInfo, nullptr, &m_Handle), "Failed to create logical device!")

    vkGetDeviceQueue(m_Handle, queueGraphicsFamily, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Handle, queuePresentFamily, 0, &m_PresentQueue);
}

const Device& Device::operator=(Device&& other) noexcept
{
    if (other.m_Handle != m_Handle)
    {
        if (m_Handle != VK_NULL_HANDLE)
            vkDestroyDevice(m_Handle, nullptr);

        m_Handle = other.m_Handle;
        m_PhysicalDevice = other.m_PhysicalDevice;
        m_GraphicsQueue = other.m_GraphicsQueue;
        m_PresentQueue = other.m_PresentQueue;

        other.m_Handle = VK_NULL_HANDLE;
        other.m_PhysicalDevice = nullptr;
        other.m_GraphicsQueue = VK_NULL_HANDLE;
        other.m_PresentQueue = VK_NULL_HANDLE;
    }

    return *this;
}

Device::~Device()
{
    if (m_Handle != VK_NULL_HANDLE)
        vkDestroyDevice(m_Handle, nullptr);
}

Image::Image() :
    m_Handle(VK_NULL_HANDLE), m_Memory(VK_NULL_HANDLE), m_View(VK_NULL_HANDLE), m_Device(nullptr)
{
}

Image::Image(Image&& other) noexcept
{
    m_Handle = other.m_Handle;
    m_Memory = other.m_Memory;
    m_View = other.m_View;
    m_Device = other.m_Device;

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
    m_Memory(VK_NULL_HANDLE), m_View(VK_NULL_HANDLE)
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

Image::Image(const Device& device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags)
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

    if (vkCreateImage(m_Device->getHandle(), &imageInfo, nullptr, &m_Handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_Device->getHandle(), m_Handle, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(m_Device->getPhysicalDevice(), memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_Device->getHandle(), &allocInfo, nullptr, &m_Memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_Device->getHandle(), m_Handle, m_Memory, 0);

    m_View = createImageView(m_Handle, device, format, aspectFlags);
}

const Image& Image::operator=(Image&& other) noexcept
{
    if (m_Handle != other.m_Handle)
    {
        if (m_Handle != VK_NULL_HANDLE)
        {
            vkDestroyImageView(m_Device->getHandle(), m_View, nullptr);
            if (m_Memory != VK_NULL_HANDLE)
            {
                vkDestroyImage(m_Device->getHandle(), m_Handle, nullptr);
                vkFreeMemory(m_Device->getHandle(), m_Memory, nullptr);
            }
        }

        m_Handle = other.m_Handle;
        m_Memory = other.m_Memory;
        m_View = other.m_View;
        m_Device = other.m_Device;

        other.m_Handle = VK_NULL_HANDLE;
        other.m_Memory = VK_NULL_HANDLE;
        other.m_View = VK_NULL_HANDLE;
        other.m_Device = nullptr;
    }

    return *this;
}

Image::~Image()
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

    throw std::runtime_error("failed to find supported format!");
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

RenderPass::RenderPass() :
    m_Handle(VK_NULL_HANDLE), m_Device(nullptr)
{
}

RenderPass::RenderPass(RenderPass&& other) noexcept
{
    m_Handle = other.m_Handle;
    m_Device = other.m_Device;

    other.m_Handle = VK_NULL_HANDLE;
    other.m_Device = nullptr;
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
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device.getHandle(), &renderPassInfo, nullptr, &m_Handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

const RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
{
    if (m_Handle != other.m_Handle)
    {
        if (m_Handle != VK_NULL_HANDLE)
            vkDestroyRenderPass(m_Device->getHandle(), m_Handle, nullptr);

        m_Handle = other.m_Handle;
        m_Device = other.m_Device;

        other.m_Handle = VK_NULL_HANDLE;
        other.m_Device = nullptr;
    }

    return *this;
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

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };

        return actualExtent;
    }
}

SwapChain::SwapChain() :
    m_Handle(VK_NULL_HANDLE), m_ImageFormat(VK_FORMAT_UNDEFINED), m_Extent({ 0,0 }), 
    m_Device(nullptr), m_Surface(nullptr), m_RenderPass(nullptr)
{
}

SwapChain::SwapChain(SwapChain&& other) noexcept
{
    m_Handle = other.m_Handle;
    m_Images = std::move(other.m_Images);
    m_ImageFormat = other.m_ImageFormat;
    m_Extent = other.m_Extent;
    m_ColorImage = std::move(other.m_ColorImage);
    m_DepthImage = std::move(other.m_DepthImage);
    m_Framebuffers = std::move(other.m_Framebuffers);
    m_Device = other.m_Device;
    m_Surface = other.m_Surface;
    m_RenderPass = other.m_RenderPass;

    other.m_Handle = VK_NULL_HANDLE;
    other.m_ImageFormat = VK_FORMAT_UNDEFINED;
    other.m_Extent = { 0,0 };
    other.m_Device = nullptr;
    other.m_Surface = nullptr;
    other.m_RenderPass = nullptr;
}

SwapChain::SwapChain(const Device& device, const Surface& surface, const RenderPass& renderPass) :
    m_Device(&device), m_Surface(&surface), m_RenderPass(&renderPass)
{
    create();
}

void SwapChain::create()
{
    const PhysicalDevice& physicalDevice = m_Device->getPhysicalDevice();
    const SwapChainSupportDetails& swapChainSupport = physicalDevice.getSwapChainSupportDetails();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, m_Surface->getWindow());

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
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice.getHandle(), *m_Surface);
    uint32_t queueFamilyIndices[] = { physicalDevice.getGraphicsQueueFamilyIndex(), physicalDevice.getPresentQueueFamilyIndex() };

    if (indices.graphicsFamily != indices.presentFamily) {
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

    ASSERT_VK_SUCCESS(vkCreateSwapchainKHR(m_Device->getHandle(), &createInfo, nullptr, &m_Handle), "Failed to create swap chain!");

    std::vector<VkImage> vkImages;

    vkGetSwapchainImagesKHR(m_Device->getHandle(), m_Handle, &imageCount, nullptr);
    vkImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Device->getHandle(), m_Handle, &imageCount, vkImages.data());

    m_ImageFormat = surfaceFormat.format;
    m_Extent = extent;

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

    // TODO transitionImageLayout(m_DepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
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

        if (vkCreateFramebuffer(m_Device->getHandle(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

void SwapChain::recreate()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_Surface->getWindow(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_Surface->getWindow(), &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_Device->getHandle());

    cleanup();

    create();
}

void SwapChain::cleanup()
{
    if (m_Handle == VK_NULL_HANDLE) return;

    m_Images.clear();

    for (size_t i = 0; i < m_Framebuffers.size(); i++) {
        vkDestroyFramebuffer(m_Device->getHandle(), m_Framebuffers[i], nullptr);
    }

    vkDestroySwapchainKHR(m_Device->getHandle(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;
}

const SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
{
    if (other.m_Handle != m_Handle)
    {
        cleanup();

        m_Handle = other.m_Handle;
        m_Images = std::move(other.m_Images);
        m_ImageFormat = other.m_ImageFormat;
        m_Extent = other.m_Extent;
        m_ColorImage = std::move(other.m_ColorImage);
        m_DepthImage = std::move(other.m_DepthImage);
        m_Framebuffers = std::move(other.m_Framebuffers);
        m_Device = other.m_Device;
        m_Surface = other.m_Surface;
        m_RenderPass = other.m_RenderPass;

        other.m_Handle = VK_NULL_HANDLE;
        other.m_ImageFormat = VK_FORMAT_UNDEFINED;
        other.m_Extent = { 0,0 };
        other.m_Device = nullptr;
        other.m_Surface = nullptr;
        other.m_RenderPass = nullptr;
    }

    return *this;
}

SwapChain::~SwapChain()
{
    cleanup();
}

}
