#pragma once

#include <queue>

#include "vulture/core/Application.h"
#include "vulture/scene/GameObject.h"

#include <queue>

namespace vulture {

/**
 * @brief A class representing a pool of GameObjects.
 *
 * The GameObjectPool class manages a pool of GameObject instances for efficient object reuse.
 * It provides methods for retrieving and returning objects to the pool.
 */
class GameObjectPool
{
public:
	/**
	 * @brief Constructs a GameObjectPool object with the specified initial size, model name and transform, and textures.
	 *
	 * @param initialSize The initial size of the GameObjectPool.
	 * @param modelName The name of the model that the GameObjects will use.
	 * @param baseTextureName Name of the base color texture.
	 * @param emissionTextureName Name of the emission color texture.
	 * @param roughnessTextureName Name of the roughness texture.
	 * @param loadTransform Initial model transform.
	 * @param growth How many new game objects should be created when the pool is empty and a new one is requested.
	 */
	GameObjectPool(
		u32 initialSize,
	   	const String& modelName,
	   	const String& baseTextureName,
	   	const String& emissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME,
	   	const String& roughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME,
		const glm::mat4& loadTransform = glm::mat4(1),
		u32 growth = 0);

	/**
	 * @brief Retrieves a GameObject instance from the pool.
	 *
	 * This method retrieves a GameObject instance from the pool.
	 *
	 * If the pool is empty and growth > 0, a new GameObject instance is created and returned.
	 * Otherwise, nullptr is returned.
	 *
	 * @return A reference to a GameObject instance.
	 */
	Ref<GameObject> get();

	/**
	 * @brief Returns a GameObject instance to the pool for reuse.
	 *
	 * This method returns a GameObject instance to the pool for future reuse. The instance
	 * is added to the end of the object queue.
	 *
	 * @param obj A reference to the GameObject instance to be returned to the pool.
	 */
	void put(Ref<GameObject> obj);

	~GameObjectPool() = default;

private:
	String m_ModelName;
	String m_TextureName;
	String m_RoughnessTextureName;
	String m_EmissionTextureName;

	u32 m_Growth;

	std::queue<Ref<GameObject>> m_ObjectQueue;
};

}