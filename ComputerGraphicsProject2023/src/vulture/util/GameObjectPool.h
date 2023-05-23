#pragma once

#include <unordered_set>
#include <queue>

#include "Types.h"

#include "vulture/core/Application.h"
#include "vulture/scene/GameObject.h"

namespace vulture {

class GameObjectPool
{
private:
	String m_ModelName;
	String m_TextureName;

	std::queue<Ref<GameObject>> m_ObjectQueue;

public:
	explicit GameObjectPool(int initialSize, const String& modelName, const String& textureName)
	{
		m_ModelName = modelName;
		m_TextureName = textureName;

		for(int i = 0; i < initialSize; i++)
		{
			m_ObjectQueue.push(makeRef<GameObject>(m_ModelName, m_TextureName));
		}
	}

	Ref<GameObject> get()
	{
		Ref<GameObject> obj;
		if(m_ObjectQueue.empty())
		{
			obj = makeRef<GameObject>(m_ModelName, m_TextureName);
		}
		else
		{
			obj = m_ObjectQueue.front();
			m_ObjectQueue.pop();
		}

		VUINFO("GameObjectPool size after get(): %d", m_ObjectQueue.size());

		return obj;
	}

	void put(Ref<GameObject> obj)
	{
		Application::getScene()->removeObject(obj);
		m_ObjectQueue.push(obj);

		VUINFO("GameObjectPool size after put(): %d", m_ObjectQueue.size());
	}

	~GameObjectPool() = default;
};

}