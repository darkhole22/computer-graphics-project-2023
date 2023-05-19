#include "Camera.h"

#include <algorithm> // std::clamp

namespace vulture {

Camera::Camera(DescriptorPool& descriptorsPool) :
	m_Uniform(Renderer::makeUniform<CameraBufferObject>()), m_DescriptorSetLayout(makeRef<DescriptorSetLayout>())
{
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->create();

	m_DescriptorSet = descriptorsPool.getDescriptorSet(*m_DescriptorSetLayout.get(), {m_Uniform});
	
	updateView();
	updateProjection();
}

void Camera::translate(glm::vec3 translation)
{
	auto ux = glm::vec3(
			glm::rotate(glm::mat4(1.0f), direction.x, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
	);

	auto uz = glm::vec3(
			glm::rotate(glm::mat4(1.0f), direction.x, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)
	);

	position += (ux * translation.x + up * translation.y +  uz * translation.z);
}

void Camera::rotate(glm::vec3 rotation)
{
	direction.x += rotation.x;
	float sgn = (direction.x >= 0.0f) ? 1.0f : -1.0f;
	float val = std::abs(direction.x);
	if (val > 3.14)
	{
		direction.x = -sgn * (2 * 3.14f  - val);
	}

	direction.y = std::clamp(direction.y + rotation.y, -maxVerticalAngle, maxVerticalAngle);
	direction.z += rotation.z;
}

void Camera::setSize(float size)
{
	if (size <= 0) return;

	m_Size = size;
}

void Camera::setFov(float fov)
{
	if (fov < 1 || fov > 179) return;

	m_Fov = glm::radians(fov);
}

void Camera::setNearPlane(float nearPlane)
{
	m_NearPlane = nearPlane;
}

void Camera::setFarPlane(float farPlane)
{
	m_FarPlane = farPlane;
}

void Camera::update(float dt)
{	
	updateView();
	updateProjection();
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
	m_Uniform->view =
			glm::rotate(glm::mat4(1.0f), -direction.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::rotate(glm::mat4(1.0f), -direction.y, glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), -direction.x, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::translate(glm::mat4(1.0f), -position);
}

} // namespace vulture
