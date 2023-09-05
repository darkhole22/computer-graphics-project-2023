#pragma once

#include "vulture/util/Types.h"
#include "vulture/renderer/DescriptorSet.h"
#include "vulture/renderer/Renderer.h"
#include "Camera.h"
#include "vulture/renderer/FrameContext.h"

namespace vulture {

struct WorldBufferObject
{
	alignas(16) glm::vec3 pointLightPosition;
	alignas(16) glm::vec4 pointLightColor;
	alignas(4) 	f32 pointLightDecay;
	alignas(4) 	f32 pointLightMaxRange;
	alignas(4) 	f32 ambientStrength = 0.005f;

	alignas(16) glm::vec3 directLightDirection;
	alignas(16) glm::vec4 directLightColor;

	alignas(16) glm::vec3 cameraPosition;
};

struct DirectLight
{
	glm::vec3 direction;
	glm::vec4 color;
};

struct PointLight
{
	glm::vec3 position = {0.0f, 0.0f, 0.0f};
	glm::vec4 color { 0.0f, 0.0f, 0.0f, 1.0f};
	f32 decay = 1.0f;
	f32 maxRange = 20.0f;
};

class World
{
public:
	explicit World(DescriptorPool& descriptorsPool);

	DirectLight directLight;
	PointLight pointLight;

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