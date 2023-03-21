#pragma once

#include "Renderer.h"

namespace computergraphicsproject {

class RenderTarget
{
public:
	RenderTarget(const RenderTarget& other) = delete;
	RenderTarget(RenderTarget&& other) = delete;

	inline uint32_t getCurrentFrame() const { return m_CurrentFrame; }
	
	void beginCommandRecording();
	void endCommandRecording();

	~RenderTarget();

	friend class Renderer;
private:
	RenderTarget(SwapChain& swapChain, const Device& divice, uint32_t currentFrame);

	SwapChain* const m_SwapChain;
	uint32_t m_CurrentFrame;
	uint32_t m_ImageIndex;
	CommandBuffer* const m_CommandBuffer;
};

}