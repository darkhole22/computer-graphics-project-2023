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
		auto frontDir = glm::normalize(m_Transform->getRotation() * glm::vec3(1.0f, 0.0f, 0.0f));
		auto upDir = glm::normalize(m_Transform->getRotation() * glm::vec3(0.0f, 1.0f, 0.0f));
		auto rightDir = glm::normalize(glm::cross(frontDir, upDir));

		// m_Transform->translate(frontDir * movement.x + upDir * movement.y + rightDir * movement.z);
		m_Transform->translate(m_Transform->getRotation() * movement);
	}

	/**
	 * @brief Translates the object by a given amount.
	 *
	 * @param x The amount to translate the object in the x direction.
	 * @param y The amount to translate the object in the y direction.
	 * @param z The amount to translate the object in the z direction.
	 */
	inline void move(f32 x, f32 y, f32 z) { move(glm::vec3(x, y, z)); }

	inline void lookAt(glm::vec3 target)
	{
		auto a = glm::normalize(target - m_Transform->getPosition());
		auto b = glm::normalize(m_Transform->getRotation() * glm::vec3(1.0f, 0.0f, 0.0f));
		
		f32 angle = glm::acos(glm::dot(a, b));
		if (angle > 0.01f)
		{
			auto v = glm::normalize(glm::cross(b, a));

			glm::quat q = glm::quat(glm::cos(angle / 2), v * glm::sin(angle / 2));

			// m_Transform->setRotation(q);
			// m_Transform->setRotation(glm::inverse(m_Transform->getRotation()) * direction);
			m_Transform->rotate(q);
		}
		b = glm::normalize(m_Transform->getRotation() * glm::vec3(1.0f, 0.0f, 0.0f));
		auto res = a - b;

	}

private:
	Ref<Transform> m_Transform = nullptr;
};

} // namespace vulture