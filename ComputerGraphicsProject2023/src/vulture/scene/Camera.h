#pragma once

#include "vulture/renderer/Renderer.h"
#include "vulture/core/Logger.h"

namespace vulture {

struct CameraBufferObject
{
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

/*
* @brief The Camera class represents a virtual camera in a 3D scene.
* It holds information about the camera's position, orientation, and projection settings.
* The Camera class is used by the Scene class to render the scene from the camera's perspective.
*/
class Camera
{
public:
	/*
	* @brief A vec3 that holds the current camera position.
	*/
	glm::vec3 position = glm::vec3(0, 0, 0);

	/*
	* @brief A vec3 that holds the current camera looking direction angles.
	*/
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);

	/*
	* @brief A vec3 that holds the up direction.
	*/
	glm::vec3 up = glm::vec3(0, 1, 0);

	/*
	* @brief Maximum vertical angle the camera can be rotated by.
	*/
	float maxVerticalAngle = glm::radians(85.0f);

	/*
	* @brief Translates the camera relatively to its look-direction.
	*
	* @param translation: translation amount over the three (relative) axes.
	*/
	void translate(glm::vec3 translation);

	/*
	* @brief Rotates the camera.
	*
	* @param rotate: rotation angles around the three axes.
	*/
	void rotate(glm::vec3 rotation);

	enum class Projection
	{
		ORTHOGONAL,
		PERSPECTIVE
	};

	/*
	* @brief Returns the current projection mode of the camera.
	* 
	* @returns The current projection mode.
	*/
	inline Projection getProjectionMode() const { return m_Projection; }

	/*
	* @brief Sets the projection mode of the camera to either orthogonal or perspective.
	* 
	* @param mode: The new projection mode.
	*/
	inline void setProjectionMode(Projection mode)
	{
		m_Projection = mode;
	}

	/*
	* @brief Sets the direction of the camera to look at a specific point in 3D space.
	* 
	* @param target: The target point.
	*/
	inline void lookAt(const glm::vec3& target)
	{
		auto dir = glm::normalize(position - target);
		direction.x = glm::atan(dir.x, dir.z);
		direction.y = glm::asin(dir.y);
	}

	/*
	* @brief Returns the size of the camera's projection in the x-direction for an orthogonal projection.
	* 
	* @returns The current size of the camera's projection plane in world units.
	*/
	inline float getSize() const { return m_Size; }

	/*
	* @brief Sets the size of the camera's projection in the x-direction for an orthogonal projection.
	* For a perspective projection, this function has no effect.
	* 
	* @param size: The new size of the camera's projection plane in world units.
	*/
	void setSize(float size);

	/*
	* @brief Returns the vertical field of view (vFOV) of the camera's projection.
	* 
	* @returns The current vFOV.
	*/
	inline float getFov() const { return m_Fov; }

	/*
	* @brief Sets the vertical field of view (vFOV) of the camera's projection.
	* 
	* @param fov: The new vFOV.
	*/
	void setFov(float fov);

	/*
	* @brief Returns the distance to the camera's near plane.
	* 
	* @returns The distance to the near plane.
	*/
	inline float getNearPlane() const { return m_NearPlane; }

	/*
	* @brief Sets the distance to the camera's near plane.
	* 
	* @param nearPlane: The new distance to the near plane.
	*/
	void setNearPlane(float nearPlane);

	/*
	* @brief Returns the distance to the camera's far plane.
	* 
	* @returns The distance to the far plane.
	*/
	inline float getFarPlane() const { return m_FarPlane; }

	/*
	* @brief Sets the distance to the camera's far plane.
	* 
	* @param farPlane: The new distance to the far plane.
	*/
	void setFarPlane(float farPlane);

	inline glm::mat4 getProjectionMatrix() const { return m_Uniform->proj; }

	inline glm::mat4 getViewMatrix() const { return m_Uniform->view; }

	friend class Scene;
private:
	Uniform<CameraBufferObject> m_Uniform;
	Ref<DescriptorSetLayout> m_DescriptorSetLayout;
	Ref<DescriptorSet> m_DescriptorSet;

	Camera(DescriptorPool& descriptorsPool);

	// Projection member variables
	Projection m_Projection = Projection::PERSPECTIVE;
	float m_Fov = glm::radians(45.0f);
	float m_Size = 5.0f;
	float m_NearPlane = 0.05f;
	float m_FarPlane = 50.0f;
	float m_AspectRatio = 1.0f;

	// Functions used by the scene
	void update(float dt);
	inline DescriptorSetLayout* getDescriptorSetLayout() { return m_DescriptorSetLayout.get(); }
	inline const DescriptorSet& getDescriptorSet() { return *m_DescriptorSet; }
	inline void map(uint32_t index) { m_DescriptorSet->map(index); }

	// Functions for internal use
	void updateProjection();
	void updateView();
};

} // namespace vulture
