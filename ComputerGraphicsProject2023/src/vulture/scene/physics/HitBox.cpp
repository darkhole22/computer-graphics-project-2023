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
	auto [it, added] = m_CurrentCollidingHitBoxes.insert(hitbox);
	if (added && !m_PreviousCollidingHitBoxes.contains(hitbox))
		emit(HitBoxEntered{ hitbox->data });
}

void HitBox::update()
{
	for (auto& hitbox : m_PreviousCollidingHitBoxes)
	{
		if (!m_CurrentCollidingHitBoxes.contains(hitbox))
			emit(HitBoxExited{ hitbox->data });
	}
	m_PreviousCollidingHitBoxes.clear();
	std::swap(m_PreviousCollidingHitBoxes, m_CurrentCollidingHitBoxes);
}

} // namespace vulture
