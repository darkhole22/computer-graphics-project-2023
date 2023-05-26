#include "CollisionEngine.h"

namespace vulture {

void CollisionEngine::update(f32 dt)
{
	for (auto& hitbox : m_HitBoxes)
	{
		hitbox->applyTransform();
	}

	auto outerIt = m_HitBoxes.begin();
	while (outerIt != m_HitBoxes.end())
	{
		auto& hitbox1 = *outerIt;
		auto innerIt = ++outerIt;
		while (innerIt != m_HitBoxes.end())
		{
			auto& hitbox2 = *innerIt;

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
			innerIt++;
		}
	}
}

void CollisionEngine::addHitbox(Ref<HitBox> hitbox)
{
	m_HitBoxes.insert(hitbox);
}

void CollisionEngine::removeHitbox(Ref<HitBox> hitbox)
{
	m_HitBoxes.erase(hitbox);
}

} // namespace vulture
