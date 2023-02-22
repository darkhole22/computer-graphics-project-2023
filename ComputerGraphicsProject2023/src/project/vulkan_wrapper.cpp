#include "vulkan_wrapper.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_set>
#include <optional>
#include <set>

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

static void checkRequiredExstentions(std::vector<std::string> const& requiredExtensions, std::unordered_set<std::string> const& aviableExtensions)
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

    std::unordered_set<std::string> aviableExtensionsSet{};

    for (const auto& extension : avilableExtensions) {
        aviableExtensionsSet.insert(std::string(extension.extensionName));
        std::cout << '\t' << extension.extensionName << '\n';
    }

    std::vector<std::string> requiredExstention(extensions.size());
    for (size_t i = 0; i < extensions.size(); i++)
    {
        requiredExstention[i] = extensions[i];
    }

    checkRequiredExstentions(requiredExstention, aviableExtensionsSet);

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    createInfo.enabledLayerCount = 0;

    createInfo.pNext = nullptr;
    VALIDATION_LAYER_IF(
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    )

    if (vkCreateInstance(&createInfo, nullptr, &m_Handle) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance!");
    }
}

Instance::Instance(Instance&& other) noexcept
{
    m_Handle = other.m_Handle;
    other.m_Handle = VK_NULL_HANDLE;
}

const Instance& Instance::operator=(Instance&& other) noexcept
{
    if (other.m_Handle != m_Handle)
    {
        if (m_Handle != VK_NULL_HANDLE)
            vkDestroyInstance(m_Handle, nullptr);
        m_Handle = other.m_Handle;
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
    m_Handle(VK_NULL_HANDLE), m_Instance(nullptr)
{
}

DebugUtilMessanger::DebugUtilMessanger(const Instance& instance)
    : m_Instance(&instance)
{
    m_Handle = VK_NULL_HANDLE;
    VALIDATION_LAYER_IF(
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance.getHandle(), &createInfo, nullptr, &m_Handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    )
}

DebugUtilMessanger::DebugUtilMessanger(DebugUtilMessanger&& other) noexcept
{
    m_Instance = other.m_Instance;
    m_Handle = other.m_Handle;

    other.m_Instance = nullptr;
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
            if (m_Instance && m_Handle != VK_NULL_HANDLE)
            {
                DestroyDebugUtilsMessengerEXT(m_Instance->getHandle(), m_Handle, nullptr);
            }
        )

        m_Instance = other.m_Instance;
        m_Handle = other.m_Handle;

        other.m_Instance = nullptr;
        other.m_Handle = VK_NULL_HANDLE;
    }

    return *this;
}

DebugUtilMessanger::~DebugUtilMessanger()
{
    VALIDATION_LAYER_IF(
        if (m_Instance && m_Handle != VK_NULL_HANDLE)
        {
            DestroyDebugUtilsMessengerEXT(m_Instance->getHandle(), m_Handle, nullptr);
        }
    )
}

Surface::Surface() :
    m_Handle(VK_NULL_HANDLE), m_Instance(nullptr)
{
}

Surface::Surface(Surface&& other) noexcept
{
    m_Instance = other.m_Instance;
    m_Handle = other.m_Handle;

    other.m_Instance = nullptr;
    other.m_Handle = VK_NULL_HANDLE;
}

Surface::Surface(const Instance& instance, GLFWwindow* window) :
    m_Handle(VK_NULL_HANDLE), m_Instance(&instance)
{
    if (glfwCreateWindowSurface(instance.getHandle(), window, nullptr, &m_Handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
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

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

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

const PhysicalDevice PhysicalDevice::pickDevice(const Instance& instance, const Surface& surface)
{
    PhysicalDevice physicalDevice;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.getHandle(), &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.getHandle(), &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (isDeviceSuitable(device, surface)) {
            QueueFamilyIndices indices = findQueueFamilies(device, surface);

            physicalDevice.m_Handle = device;
            physicalDevice.m_QueueGraphicsFamily = indices.graphicsFamily.value();
            physicalDevice.m_QueuePresentFamily = indices.presentFamily.value();

            // TODO m_MsaaSamples = getMaxUsableSampleCount();
            break;
        }
    }

    if (physicalDevice.m_Handle == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice.m_Handle, &deviceProperties);
    std::cout << "Device name: " << deviceProperties.deviceName << "\n";

    return physicalDevice;
}

Device::Device() :
    m_Handle(VK_NULL_HANDLE)
{
}

Device::Device(Device&& other) noexcept
{
    m_Handle = other.m_Handle;
    other.m_Handle = VK_NULL_HANDLE;
}

Device::Device(const PhysicalDevice& physicalDevice)
{
    uint32_t queueGraphicsFamily = physicalDevice.getGraphicsQueueFamily();
    uint32_t queuePresentFamily = physicalDevice.getPresentQueueFamily();

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

    if (vkCreateDevice(physicalDevice.getHandle(), &createInfo, nullptr, &m_Handle) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
}

const Device& Device::operator=(Device&& other) noexcept
{
    if (other.m_Handle != m_Handle)
    {
        if (m_Handle != VK_NULL_HANDLE)
            vkDestroyDevice(m_Handle, nullptr);

        m_Handle = other.m_Handle;
        other.m_Handle = VK_NULL_HANDLE;
    }

    return *this;
}

Device::~Device()
{
    if (m_Handle != VK_NULL_HANDLE)
        vkDestroyDevice(m_Handle, nullptr);
}

}
