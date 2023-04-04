#pragma once

#include "vulture/renderer/Renderer.h"

namespace vulture {

struct CameraBufferObject
{
	glm::mat4 view;
	glm::mat4 proj;
};

class Camera
{
public:
	Camera(const Renderer& renderer, DescriptorPool& descriptorsPool);

	inline DescriptorSetLayout* getDescriptorSetLayout() { return &m_DescriptorSetLayout; }
	inline const DescriptorSet& getDescriptorSet() { return *(m_DescriptorSet.lock()); }

private:
	Uniform<CameraBufferObject> m_Uniform;
	DescriptorSetLayout m_DescriptorSetLayout;
	std::weak_ptr<DescriptorSet> m_DescriptorSet;
};

} // namespace vulture
