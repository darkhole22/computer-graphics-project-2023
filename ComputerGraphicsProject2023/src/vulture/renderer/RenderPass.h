#pragma once

#include "vulture/core/Core.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace vulture {

/**
* @class RenderPass
*
* @brief Represents a Vulkan render pass used for rendering operations.
*/
class RenderPass
{
public:
	NO_COPY(RenderPass)

	/**
	 * @brief Constructor for the RenderPass class.
	 *
	 * @param attachmentFormat The format of the attachment used in the render pass.
	 */
	explicit RenderPass(VkFormat attachmentFormat);

	/**
	 * @brief Gets the Vulkan handle of the render pass.
	 *
	 * @return The Vulkan handle of the render pass.
	 */
	inline VkRenderPass getHandle() const { return m_Handle; }

	/**
	 * @brief Gets the clear values used in the render pass.
	 *
	 * @return A vector of VkClearValue structures used for clearing attachments.
	 */
	inline const std::vector<VkClearValue>& getClearValues() const { return m_ClearValues; }

	~RenderPass();
private:
	VkRenderPass m_Handle;
	std::vector<VkClearValue> m_ClearValues;
};

} // namespace vulture
