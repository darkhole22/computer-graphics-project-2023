#pragma once

#include "vulture/renderer/Renderer.h"

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
	glm::vec3 position;

	/*
	* @brief A vec3 that holds the current camera looking direction.
	*/
	glm::vec3 direction;

	/*
	* @brief A vec3 that holds the up direction.
	*/
	glm::vec3 up;

	/*
	* @brief A float that holds the roll rotation.
	*/
	f32 roll;

	/*
	* @brief Maximum vertical angle the camera can be rotated by.
	*/
	f32 maxVerticalAngle = glm::radians(85.0f);

	/*
	* @brief Maximum vertical angle the camera can be rotated by.
	*/
	f32 maxRollAngle = glm::radians(45.0f);

	/*
	* @brief Translates the camera relatively to its look-direction.
	*
	* @param translation: translation amount over the three (relative) axes.
	*/
	void translate(glm::vec3 translation);

	/*
	* @brief Translates the camera relatively to its look-direction.
	*
	* @param x: translation amount over the x (relative) axis.
	* @param y: translation amount over the y (relative) axis.
	* @param z: translation amount over the z (relative) axis.
	*/
	inline void translate(f32 x, f32 y, f32 z) { translate(glm::vec3(x, y, z)); };

	/*
	* @brief Rotates the camera.
	*
	* @param rotate: rotation angles around the three axes.
	*/
	void rotate(glm::vec3 rotation);

	/*
	* @brief Rotates the camera.
	*
	* @param x: rotation amount over the x axis.
	* @param y: rotation amount over the y axis.
	* @param z: rotation amount over the z axis.
	*/
	inline void rotate(f32 x, f32 y, f32 z) { rotate(glm::vec3(x, y, z)); };

	/*
	* @brief Rolls the camera without changing the up direction.
	*
	* @param rotation: angles around the front axis.
	*/
	void addRoll(f32 rotation);

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
	inline void setProjectionMode(Projection mode) { m_Projection = mode; }

	/*
	* @brief Sets the direction of the camera to look at a specific point in 3D space.
	* 
	* @param target: The target point.
	*/
	inline void lookAt(const glm::vec3& target) { this->direction = target - this->position; }

	/*
	* @brief Returns the size of the camera's projection in the x-direction for an orthogonal projection.
	* 
	* @returns The current size of the camera's projection plane in world units.
	*/
	inline f32 getSize() const { return m_Size; }

	/*
	* @brief Sets the size of the camera's projection in the x-direction for an orthogonal projection.
	* For a perspective projection, this function has no effect.
	* 
	* @param size: The new size of the camera's projection plane in world units.
	*/
	void setSize(f32 size);

	/*
	* @brief Returns the vertical field of view (vFOV) of the camera's projection.
	* 
	* @returns The current vFOV.
	*/
	inline f32 getFov() const { return m_Fov; }

	/*
	* @brief Sets the vertical field of view (vFOV) of the camera's projection.
	* 
	* @param fov: The new vFOV.
	*/
	void setFov(f32 fov);

	/*
	* @brief Returns the distance to the camera's near plane.
	* 
	* @returns The distance to the near plane.
	*/
	inline f32 getNearPlane() const { return m_NearPlane; }

	/*
	* @brief Sets the distance to the camera's near plane.
	* 
	* @param nearPlane: The new distance to the near plane.
	*/
	void setNearPlane(f32 nearPlane);

	/*
	* @brief Returns the distance to the camera's far plane.
	* 
	* @returns The distance to the far plane.
	*/
	inline f32 getFarPlane() const { return m_FarPlane; }

	/*
	* @brief Sets the distance to the camera's far plane.
	* 
	* @param farPlane: The new distance to the far plane.
	*/
	void setFarPlane(f32 farPlane);

	inline glm::mat4 getProjectionMatrix() const { return m_Uniform->proj; }

	inline glm::mat4 getViewMatrix() const { return m_Uniform->view; }

	/*
	* @brief Resets the camera transform to its initial state.
	*/
	void reset();

	friend class Scene;
private:
	Uniform<CameraBufferObject> m_Uniform;
	Ref<DescriptorSetLayout> m_DescriptorSetLayout;
	Ref<DescriptorSet> m_DescriptorSet;

	Camera(DescriptorPool& descriptorsPool);

	// Projection member variables
	Projection m_Projection = Projection::PERSPECTIVE;
	f32 m_Fov = glm::radians(45.0f);
	f32 m_Size = 5.0f;
	f32 m_NearPlane = 0.05f;
	f32 m_FarPlane = 50.0f;
	f32 m_AspectRatio = 1.0f;

	// Initial Transform
	const glm::vec3 c_InitialPosition = { 0.0f, 0.0f, 0.0f };
	const glm::vec3 c_InitialDirection = { 1.0f, 0.0f, 0.0f };
	const glm::vec3 c_InitialUpDirection = { 0.0f, 1.0f, 0.0f };
	const f32 c_InitialRoll = 0.0f;

	// Functions used by the scene
	void update(f32 dt);
	inline DescriptorSetLayout* getDescriptorSetLayout() { return m_DescriptorSetLayout.get(); }
	inline const DescriptorSet& getDescriptorSet() { return *m_DescriptorSet; }
	inline void map(uint32_t index) { m_DescriptorSet->map(index); }

	// Functions for internal use
	void updateProjection();
	void updateView();
};

} // namespace vulture
