#pragma once

#include "vulture/util/Types.h"
#include "vulture/util/Transform.h"

using namespace vulture;

namespace game {

class MovementComponent
{
public:
	explicit MovementComponent(Ref<Transform> transform)
	{
		m_Transform = transform;
	}

	/**
	 * @brief Translates the object by a given amount.
	 *
	 * @param translation The amount to translate the object by as a glm::vec3.
	 */
	void move(glm::vec3 movement)
	{
		auto frontDir = glm::normalize(m_Transform->getRotation() * m_FacingDirection);
		auto rightDir = glm::normalize(glm::cross(frontDir, m_UpDirection));

		m_Transform->translate(frontDir * movement.x + m_UpDirection * movement.y + rightDir * movement.z);
	}

	/**
	 * @brief Translates the object by a given amount.
	 *
	 * @param x The amount to translate the object in the x direction.
	 * @param y The amount to translate the object in the y direction.
	 * @param z The amount to translate the object in the z direction.
	 */
	inline void move(f32 x, f32 y, f32 z) { move(glm::vec3(x, y, z)); }

private:
	Ref<Transform> m_Transform = nullptr;

	glm::vec3 m_FacingDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 m_UpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
};

} // namespace vulture