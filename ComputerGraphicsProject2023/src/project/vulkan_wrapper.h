#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

namespace computergraphicsproject {

class Instance
{
public:
	Instance();
	Instance(const std::string& applicationName);
	Instance(const Instance& other) = delete;
	Instance(Instance&& other) noexcept;

	inline const VkInstance& getHandle() const { return m_Handle; }

	const Instance operator=(const Instance& other) = delete;
	const Instance& operator=(Instance&& other) noexcept;

	~Instance();
private:
	VkInstance m_Handle;
};

class DebugUtilMessanger
{
public:
	DebugUtilMessanger();
	DebugUtilMessanger(const Instance& instance);
	DebugUtilMessanger(const DebugUtilMessanger& other) = delete;
	DebugUtilMessanger(DebugUtilMessanger&& other) noexcept;

	inline const VkDebugUtilsMessengerEXT& getHandle() const { return m_Handle; }

	const DebugUtilMessanger operator=(const DebugUtilMessanger& other) = delete;
	const DebugUtilMessanger& operator=(DebugUtilMessanger&& other) noexcept;

	~DebugUtilMessanger();
private:
	VkDebugUtilsMessengerEXT m_Handle;
	const Instance* m_Instance;
};

class Surface
{
public:
	Surface();
	Surface(const Surface& other) = delete;
	Surface(Surface&& other) noexcept;
	Surface(const Instance& instance, GLFWwindow* window);

	inline const VkSurfaceKHR& getHandle() const { return m_Handle; }

	const Surface operator=(const Surface& other) = delete;
	const Surface& operator=(Surface&& other) noexcept;

	~Surface();
private:
	VkSurfaceKHR m_Handle;
	const Instance* m_Instance;
};

class PhysicalDevice
{
public:
	inline PhysicalDevice() : 
		m_Handle(VK_NULL_HANDLE), m_QueueGraphicsFamily(0), m_QueuePresentFamily(0) {}
	inline PhysicalDevice(const PhysicalDevice& other) : 
		m_Handle(other.m_Handle), m_QueueGraphicsFamily(other.m_QueueGraphicsFamily), 
		m_QueuePresentFamily(other.m_QueuePresentFamily) {}

	static const PhysicalDevice pickDevice(const Instance& instance, const Surface& surface);

	inline const VkPhysicalDevice& getHandle() const { return m_Handle; }
	inline uint32_t getGraphicsQueueFamily() const {	return m_QueueGraphicsFamily; }
	inline uint32_t getPresentQueueFamily() const {	return m_QueuePresentFamily; }

	inline const PhysicalDevice& operator=(const PhysicalDevice& other) noexcept
	{
		m_Handle = other.m_Handle;
		return *this;
	}

	inline ~PhysicalDevice() {};
private:
	VkPhysicalDevice m_Handle;
	uint32_t m_QueueGraphicsFamily;
	uint32_t m_QueuePresentFamily;
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

	~Device();
private:
	VkDevice m_Handle;

	VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
	VkQueue m_PresentQueue = VK_NULL_HANDLE;
};

class Queue
{
public:
	Queue();

	~Queue();

	friend class Device;
private:

};



}
