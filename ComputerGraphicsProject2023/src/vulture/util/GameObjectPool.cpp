#include "GameObjectPool.h"

namespace vulture {

GameObjectPool::GameObjectPool(int initialSize, const String& modelName, const String& textureName)
{
	m_ModelName = modelName;
	m_TextureName = textureName;

	for(int i = 0; i < initialSize; i++)
	{
		m_ObjectQueue.push(makeRef<GameObject>(m_ModelName, m_TextureName));
	}
}

Ref<GameObject> GameObjectPool::get()
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

void GameObjectPool::put(Ref<GameObject> obj)
{
	Application::getScene()->removeObject(obj);
	m_ObjectQueue.push(obj);

	VUINFO("GameObjectPool size after put(): %d", m_ObjectQueue.size());
}

} // namespace vulture