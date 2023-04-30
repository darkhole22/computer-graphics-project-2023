#pragma once

#include "vulture/core/Core.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace vulture {

class RenderPass
{
public:
	NO_COPY(RenderPass)

	RenderPass(VkFormat attachmentFormat/*info*/);

	inline VkRenderPass getHandle() const { return m_Handle; }

	inline const std::vector<VkClearValue>& getClearValues() const { return m_ClearValues; }

	~RenderPass();
private:
	VkRenderPass m_Handle;
	std::vector<VkClearValue> m_ClearValues;
};

} // namespace vulture
