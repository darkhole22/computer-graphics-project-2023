#pragma once

#include "vulture/util/Types.h"
#include "vulture/renderer/DescriptorSet.h"
#include "vulture/renderer/Renderer.h"
#include "Camera.h"
#include "vulture/renderer/FrameContext.h"

namespace vulture {

struct WorldBufferObject
{
	alignas(16) glm::vec3 lightDirection;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 cameraPosition;
};

struct DirectLight
{
	glm::vec3 direction;
	glm::vec4 color;
};

class World
{
public:
	explicit World(DescriptorPool& descriptorsPool);

	DirectLight directLight;

	~World() = default;

	friend class Scene;
private:
	Uniform<WorldBufferObject> m_Uniform;
	Ref<DescriptorSetLayout> m_DescriptorSetLayout;
	Ref<DescriptorSet> m_DescriptorSet;

	inline DescriptorSetLayout* getDescriptorSetLayout() { return m_DescriptorSetLayout.get(); }
	inline const DescriptorSet& getDescriptorSet() { return *m_DescriptorSet; }

	void updateUniforms(FrameContext& target, const Camera& camera);
};

} // vulture