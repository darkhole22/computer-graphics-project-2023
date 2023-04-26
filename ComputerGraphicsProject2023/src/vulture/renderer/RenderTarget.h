#pragma once

#include "Renderer.h"

namespace vulture {

struct FrameInfo
{
	uint32_t index;
	uint32_t count;
};

class RenderTarget
{
public:
	RenderTarget(const RenderTarget& other) = delete;
	RenderTarget(RenderTarget&& other) = delete;

	inline FrameInfo getFrameInfo() const { return {m_ImageIndex, m_ImageCount}; }

	inline bool updated() const { return m_SwapChainRecreated; }
	inline const VkExtent2D& getExtent() const { return m_SwapChain->getExtent(); }
	
	void beginCommandRecording();
	void endCommandRecording();

	void bindPipeline(const Pipeline& pipeline);
	void bindDescriptorSet(const Pipeline& pipeline, const DescriptorSet& descriptorSet, uint32_t set);
	void drawModel(const Model& model);
	void bindVertexBuffer(const Buffer& buffer);
	void bindIndexBuffer(const Buffer& buffer);
	void drawIndexed(uint32_t count);

	~RenderTarget();

	friend class Renderer;
private:
	RenderTarget(SwapChain& swapChain, const Device& device, uint32_t currentFrame, bool swapChainRecreated);

	SwapChain* const m_SwapChain;
	uint32_t m_CurrentFrame;
	uint32_t m_ImageIndex;
	CommandBuffer* const m_CommandBuffer;
	uint32_t m_ImageCount;

	bool m_SwapChainRecreated;
};

} // namespace vulture