#pragma once

#include <unordered_set>

#include "../../vulture/util/GameObjectPool.h"
#include "../../vulture/core/Core.h"

using namespace vulture;

namespace game {

enum EntityStatus
{
	ALIVE,
	DEAD
};

template<typename T>
class Factory
{
public:
	explicit Factory<T>(int initialSize)
	{
		m_ObjectPool = new GameObjectPool(initialSize, T::s_ModelName, T::s_TextureName);
	}

	Ref<T> get()
	{
		auto obj = m_ObjectPool->get();
		Application::getScene()->addObject(obj);

		auto t = makeRef<T>(obj);
		m_ActiveEntities.insert(t);

		return t;
	}

	void remove(Ref<T> t)
	{
		m_ObjectPool->put(t->m_GameObject);

		auto it = m_ActiveEntities.find(t);
		m_ActiveEntities.erase(it);
	}

	void update(f32 dt)
	{
		std::vector<Ref<T>> toReturn;

		for (auto t: m_ActiveEntities)
		{
			if (t->update(dt) == DEAD)
			{
				toReturn.push_back(t);
			}
		}

		for (auto t: toReturn)
		{
			remove(t);
		}
	}

	~Factory() = default;

private:
	GameObjectPool* m_ObjectPool;
	std::unordered_set<Ref<T>> m_ActiveEntities;
};

}