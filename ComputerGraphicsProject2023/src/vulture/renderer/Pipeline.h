#pragma once

#include "vulture/core/Core.h"
#include "RenderPass.h"
#include "DescriptorSet.h"

namespace vulture {

class VertexLayout
{
public:
	NO_COPY(VertexLayout)

	VertexLayout(u32 size, const std::vector<std::pair<VkFormat, u32>>& descriptors);

	inline VkVertexInputBindingDescription getBinding() const { return m_Bindings; }
	inline const std::vector<VkVertexInputAttributeDescription>& getAttributes() const { return m_Attributes; }
private:
	VkVertexInputBindingDescription m_Bindings;
	std::vector<VkVertexInputAttributeDescription> m_Attributes;
};

class Shader
{
public:
	NO_COPY(Shader)

	Shader(const String& name);

	VkPipelineShaderStageCreateInfo getStage(VkShaderStageFlagBits stageType, const char* mainName = "main") const;

	~Shader();
private:
	VkShaderModule m_Handle;
};

struct PipelineAdvancedConfig
{
	VkCompareOp compareOperator = VK_COMPARE_OP_LESS;
	bool useAlpha = false;

	static const PipelineAdvancedConfig defaultConfig;
};

class Pipeline
{
public:
	NO_COPY(Pipeline)

	Pipeline(const RenderPass& renderPass, const String& vertexShader, const String& fragmentShader,
		const std::vector<DescriptorSetLayout*>& descriptorSetLayouts, const VertexLayout& vertexLayout,
		const PipelineAdvancedConfig& config = PipelineAdvancedConfig::defaultConfig);

	inline VkPipeline getHandle() const { return m_Handle; }
	inline VkPipelineLayout getLayout() const { return m_Layout; }

	~Pipeline();
private:
	VkPipeline m_Handle = VK_NULL_HANDLE;
	VkPipelineLayout m_Layout = VK_NULL_HANDLE;
};

} // namespace vulture
