#pragma once

#include "vulture/core/Core.h"
#include "vulture/util/Transform.h"

namespace vulture {

/**
 * @brief Represents different types of collision shapes.
 *
 * The CollisionShapeType enum class provides a set of options for defining
 * the type of collision shape used in collision detection and response.
 * Currently, the supported shape types are:
 *   - CAPSULE: Represents a capsule-shaped collision volume.
 */
enum class CollisionShapeType
{
	CAPSULE
};

/**
 * @brief Base class for collision shapes.
 *
 * The CollisionShape class serves as a base class for defining collision shapes
 * used in collision detection and response. It provides an interface for testing
 * collisions with other shapes, applying transformations, and ensures proper
 * polymorphic behavior.
 */
class CollisionShape
{
public:
	/**
	 * @brief The type of the collision shape.
	 */
	const CollisionShapeType c_Type;

	/**
	 * @brief Constructs a CollisionShape object of the specified type.
	 *
	 * @param type The type of the collision shape.
	 */
	explicit CollisionShape(CollisionShapeType type) : c_Type(type) {}

	/**
	 * @brief Tests collision between this shape and another shape.
	 *
	 * @param other A reference to the other collision shape.
	 *
	 * @return true if a collision is detected, false otherwise.
	 */
	virtual bool testCollision(const Ref<CollisionShape> other) const = 0;

	/**
	 * @brief Applies a transformation to the collision shape.
	 *
	 * @param transform The transformation to apply.
	 */
	virtual void applyTransform(const Transform& transform) = 0;

	/**
	 * @brief Virtual destructor for proper cleanup of derived classes.
	 */
	virtual ~CollisionShape() = default;
};

/**
 * @brief Represents a capsule-shaped collision shape.
 *
 * The CapsuleCollisionShape class defines a capsule-shaped collision volume
 * used in collision detection and response. It inherits from the CollisionShape
 * base class and provides specific functionality for testing collisions with
 * other shapes and applying transformations to the capsule.
 */
class CapsuleCollisionShape : public CollisionShape
{
public:
	/**
	 * @brief Constructs a CapsuleCollisionShape object with the specified radius and height.
	 *
	 * @param radius The radius of the capsule.
	 * @param height The height of the capsule.
	 */
	CapsuleCollisionShape(f32 radius, f32 height);

	/**
	 * @brief Applies a transformation to the capsule collision shape.
	 *
	 * @param transform The transformation to apply.
	 */
	virtual void applyTransform(const Transform& transform);

	/**
	 * @brief Tests collision between this capsule and another collision shape.
	 *
	 * @param other A reference to the other collision shape.
	 * @return true if a collision is detected, false otherwise.
	 */
	virtual bool testCollision(const Ref<CollisionShape> other) const;

	/**
	 * @brief Virtual destructor for proper cleanup of derived classes.
	 */
	virtual ~CapsuleCollisionShape() = default;
private:
	f32 m_Radius;
	f32 m_Height;
	glm::vec3 m_Tip;
	glm::vec3 m_Base;
public:
	friend bool testCollisionCapsuleCapsule(const CapsuleCollisionShape* c1, const CapsuleCollisionShape* c2);
};

} // namespace vulture
