#include "GameObjectPool.h"

#define VU_LOGGER_DISABLE_INFO
#include "vulture/core/Logger.h"

namespace vulture {

GameObjectPool::GameObjectPool(
		u32 initialSize,
		const String& modelName, const String& baseTextureName, const String& emissionTextureName, const String& roughnessTextureName,
		const glm::mat4& loadTransform,
		u32 growth
) :
		m_ModelName(modelName),
		m_TextureName(baseTextureName), m_EmissionTextureName(emissionTextureName), m_RoughnessTextureName(roughnessTextureName),
		m_Growth(growth)
{
	for (u32 i = 0; i < initialSize; i++)
	{
		m_ObjectQueue.push(makeRef<GameObject>(m_ModelName, m_TextureName, m_EmissionTextureName, m_RoughnessTextureName, loadTransform));
	}
}

Ref<GameObject> GameObjectPool::get()
{
	Ref<GameObject> obj;
	if (!m_ObjectQueue.empty())
	{
		obj = m_ObjectQueue.front();
		m_ObjectQueue.pop();

		return obj;
	}

	if (m_Growth <= 0) return nullptr;

	// Instantiate m_Growth new game objects
	for (u32 i = 0; i < m_Growth; i++)
	{
		auto allocated = makeRef<GameObject>(m_ModelName, m_TextureName, m_EmissionTextureName, m_RoughnessTextureName);
		m_ObjectQueue.push(allocated);
	}
	obj = makeRef<GameObject>(m_ModelName, m_TextureName, m_EmissionTextureName, m_RoughnessTextureName);

	return obj;
}

void GameObjectPool::put(Ref<GameObject> obj)
{
	Application::getScene()->removeObject(obj);
	m_ObjectQueue.push(obj);
}

} // namespace vulture