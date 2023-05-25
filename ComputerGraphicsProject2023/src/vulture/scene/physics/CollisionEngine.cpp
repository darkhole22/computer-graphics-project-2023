#include "CollisionEngine.h"

#include "vulture/core/Logger.h"

namespace vulture {

void CollisionEngine::update(f32 dt)
{
	for (auto& hitbox : m_HitBoxs)
	{
		hitbox->applyTransform();
	}

	auto outherIterator = m_HitBoxs.begin();
	while (outherIterator != m_HitBoxs.end())
	{
		auto& hitbox1 = *outherIterator;
		auto innerIterator = ++outherIterator;
		while (innerIterator != m_HitBoxs.end())
		{
			auto& hitbox2 = *innerIterator;

			if ((hitbox1->collisionMask & hitbox2->layerMask) != 0 ||
				(hitbox2->collisionMask & hitbox1->layerMask) != 0)
			{
				for (auto& shape1 : *hitbox1)
				{
					for (auto& shape2 : *hitbox2)
					{
 						if (shape1->testCollision(shape2))
						{
							if ((hitbox1->collisionMask & hitbox2->layerMask) != 0)
							{
								hitbox1->registerCollidingHitbox(hitbox2);
							}
							if ((hitbox2->collisionMask & hitbox1->layerMask) != 0)
							{
								hitbox2->registerCollidingHitbox(hitbox1);
							}
						}
					}
				}
			}
			hitbox1->update();
			innerIterator++;
		}
	}
}

void CollisionEngine::addHitbox(Ref<HitBox> hitbox)
{
	m_HitBoxs.insert(hitbox);
}

void CollisionEngine::removeHitbox(Ref<HitBox> hitbox)
{
	m_HitBoxs.erase(hitbox);
}

} // namespace vulture
