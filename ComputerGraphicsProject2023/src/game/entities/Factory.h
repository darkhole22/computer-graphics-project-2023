#pragma once

#include <unordered_set>

#include "vulture/util/GameObjectPool.h"
#include "vulture/core/Core.h"

using namespace vulture;

namespace game {

enum EntityStatus
{
	ALIVE,
	DEAD
};

/**
 * @brief A generic factory class for creating and managing entities.
 *
 * The Factory class provides a generic interface for creating and managing entities of type T.
 *
 * It utilizes a GameObjectPool to efficiently manage object reuse and provides methods for creating, removing, and updating entities.
 *
 * @tparam T The type of entities to be managed by the factory.
 */
template<typename T>
class Factory
{
public:
	/**
	* @brief Constructs a Factory object with the specified initial size.
	*
	* This constructor creates a Factory object and initializes an associated GameObjectPool
	* with the specified initial size, using the model name and texture name of type T.
	*
	* @param initialSize The initial size of the GameObjectPool.
	*/
	explicit Factory<T>(int initialSize, const glm::mat4& loadTransform = glm::mat4(1))
	{
		m_ObjectPool = makeRef<GameObjectPool>(initialSize, T::s_ModelName, T::s_TextureName, loadTransform);
	}

	/**
	 * @brief Retrieves a new entity from the factory.
	 *
	 * This method retrieves a new entity from the factory by acquiring a GameObject instance
	 * from the object pool, adding it to the current scene, and creating an entity of type T
	 * using the acquired GameObject. The created entity is then stored in the active entity set.
	 *
	 * @return A reference to the created entity of type T.
	 */
	Ref<T> get()
	{
		auto obj = m_ObjectPool->get();
		Application::getScene()->addObject(obj);

		auto t = makeRef<T>(obj);
		m_ActiveEntities.insert(t);

		return t;
	}

	/**
	 * @brief Removes an entity from the factory.
	 *
	 * This method removes the specified entity from the factory by returning its associated
	 * GameObject instance to the object pool and erasing the entity from the active entity set.
	 *
	 * @param t A reference to the entity to be removed.
	 */
	void remove(Ref<T> t)
	{
		m_ObjectPool->put(t->m_GameObject);
		Application::getScene()->removeObject(t->m_GameObject);

		auto it = m_ActiveEntities.find(t);
		m_ActiveEntities.erase(it);
	}

	/**
	 * @brief Updates all active entities.
	 *
	 * This method updates all active entities by iterating through the active entity set,
	 * calling the update method on each entity, and removing entities with a status of DEAD.
	 *
	 * @param dt The time elapsed since the last update, in seconds.
	 */
	void update(f32 dt)
	{
		std::vector<Ref<T>> toRemove;

		for (auto& t : m_ActiveEntities)
		{
			if (t->update(dt) == EntityStatus::DEAD)
			{
				toRemove.push_back(t);
			}
		}

		for (auto& t : toRemove)
		{
			remove(t);
		}
	}

	/**
	 * @brief Removes all active entities.
	 *
	 * This method removes all active entities, returning their GameObject
	 * to the GameObjectPool.
	 */
	void reset()
	{
		for (auto& t : m_ActiveEntities)
		{
			m_ObjectPool->put(t->m_GameObject);
			Application::getScene()->removeObject(t->m_GameObject);
		}

		m_ActiveEntities.clear();
	}

	/**
	 * @brief Returns an iterator pointing to the beginning of the active entity set.
	 * @return An iterator pointing to the beginning of the active entity set.
	 */
	inline auto begin() { return m_ActiveEntities.begin(); }

	/**
	 * @brief Returns an iterator pointing to the end of the active entity set.
	 * @return An iterator pointing to the end of the active entity set.
	 */
	inline auto end() { return m_ActiveEntities.end(); }

	~Factory() = default;

private:
	Ref<GameObjectPool> m_ObjectPool;
	std::unordered_set<Ref<T>> m_ActiveEntities;
};

}