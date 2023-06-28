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
	 * @brief Constructs a GameObjectPool object with the specified initial size, model name, and texture name.
	 *
	 * @param initialSize The initial size of the GameObjectPool.
	 * @param modelName The name of the model that the GameObjects will use.
	 * @param textureName The name of the texture that the GameObjects will use.
	 */
	GameObjectPool(
		u32 initialSize,
	   	const String& modelName,
	   	const String& textureName,
	   	const String& emissionTextureName = DEFAULT_EMISSION_TEXTURE_NAME,
	   	const String& roughnessTextureName = DEFAULT_ROUGHNESS_TEXTURE_NAME);

	/**
	 * @brief Retrieves a GameObject instance from the pool.
	 *
	 * This method retrieves a GameObject instance from the pool. If the pool is empty,
	 * a new GameObject instance is created and returned.
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

	std::queue<Ref<GameObject>> m_ObjectQueue;
};

}