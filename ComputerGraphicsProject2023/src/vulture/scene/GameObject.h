#pragma once

#include <string>
#include "vulture/renderer/Renderer.h"
#include "vulture/util/Transform.h"

namespace vulture{

/**
 * ObjectHandle is a unique identifier used to track individual instances of GameObject in the Scene class. It is
 * a 64-bit integer type that is incremented for each new game object that is added to the scene. This allows the
 * Scene class to quickly and efficiently look up game objects by their handles.
 */
using ObjectHandle = i64;


/**
 * ModelBufferObject represents a uniform buffer object containing a single model matrix, which is used to transform
 * a 3D model in a shader program. The model matrix represents the current position, rotation, and scale of the object.
 * This struct is used by the GameObject class to update its model matrix before rendering.
 */
struct ModelBufferObject
{
	glm::mat4 model = glm::mat4(1.0f);
};

struct ObjectBufferObject
{
	f32 emissionStrength = 0.0f;
};

#define DEFAULT_EMISSION_TEXTURE_NAME "default"
#define DEFAULT_ROUGHNESS_TEXTURE_NAME "default"

/**
 * Represents a game object in the scene. A game object is a container for
 * a 3D model and associated properties like position, rotation, and scale.
 */
class GameObject
{
public:
	static const String c_DefaultEmissionTextureName;
	static const String c_DefaultRoughnessTextureName;

	/**
	 * @brief Constructs a new GameObject object.
	 *
	 * @param modelName Name of the model.
	 * @param textureName Name of the texture.
	 */
	GameObject(
		const String &modelName,
		const String &baseTextureName,
		const String &emissionTextureName = c_DefaultEmissionTextureName,
		const String &roughnessTextureName = c_DefaultRoughnessTextureName
	);

	/**
	 * @brief Constructs a new GameObject object.
	 *
	 * @param name Name of both the model and of the texture.
	 */
	explicit GameObject(const String& name) : GameObject(name, name) {};

	inline void setEmissionStrength(f32 strength) { m_ObjectUniform->emissionStrength = strength; }

	Ref<Transform> transform;

	friend class Scene;
private:
	Ref<Model> m_Model;

	Ref<Texture> m_BaseTexture;
	Ref<TextureSampler> m_TextureSampler;

	Ref<Texture> m_EmissionTexture;
	Ref<TextureSampler> m_EmissionTextureSampler;

	Ref<Texture> m_RoughnessTexture;
	Ref<TextureSampler> m_RoughnessTextureSampler;

	Uniform<ModelBufferObject> m_ModelUniform;
	Uniform<ObjectBufferObject> m_ObjectUniform;

	static ObjectHandle s_NextHandle; 		// The next available handle for a new game object.
	ObjectHandle m_Handle = -1;  			// The unique handle assigned to this game object.

	/**
	 * Updates the ModelBufferObject associated with this game object by computing its current model matrix based on its
	 * position, rotation, and scale. This method is called by the Scene class on each game object during the update loop.
	 *
	 * @param dt The time elapsed since the last frame, in seconds.
	 */
	inline void update(f64 dt) { m_ModelUniform->model = transform->getWorldMatrix(); }
};

} // vulture