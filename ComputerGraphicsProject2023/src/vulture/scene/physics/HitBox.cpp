#include "HitBox.h"

namespace vulture {

HitBox::HitBox(Ref<CollisionShape> shape)
{
	m_Shapes.push_back(shape);
}

void HitBox::addCollisionShape(Ref<CollisionShape> shape)
{
	m_Shapes.push_back(shape);
}

void HitBox::applyTransform()
{
	for (auto& shape : m_Shapes)
	{
		shape->applyTransform(transform);
	}
}

void HitBox::registerCollidingHitbox(Ref<HitBox> hitbox)
{
	auto [it, added] = m_CurrentCollidingHitBoxs.insert(hitbox);
	if (added && !m_PreviousCollidingHitBoxs.contains(hitbox))
		emit(HitBoxEntered{ hitbox->data });
}

void HitBox::update()
{
	for (auto& hitbox : m_PreviousCollidingHitBoxs)
	{
		if (!m_CurrentCollidingHitBoxs.contains(hitbox))
			emit(HitBoxExited{ hitbox->data });
	}
	m_PreviousCollidingHitBoxs.clear();
	std::swap(m_PreviousCollidingHitBoxs, m_CurrentCollidingHitBoxs);
}

} // namespace vulture
