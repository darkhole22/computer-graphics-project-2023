#include "Pipeline.h"

#include "vulture/core/Logger.h"
#include "VulkanContext.h"

#include <fstream>

namespace vulture {

extern VulkanContextData vulkanData;

const PipelineAdvancedConfig PipelineAdvancedConfig::defaultConfig = PipelineAdvancedConfig{};

VertexLayout::VertexLayout(u32 size, const std::vector<std::pair<VkFormat, u32>>& descriptors)
{
	m_Bindings.binding = 0;
	m_Bindings.stride = size;
	m_Bindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	u32 location = 0;
	m_Attributes.reserve(descriptors.size());
	for (auto& [format, offset] : descriptors)
	{
		VkVertexInputAttributeDescription descriptor{};
		descriptor.binding = 0;
		descriptor.location = location++;
		descriptor.format = format;
		descriptor.offset = offset;

		m_Attributes.push_back(descriptor);
	}
}

Shader::Shader(const String& name)
{
	std::ifstream file(name.cString(), std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		VUERROR("Failed to open file [%s]!", name.cString());
		throw std::runtime_error("Failed to open file!");
	}

	auto fileSize = file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

	file.close();

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = buffer.size();
	createInfo.pCode = reinterpret_cast<const u32*>(buffer.data());

	VkResult result = vkCreateShaderModule(vulkanData.device, &createInfo, vulkanData.allocator, &m_Handle);

	if (result != VK_SUCCESS)
	{
		VUERROR("Failed to create shader module!");
	}
}

VkPipelineShaderStageCreateInfo Shader::getStage(VkShaderStageFlagBits stageType, const char* mainName) const
{
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = stageType;
	vertShaderStageInfo.module = m_Handle;
	vertShaderStageInfo.pName = mainName;

	return vertShaderStageInfo;
}

Shader::~Shader()
{
	vkDestroyShaderModule(vulkanData.device, m_Handle, vulkanData.allocator);
}

Pipeline::Pipeline(
	const RenderPass& renderPass,
	const String& vertexShader,
	const String& fragmentShader,
	const std::vector<DescriptorSetLayout*>& descriptorSetLayouts,
	const VertexLayout& vertexLayout,
	const PipelineAdvancedConfig& config)
{
	Shader vertShader(vertexShader);
	Shader fragShader(fragmentShader);

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShader.getStage(VK_SHADER_STAGE_VERTEX_BIT),
		fragShader.getStage(VK_SHADER_STAGE_FRAGMENT_BIT) };

	auto& attributeDescriptions = vertexLayout.getAttributes();
	auto vertexBindingDescriptions = vertexLayout.getBinding();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDescriptions;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = config.cullMode;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f;		   // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f;	   // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vulkanData.msaaSamples;
	multisampling.minSampleShading = 1.0f;			// Optional
	multisampling.pSampleMask = nullptr;			// Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE;		// Optional


	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	// blendEnable controls whether blending is enabled for the corresponding color attachment.
	// If blending is not enabled, the source fragment’s color for that attachment is passed through unmodified.
	if (config.useAlpha)
	{
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	}
	else
	{
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	}

	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;			 // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;	 // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;			 // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	std::vector<VkDescriptorSetLayout> dsls(descriptorSetLayouts.size());
	for (size_t i = 0; i < dsls.size(); i++)
	{
		dsls[i] = descriptorSetLayouts[i]->getHandle();
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(dsls.size());
	pipelineLayoutInfo.pSetLayouts = dsls.data();
	pipelineLayoutInfo.pushConstantRangeCount = 0;	  // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	// ASSERT_VK_SUCCESS(
	VkResult result = vkCreatePipelineLayout(vulkanData.device, &pipelineLayoutInfo, vulkanData.allocator, &m_Layout);

	if (result != VK_SUCCESS)
	{
		VUERROR("Failed to create pipeline layout!");
		return;
	}

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	VkBool32 useDepth = config.useDepthTesting ? VK_TRUE : VK_FALSE;
	depthStencil.depthTestEnable = useDepth;
	depthStencil.depthWriteEnable = useDepth;
	depthStencil.depthCompareOp = config.compareOperator;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {};	 // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_Layout;
	pipelineInfo.renderPass = renderPass.getHandle();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1;			  // Optional

	result = vkCreateGraphicsPipelines(vulkanData.device, VK_NULL_HANDLE, 1, &pipelineInfo, vulkanData.allocator, &m_Handle);

	if (result != VK_SUCCESS)
	{
		VUERROR("Failed to create graphics pipeline!");
		return;
	}
}

Pipeline::~Pipeline()
{
	vkDestroyPipeline(vulkanData.device, m_Handle, vulkanData.allocator);
	vkDestroyPipelineLayout(vulkanData.device, m_Layout, vulkanData.allocator);
}

} // namespace vulture
