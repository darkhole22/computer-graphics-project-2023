#pragma once

#include <string>

#include <vulkan/vulkan.h>

namespace computergraphicsproject {

class Instance
{
public:
	Instance();
	Instance(const std::string& applicationName);
	inline const VkInstance& getHandle() const { return m_Handle; }
	~Instance();
private:
	VkInstance m_Handle;
};

class DebugUtilMessanger
{
public:
	DebugUtilMessanger(const Instance& instance);
	inline const VkDebugUtilsMessengerEXT& getHandle() const { return m_Handle; }
	~DebugUtilMessanger();

private:
	const Instance& m_Instance;
	VkDebugUtilsMessengerEXT m_Handle;
};

}
