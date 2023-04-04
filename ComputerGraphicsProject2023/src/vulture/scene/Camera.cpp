#include "Camera.h"

#include <chrono>

namespace vulture {

Camera::Camera(const Renderer& renderer, DescriptorPool& descriptorsPool) :
	m_Uniform(renderer.makeUniform<CameraBufferObject>()), m_DescriptorSetLayout(renderer.makeDescriptorSetLayout())
{
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->create();

	m_DescriptorSet = descriptorsPool.getDescriptorSet(*m_DescriptorSetLayout.get(), {m_Uniform});
	
	glm::mat3 camDir = glm::mat3(1.0f);
	glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 1.0f);

	m_Uniform->view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_Uniform->proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	m_Uniform->proj[1][1] *= -1;
}

void Camera::update()
{
	
		static auto startTime = std::chrono::high_resolution_clock::now();
		static float lastTime = 0.0f;

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		float deltaT = time - lastTime;
		lastTime = time;

		glm::mat3 camDir = glm::mat3(1.0f);
		glm::vec3 camPos = glm::vec3(cos(time * 0.1f), 0.0f, sin(time * 0.1f));

		m_Uniform->view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_Uniform->proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
		m_Uniform->proj[1][1] *= -1;
	
}

} // namespace vulture
