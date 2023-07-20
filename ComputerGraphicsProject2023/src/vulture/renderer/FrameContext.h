#pragma once

#include "Renderer.h"

namespace vulture {

/**
 * @struct FrameInfo
 *
 * @brief Contains information about a frame.
 */
struct FrameInfo
{
	u32 index;
	u32 count;
};

/**
 * @class FrameContext
 *
 * @brief Manages per-frame context information for rendering.
 */
class FrameContext
{
public:
	NO_COPY(FrameContext)

	/**
	 * @brief Gets the FrameInfo for the current frame.
	 *
	 * @return FrameInfo structure containing the index and count of the current frame.
	 */
	inline FrameInfo getFrameInfo() const { return {m_ImageIndex, m_ImageCount}; }

	/**
	 * @brief Checks if the FrameContext has been updated due to swap chain recreation.
	 *
	 * @return True if the FrameContext has been updated; otherwise, false.
	 */
	inline bool updated() const { return m_SwapChainRecreated; }

	/**
	 * @brief Gets the extent of the swap chain (rendering area) associated with this FrameContext.
	 *
	 * @return The extent (width and height) of the swap chain.
	 */
	inline const VkExtent2D& getExtent() const { return m_SwapChain->getExtent(); }

	/**
	 * @brief Begins recording rendering commands into the FrameContext's command buffer.
	 */
	void beginCommandRecording();

	/**
	 * @brief Ends recording rendering commands into the FrameContext's command buffer.
	 */
	void endCommandRecording();

	/**
	 * @brief Binds the specified pipeline for rendering commands.
	 *
	 * @param pipeline The rendering pipeline to be bound.
	 */
	void bindPipeline(const Pipeline& pipeline);

	/**
	 * @brief Binds the specified descriptor set for rendering commands.
	 *
	 * @param pipeline The rendering pipeline to bind the descriptor set to.
	 * @param descriptorSet The descriptor set to be bound.
	 * @param set The set number of the descriptor set.
	 */
	void bindDescriptorSet(const Pipeline& pipeline, const DescriptorSet& descriptorSet, u32 set);

	/**
	 * @brief Draws the specified model during rendering commands.
	 *
	 * @param model The model to be drawn.
	 */
	void drawModel(const Model& model);

	/**
	 * @brief Binds the specified vertex buffer for rendering commands.
	 *
	 * @param buffer The vertex buffer to be bound.
	 */
	void bindVertexBuffer(const Buffer& buffer);

	/**
	 * @brief Binds the specified index buffer for rendering commands.
	 *
	 * @param buffer The index buffer to be bound.
	 */
	void bindIndexBuffer(const Buffer& buffer);

	/**
	 * @brief Draws indexed primitives during rendering commands.
	 *
	 * @param count The number of indices to draw.
	 */
	void drawIndexed(u32 count);

	/**
	 * @brief Destructor for the FrameContext class.
	 *
	 * When destroyed, the frame is submitted to the SwapChain for rendering, even in case
	 * something unexpected occurs.
	 */
	~FrameContext();

	friend class Renderer;
private:
	FrameContext(SwapChain& swapChain, u32 currentFrame, bool swapChainRecreated);

	SwapChain* const m_SwapChain;
	u32 m_CurrentFrame;
	u32 m_ImageIndex;
	CommandBuffer* const m_CommandBuffer;
	u32 m_ImageCount;

	bool m_SwapChainRecreated;
};

} // namespace vulture