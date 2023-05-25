#pragma once

#include "vulture/core/Core.h"
#include "HitBox.h"

namespace vulture {

class CollisionEngine
{
public:
	void update(f32 dt);

	void addHitbox(Ref<HitBox> hitbox);
	void removeHitbox(Ref<HitBox> hitbox);
private:
	std::unordered_set<Ref<HitBox>> m_HitBoxs;
};

} // namespace vulture
