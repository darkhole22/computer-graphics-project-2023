#include "Camera.h"

#include <chrono>

namespace vulture {

Camera::Camera(const Renderer& renderer, DescriptorPool& descriptorsPool) :
	m_Uniform(renderer.makeUniform<CameraBufferObject>()), m_DescriptorSetLayout(renderer.makeDescriptorSetLayout())
{
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->create();

	m_DescriptorSet = descriptorsPool.getDescriptorSet(*m_DescriptorSetLayout.get(), {m_Uniform});
	
	updateView();
	updateProjection();
}

void Camera::setSize(float size)
{
	if (size <= 0) return;

	m_Size = size;
	updateProjection();
}

void Camera::setFov(float fov)
{
	if (fov < 1 || fov > 179) return;

	m_Fov = glm::radians(fov);
	updateProjection();
}

void Camera::setNearPlane(float nearPlane)
{
	m_NearPlane = nearPlane;
	updateProjection();
}

void Camera::setFarPlane(float farPlane)
{
	m_FarPlane = farPlane;
	updateProjection();
}

void Camera::update(float dt)
{	
	updateView();
}

void Camera::updateProjection()
{
	if (m_Projection == Projection::PERSPECTIVE)
	{
		m_Uniform->proj = glm::perspective(m_Fov, m_AspectRatio, m_NearPlane, m_FarPlane);
		m_Uniform->proj[1][1] *= -1;
	}
	else
	{
		float hHeight = m_Size / 2;
		float hWidth = hHeight * m_AspectRatio;
		m_Uniform->proj = glm::ortho(-hWidth, hWidth, hHeight, -hHeight, m_NearPlane, m_FarPlane);
	}
}

void Camera::updateView()
{
	m_Uniform->view = glm::lookAt(this->position, this->position + this->direction, this->up);
}

} // namespace vulture
