#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include "vulture/renderer/Model.h"
#include "vulture/renderer/Renderer.h"
#include "vulture/core/Logger.h"


namespace vulture{

/**
 * ObjectHandle is a unique identifier used to track individual instances of GameObject in the Scene class. It is
 * a 64-bit integer type that is incremented for each new game object that is added to the scene. This allows the
 * Scene class to quickly and efficiently look up game objects by their handles.
 */
using ObjectHandle = int64_t;

/**
 * ModelBufferObject represents a uniform buffer object containing a single model matrix, which is used to transform
 * a 3D model in a shader program. The model matrix represents the current position, rotation, and scale of the object.
 * This struct is used by the GameObject class to update its model matrix before rendering.
 */
struct ModelBufferObject
{
	glm::mat4 model = glm::mat4(1.0f);
};

/**
 * Represents a game object in the scene. A game object is a container for
 * a 3D model and associated properties like position, rotation, and scale.
 */
class GameObject
{
public:
	/**
	 * @brief Constructs a new GameObject object.
	 *
	 * @param modelPath Path to the model file.
	 * @param texturePath Path to the texture file.
	 */
	GameObject(const std::string& modelPath, const std::string& texturePath);

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
	inline void setPosition(glm::vec3 pos) { m_Position = pos; }

	/**
	 * @brief Sets the position of the object.
	 *
	 * @param x The x-coordinate of the new position.
	 * @param y The y-coordinate of the new position.
	 * @param z The z-coordinate of the new position.
	 */
	inline void setPosition(float x, float y, float z) { setPosition(glm::vec3(x, y, z)); }

	/**
	 * @brief Translates the object by a given amount.
	 *
	 * @param translation The amount to translate the object by as a glm::vec3.
	 */
	inline void translate(glm::vec3 translation) { m_Position += translation; }

	/**
	 * @brief Translates the object by a given amount.
	 *
	 * @param x The amount to translate the object in the x direction.
	 * @param y The amount to translate the object in the y direction.
	 * @param z The amount to translate the object in the z direction.
	 */
	inline void translate(float x, float y, float z) { translate(glm::vec3(x, y, z)); }

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
	inline void setScale(glm::vec3 scale) { m_Scale = scale; }

	/**
	 * @brief Sets the scale of the object.
	 *
	 * @param x The new scale in the x direction.
	 * @param y The new scale in the y direction.
	 * @param z The new scale in the z direction.
	 */
	inline void setScale(float x, float y, float z) { setScale(glm::vec3(x, y, z)); }

	/**
	 * @brief Returns the rotation of the object as a quaternion.
	 *
	 * @return The rotation of the object as a glm::quat.
	 */
	inline glm::quat getRotation() { return m_Rotation; }

	/**
	 * @brief Sets the rotation of the game object using a rotation vector.
	 *
	 * @param rot The rotation vector.
	 */
	inline void setRotation(glm::vec3 rot) { m_Rotation = glm::quat(rot); }

	/**
	 * @brief Sets the rotation of the game object using Euler angles.
	 *
	 * @param x The rotation angle around the x-axis.
	 * @param y The rotation angle around the y-axis.
	 * @param z The rotation angle around the z-axis.
	 */
	inline void setRotation(float x, float y, float z) { setRotation(glm::vec3(x, y, z)); }

	/**
	 * @brief Rotates the game object by the specified angles around the x, y, and z axes, respectively.
	 *
	 * @param x The angle to rotate around the x-axis.
	 * @param y The angle to rotate around the y-axis.
	 * @param z The angle to rotate around the z-axis.
	 */
	inline void rotate(float x, float y, float z) { m_Rotation *= glm::quat(glm::vec3(x, y, z)); }

	friend class Scene;
private:
	static ObjectHandle s_NextHandle; 		// The next available handle for a new game object.

	Ref<Model> m_Model;
	Ref<Texture> m_Texture;
	Ref<TextureSampler> m_TextureSampler;
	Uniform<ModelBufferObject> m_Uniform;

	ObjectHandle m_Handle = -1;  			// The unique handle assigned to this game object.

	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::quat m_Rotation = glm::quat(glm::vec3(0.0f));

	/**
	 * Updates the ModelBufferObject associated with this game object by computing its current model matrix based on its
	 * position, rotation, and scale. This method is called by the Scene class on each game object during the update loop.
	 *
	 * @param dt The time elapsed since the last frame, in seconds.
	 */
	void update(float dt);
};

} // vulture