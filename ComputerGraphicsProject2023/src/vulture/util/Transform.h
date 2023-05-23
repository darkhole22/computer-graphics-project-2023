#pragma once

#include "Types.h"

namespace vulture {

class Transform
{
public:
	/**
	 * @brief Returns the position of the object.
	 *
	 * @return The position of the object as a glm::vec3.
	 */
	inline glm::vec3 getPosition() { return m_Position; }

	/**
	 * @brief Sets the position of the object.
	 *
	 * @param pos The new position of the object as a glm::vec3.
	 */
	inline void setPosition(glm::vec3 pos)
	{
		translate(pos - m_Position);
		m_ShouldUpdate = true;
	}

	/**
	 * @brief Sets the position of the object.
	 *
	 * @param x The x-coordinate of the new position.
	 * @param y The y-coordinate of the new position.
	 * @param z The z-coordinate of the new position.
	 */
	inline void setPosition(f32 x, f32 y, f32 z) { setPosition(glm::vec3(x, y, z)); }

	/**
	 * @brief Translates the object by a given amount.
	 *
	 * @param translation The amount to translate the object by as a glm::vec3.
	 */
	void translate(glm::vec3 translation) {
		// m_Position += translation;
		auto frontDir = glm::normalize(m_Rotation * m_FacingDirection);
		auto rightDir = glm::normalize(glm::cross(frontDir, m_UpDirection));

		m_Position += frontDir * translation.x + m_UpDirection * translation.y + rightDir * translation.z;
		m_ShouldUpdate = true;
	}

	/**
	 * @brief Translates the object by a given amount.
	 *
	 * @param x The amount to translate the object in the x direction.
	 * @param y The amount to translate the object in the y direction.
	 * @param z The amount to translate the object in the z direction.
	 */
	inline void translate(f32 x, f32 y, f32 z) { translate(glm::vec3(x, y, z)); }

	/**
	 * @brief Returns the scale of the object.
	 *
	 * @return The scale of the object as a glm::vec3.
	 */
	inline glm::vec3 getScale() { return m_Scale; }

	/**
	 * @brief Sets the scale of the object.
	 *
	 * @param scale The new scale of the object as a glm::vec3.
	 */
	inline void setScale(glm::vec3 scale) { m_Scale = scale; m_ShouldUpdate = true; }

	/**
	 * @brief Sets the scale of the object.
	 *
	 * @param x The new scale in the x direction.
	 * @param y The new scale in the y direction.
	 * @param z The new scale in the z direction.
	 */
	inline void setScale(f32 x, f32 y, f32 z) { setScale(glm::vec3(x, y, z)); }

	/**
	 * @brief Sets the scale of the object.
	 *
	 * @param scale The new scale in all directions direction.
	 */
	inline void setScale(f32 scale) { setScale(glm::vec3(scale, scale, scale)); }

	/**
	 * @brief Returns the rotation of the object as a quaternion.
	 *
	 * @return The rotation of the object as a glm::quat.
	 */
	inline glm::quat getRotation() { return m_Rotation; }

	/**
	 * @brief Sets the rotation of the object using a rotation vector.
	 *
	 * @param rot The rotation vector.
	 */
	inline void setRotation(glm::vec3 rot) { m_Rotation = glm::quat(rot); m_ShouldUpdate = true; }

	/**
	 * @brief Sets the rotation of the object using Euler angles.
	 *
	 * @param x The rotation angle around the x-axis.
	 * @param y The rotation angle around the y-axis.
	 * @param z The rotation angle around the z-axis.
	 */
	inline void setRotation(f32 x, f32 y, f32 z) { setRotation(glm::vec3(x, y, z)); }

	/**
	 * @brief Rotates the object by the specified angles around the x, y, and z axes, respectively.
	 *
	 * @param rotation The rotation vector around the three axes.
	 */
	inline void rotate(glm::vec3 rotation) { m_Rotation *= glm::quat(rotation); m_ShouldUpdate = true; }

	/**
	 * @brief Rotates the object by the specified angles around the x, y, and z axes, respectively.
	 *
	 * @param x The angle to rotate around the x-axis.
	 * @param y The angle to rotate around the y-axis.
	 * @param z The angle to rotate around the z-axis.
	 */
	inline void rotate(f32 x, f32 y, f32 z) { rotate(glm::vec3(x, y, z)); }

	/**
	 * @brief Computes the World Matrix representing the current transform.
	 *
	 * @return The world matrix of the object.
	 */
	glm::mat4 getWorldMatrix()
	{
		if (m_ShouldUpdate)
		{
			m_WorldMatrix = glm::translate(glm::mat4(1.0f), m_Position) *
							glm::mat4(m_Rotation) *
							glm::scale(glm::mat4(1.0f), m_Scale);

			m_ShouldUpdate = false;
		}

		return m_WorldMatrix;
	}
private:
	glm::vec3 m_Position = glm::vec3(0.0f);
	glm::quat m_Rotation = glm::quat(glm::vec3(0.0f));
	glm::vec3 m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);

	bool m_ShouldUpdate = true;

	glm::mat4 m_WorldMatrix;

	glm::vec3 m_FacingDirection = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 m_UpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
};

} // namespace vulture