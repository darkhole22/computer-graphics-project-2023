#pragma once

#include "vulture/core/Core.h"
#include "HitBox.h"

/**
 * @brief Manages collision detection and response for hit boxes.
 *
 * The CollisionEngine class is responsible for managing collision detection
 * and response for hit boxes. It tracks a collection of hit boxes and provides
 * functions to update the engine and add/remove hit boxes dynamically.
 */
namespace vulture {

class CollisionEngine
{
public:
	/**
	 * @brief Updates the collision engine for a given time step.
	 *
	 * @param dt The time step for the update.
	 */
	void update(f32 dt);

	/**
	 * @brief Adds a hit box to the collision engine.
	 *
	 * @param hitbox The hit box to add.
	 */
	void addHitbox(Ref<HitBox> hitbox);

	/**
	 * @brief Removes a hit box from the collision engine.
	 *
	 * @param hitbox The hit box to remove.
	 */
	void removeHitbox(Ref<HitBox> hitbox);
private:
	std::unordered_set<Ref<HitBox>> m_HitBoxs;
};

} // namespace vulture
