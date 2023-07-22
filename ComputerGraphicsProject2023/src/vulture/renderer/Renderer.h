#pragma once

#include "Window.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "DescriptorSet.h"
#include "Pipeline.h"
#include "Model.h"

namespace vulture {

class FrameContext;

/**
 * @struct ResourceInfo
 *
 * @brief Contains information about a resource, particularly its path.
 */
struct ResourceInfo
{
	String path;
};

/**
 * @struct RendererData
 *
 * @brief Holds data required for initializing the Renderer.
 */
struct RendererData
{
	ResourceInfo resourceInfo;

	SwapChain* swapChain = nullptr;
	RenderPass* renderPass = nullptr;

	u32 currentFrame = 0;
};

/**
 * @class Renderer
 *
 * @brief Manages the rendering system and its components.
 */
class Renderer
{
public:
	/**
	* @brief Initializes the Renderer with the specified application name and window.
	*
	* @param applicationName The name of the application.
	* @param window The window instance to associate with the Renderer.
	* @return True if initialization is successful; otherwise, false.
	*/
	static bool init(const String& applicationName, const Window& window);

	/**
	* @brief Cleans up and releases resources used by the Renderer.
	*/
	static void cleanup();

	/**
	 * @brief Retrieves the FrameContext for the current frame.
	 *
	 * @return FrameContext instance representing the current frame's rendering context.
	 */
	static FrameContext getFrameContext();

	/**
	 * @brief Waits for the rendering to finish before continuing execution.
	 */
	static void waitIdle();

	/**
	 * @brief Gets the current RenderPass used by the Renderer.
	 *
	 * @return Reference to the RenderPass currently used by the Renderer.
	 */
	static const RenderPass& getRenderPass();

	/**
	 * @brief Creates a DescriptorPool suitable for the Renderer's current swap chain image count.
	 *
	 * @return DescriptorPool instance created with the Renderer's swap chain image count.
	 */
	static inline DescriptorPool makeDescriptorPool() { return DescriptorPool(getImageCount()); }

	/**
	 * @brief Creates a Uniform buffer for the Renderer with the specified type.
	 *
	 * @tparam T Type of data to be stored in the Uniform buffer.
	 * @return Uniform buffer instance created with the Renderer's swap chain image count.
	 */
	template <class T> static inline Uniform<T> makeUniform() { return Uniform<T>(getImageCount()); }

	/**
	 * @brief Gets the default VertexLayout used by the Renderer .
	 *
	 * @return The VertexLayout associated with the Renderer.
	 */
	static inline const VertexLayout getVertexLayout()
	{
		return VertexLayout(sizeof(Vertex), {
			{VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, pos))},
			{VK_FORMAT_R32G32B32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, norm))},
			{VK_FORMAT_R32G32_SFLOAT, static_cast<uint32_t>(offsetof(Vertex, texCoord))}
		});
	}
private:
	/**
	 * @brief Gets the number of images in the Renderer's swap chain.
	 *
	 * @return The number of images in the swap chain.
	 */
	static u32 getImageCount();
};

} // namespace vulture