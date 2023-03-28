#pragma once

#include "Renderer.h"

namespace computergraphicsproject {

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

	inline bool updated() const { return m_SwapChainRecreated; };
	
	void beginCommandRecording();
	void endCommandRecording();

	void bindPipeline(const Pipeline& pipeline);

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

}