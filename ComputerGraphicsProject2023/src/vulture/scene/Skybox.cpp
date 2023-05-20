#include "Skybox.h"

namespace vulture {

VertexLayout Skybox::s_VertexLayout = VertexLayout(sizeof(SkyboxVertex),
												   { {VK_FORMAT_R32G32B32_SFLOAT, static_cast<u32>(offsetof(SkyboxVertex, position))} });

Skybox::Skybox(DescriptorPool& descriptorsPool) :
	m_DescriptorPool(&descriptorsPool)
{
	m_DSLayout = makeRef<DescriptorSetLayout>();
	m_DSLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DSLayout->addBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	m_DSLayout->create();

	PipelineAdvancedConfig config{};
	config.useDepthTesting = false;

	m_Pipeline = Ref<Pipeline>(
		new Pipeline(Renderer::getRenderPass(),
		"res/shaders/Skybox_vert.spv", "res/shaders/Skybox_frag.spv",
		{ m_DSLayout.get() },
		s_VertexLayout,
		config)
	);

	m_Uniform = Renderer::makeUniform<SkyboxBufferObject>();

	constexpr u32 vertexCount = 8;

	SkyboxVertex vertices[vertexCount] = {
		glm::vec3{-1.0f, 1.0f,-1.0f},
		glm::vec3{ 1.0f, 1.0f,-1.0f},
		glm::vec3{ 1.0f, 1.0f, 1.0f},
		glm::vec3{-1.0f, 1.0f, 1.0f},
		glm::vec3{-1.0f,-1.0f,-1.0f},
		glm::vec3{ 1.0f,-1.0f,-1.0f},
		glm::vec3{ 1.0f,-1.0f, 1.0f},
		glm::vec3{-1.0f,-1.0f, 1.0f}
	};

	u32 indices[c_IndexCount] = {
		0, 1, 3,
		1, 2, 3,

		4, 5, 0,
		5, 1, 0,

		5, 6, 1,
		1, 6, 2,

		3, 2, 6,
		3, 6, 7,

		0, 7, 4,
		0, 3, 7,

		7, 6, 5,
		7, 5, 4
	};

	VkDeviceSize vertexBufferSize = sizeof(SkyboxVertex) * vertexCount;
	Buffer vertexStagingBuffer = Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vertexStagingBuffer.map(vertices);
	m_VertexBuffer = Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vertexStagingBuffer.copyToBuffer(vertexBufferSize, m_VertexBuffer);

	VkDeviceSize indexBufferSize = sizeof(u32) * c_IndexCount;
	Buffer indexStagingBuffer = Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	indexStagingBuffer.map(indices);
	m_IndexBuffer = Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	indexStagingBuffer.copyToBuffer(indexBufferSize, m_IndexBuffer);
}

void Skybox::set(const String& name)
{
	if (m_CurrentName != name)
	{
		if (name.isEmpty())
		{
			m_DescriptorSet.reset();
		}
		else
		{
			Texture::getCubemapAsync(name, [this](Ref<Texture> texture) {
				m_Texture = texture;
				m_TextureSampler = makeRef<TextureSampler>(*m_Texture);
				m_DescriptorSet.reset();
				m_DescriptorSet = m_DescriptorPool->getDescriptorSet(*m_DSLayout, { m_Uniform, *m_TextureSampler });
				emit(SkyboxRecreated{});
			});
		}
		m_CurrentName = name;
		emit(SkyboxRecreated{});
	}
}

void Skybox::recordCommandBuffer(FrameContext& target)
{
	if (m_DescriptorSet)
	{
		target.bindPipeline(*m_Pipeline);
		target.bindDescriptorSet(*m_Pipeline, *m_DescriptorSet, 0);
		target.bindVertexBuffer(m_VertexBuffer);
		target.bindIndexBuffer(m_IndexBuffer);
		target.drawIndexed(c_IndexCount);
	}
}

void Skybox::updateUniforms(FrameContext& target, const Camera& camera)
{
	auto [index, count] = target.getFrameInfo();

	if (m_DescriptorSet)
	{
		m_Uniform->proj = camera.getProjectionMatrix();
		m_Uniform->view = camera.getViewMatrix();
		m_Uniform->view[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		m_DescriptorSet->map(index);
	}
}

} // namespace vulture
