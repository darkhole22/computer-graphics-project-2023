#pragma once

#include "vulture/core/Core.h"
#include "RenderPass.h"
#include "DescriptorSet.h"

namespace vulture {

/**
* @class VertexLayout
*
* @brief Represents the vertex layout used in a rendering pipeline.
*/
class VertexLayout
{
public:
	NO_COPY(VertexLayout)


	/**
	 * @brief Constructor for the VertexLayout class.
	 *
	 * @param size The size of the vertex layout.
	 * @param descriptors A vector of pairs representing the format and size of the vertex attributes.
	 */
	VertexLayout(u32 size, const std::vector<std::pair<VkFormat, u32>>& descriptors);

	/**
	 * @brief Gets the vertex input binding description used in the vertex layout.
	 *
	 * @return The vertex input binding description.
	 */
	inline VkVertexInputBindingDescription getBinding() const { return m_Bindings; }

	/**
	 * @brief Gets the vertex input attribute descriptions used in the vertex layout.
	 *
	 * @return A vector of vertex input attribute descriptions.
	 */
	inline const std::vector<VkVertexInputAttributeDescription>& getAttributes() const { return m_Attributes; }
private:
	VkVertexInputBindingDescription m_Bindings;
	std::vector<VkVertexInputAttributeDescription> m_Attributes;
};

/**
 * @class Shader
 *
 * @brief Represents a Vulkan shader module used in a rendering pipeline.
 */
class Shader
{
public:
	NO_COPY(Shader)

	/**
	 * @brief Constructor for the Shader class.
	 *
	 * @param name The name of the shader.
	 */
	Shader(const String& name);

	/**
	 * @brief Gets the pipeline shader stage create info for a specific shader stage.
	 *
	 * @param stageType The shader stage type (e.g., vertex, fragment).
	 * @param mainName The name of the main function in the shader (default is "main").
	 * @return The pipeline shader stage create info.
	 */
	VkPipelineShaderStageCreateInfo getStage(VkShaderStageFlagBits stageType, const char* mainName = "main") const;

	~Shader();
private:
	VkShaderModule m_Handle;
};

/**
 * @struct PipelineAdvancedConfig
 *
 * @brief Contains advanced configuration options for a rendering pipeline.
 */
struct PipelineAdvancedConfig
{
	VkCompareOp compareOperator = VK_COMPARE_OP_LESS;
	bool useAlpha = false;
	bool useDepthTesting = true;

	static const PipelineAdvancedConfig defaultConfig;
};

/**
 * @class Pipeline
 *
 * @brief Represents a Vulkan rendering pipeline.
 */
class Pipeline
{
public:
	NO_COPY(Pipeline)

	/**
	 * @brief Constructor for the Pipeline class.
	 *
	 * @param renderPass The RenderPass associated with the pipeline.
	 * @param vertexShader The name of the vertex shader used in the pipeline.
	 * @param fragmentShader The name of the fragment shader used in the pipeline.
	 * @param descriptorSetLayouts A vector of DescriptorSetLayout pointers used in the pipeline.
	 * @param vertexLayout The VertexLayout used in the pipeline.
	 * @param config Advanced configuration options for the pipeline (default is PipelineAdvancedConfig::defaultConfig).
	 */
	Pipeline(const RenderPass& renderPass, const String& vertexShader, const String& fragmentShader,
		const std::vector<DescriptorSetLayout*>& descriptorSetLayouts, const VertexLayout& vertexLayout,
		const PipelineAdvancedConfig& config = PipelineAdvancedConfig::defaultConfig);

	/**
	 * @brief Gets the Vulkan handle of the pipeline.
	 *
	 * @return The Vulkan handle of the pipeline.
	 */
	inline VkPipeline getHandle() const { return m_Handle; }

	/**
	 * @brief Gets the Vulkan handle of the pipeline layout.
	 *
	 * @return The Vulkan handle of the pipeline layout.
	 */
	inline VkPipelineLayout getLayout() const { return m_Layout; }

	~Pipeline();
private:
	VkPipeline m_Handle = VK_NULL_HANDLE;
	VkPipelineLayout m_Layout = VK_NULL_HANDLE;
};

} // namespace vulture
