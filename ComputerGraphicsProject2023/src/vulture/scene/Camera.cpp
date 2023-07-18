#include "Camera.h"

#include <algorithm> // std::clamp

namespace vulture {

Camera::Camera(DescriptorPool& descriptorsPool) :
	m_Uniform(Renderer::makeUniform<CameraBufferObject>()), m_DescriptorSetLayout(makeRef<DescriptorSetLayout>())
{
	m_DescriptorSetLayout->addBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	m_DescriptorSetLayout->create();

	m_DescriptorSet = descriptorsPool.getDescriptorSet(m_DescriptorSetLayout, { m_Uniform });

	// Initialize transform
	reset();

	updateView();
	updateProjection();
}

void Camera::translate(glm::vec3 translation)
{
	up = glm::normalize(up);
	direction = glm::normalize(direction);
	glm::vec3 right = glm::normalize(glm::cross(direction, up));
	glm::vec3 front = glm::normalize(glm::cross(up, right));
	glm::mat3 transform = { right, up, front};

	position += transform * translation;
}

void Camera::rotate(glm::vec3 rotation)
{
	up = glm::normalize(up);
	direction = glm::normalize(direction);
	glm::vec3 right = glm::normalize(glm::cross(direction, up));
	glm::vec3 front = glm::normalize(glm::cross(up, right));

	f32 pitch = std::asin(glm::dot(glm::cross(front, direction), right));
	pitch = std::clamp(pitch + rotation.y, -maxVerticalAngle, maxVerticalAngle);

	direction = glm::vec3(
		glm::rotate(glm::mat4(1.0f), rotation.x, up) *
		glm::rotate(glm::mat4(1.0f), pitch, right) *
		glm::vec4(front, 1.0f));
	up = glm::vec3(glm::rotate(glm::mat4(1.0f), rotation.z, front) * glm::vec4(up, 1.0f));
}

void Camera::addRoll(f32 rotation)
{
	roll = std::clamp(roll + rotation, -maxRollAngle, maxRollAngle);
}

void Camera::setSize(f32 size)
{
	if (size <= 0) return;

	m_Size = size;
}

void Camera::setFov(f32 fov)
{
	if (fov < 1 || fov > 179) return;

	m_Fov = glm::radians(fov);
}

void Camera::setNearPlane(f32 nearPlane)
{
	m_NearPlane = nearPlane;
}

void Camera::setFarPlane(f32 farPlane)
{
	m_FarPlane = farPlane;
}

void Camera::update(f32 dt)
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
		f32 hHeight = m_Size / 2;
		f32 hWidth = hHeight * m_AspectRatio;
		m_Uniform->proj = glm::ortho(-hWidth, hWidth, hHeight, -hHeight, m_NearPlane, m_FarPlane);
	}
}

void Camera::updateView()
{
	m_Uniform->view = glm::rotate(glm::mat4(1.0f), roll, glm::vec3(0.0f, 0.0f, 1.0f)) *
		glm::lookAt(this->position, this->position + this->direction, this->up);
}

void Camera::reset()
{
	position = c_InitialPosition;
	direction = c_InitialDirection;
	up = c_InitialUpDirection;
	roll = c_InitialRoll;
}

} // namespace vulture
