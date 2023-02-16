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
	inline PhysicalDevice() : m_Handle(VK_NULL_HANDLE) {}
	inline PhysicalDevice(const PhysicalDevice& other) : m_Handle(other.m_Handle) {}

	static const PhysicalDevice& pickDevice(const Instance& instance, const Surface& surface);

	inline const VkPhysicalDevice& getHandle() const { return m_Handle; }

	inline const PhysicalDevice& operator=(const PhysicalDevice& other) noexcept
	{
		m_Handle = other.m_Handle;
		return *this;
	}

	inline ~PhysicalDevice() {};
private:
	VkPhysicalDevice m_Handle;
};



}
