#pragma once

#include "vulture/core/Core.h"
#include "vulture/util/Transform.h"

namespace vulture {

enum class CollisionShapeType
{
	// AABB,
	CAPSULE
};

class CollisionShape
{
public:
	const CollisionShapeType c_Type;
	
	CollisionShape(CollisionShapeType type) : c_Type(type) {}

	virtual bool testCollision(const Ref<CollisionShape> other) const = 0;

	virtual void applyTransform(const Transform& transform) = 0;

	virtual ~CollisionShape() = default;
};

class CapsuleCollisionShape : public CollisionShape
{
public:
	CapsuleCollisionShape(f32 radius, f32 height);

	virtual void applyTransform(const Transform& transform);

	virtual bool testCollision(const Ref<CollisionShape> other) const;

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
