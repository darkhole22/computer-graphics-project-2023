#pragma once

#include "Renderer.h"

namespace vulture {

struct FrameInfo
{
	u32 index;
	u32 count;
};

class FrameContext
{
public:
	NO_COPY(FrameContext)

	inline FrameInfo getFrameInfo() const { return {m_ImageIndex, m_ImageCount}; }

	inline bool updated() const { return m_SwapChainRecreated; }
	inline const VkExtent2D& getExtent() const { return m_SwapChain->getExtent(); }
	
	void beginCommandRecording();
	void endCommandRecording();

	void bindPipeline(const Pipeline& pipeline);
	void bindDescriptorSet(const Pipeline& pipeline, const DescriptorSet& descriptorSet, u32 set);
	void drawModel(const Model& model);
	void bindVertexBuffer(const Buffer& buffer);
	void bindIndexBuffer(const Buffer& buffer);
	void drawIndexed(u32 count);

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