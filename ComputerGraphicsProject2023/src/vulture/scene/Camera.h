#pragma once

#include "vulture/renderer/Renderer.h"

namespace vulture {

struct CameraBufferObject
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class Camera
{
public:
	Camera(const Renderer& renderer, DescriptorPool& descriptorsPool);

	inline DescriptorSetLayout* getDescriptorSetLayout() { return m_DescriptorSetLayout.get(); }
	inline const DescriptorSet& getDescriptorSet() { return *m_DescriptorSet.lock(); }

	void update();

	inline void map(uint32_t index) { m_Uniform.map(index); }
private:
	Uniform<CameraBufferObject> m_Uniform;
	Ref<DescriptorSetLayout> m_DescriptorSetLayout;
	std::weak_ptr<DescriptorSet> m_DescriptorSet;
};

} // namespace vulture
