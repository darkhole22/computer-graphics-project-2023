#include "CollisionShape.h"

#include <algorithm>

namespace vulture {

bool testCollisionCapsuleCapsule(const CapsuleCollisionShape* c1, const CapsuleCollisionShape* c2);

CapsuleCollisionShape::CapsuleCollisionShape(f32 radius, f32 height) :
	CollisionShape(CollisionShapeType::CAPSULE), m_Radius(radius)
{
	f32 diameter = m_Radius * 2;
	m_Height = std::max(height, diameter + 0.001f);
	f32 vScale = (m_Height - diameter) / 2;
	m_Tip = glm::vec3(0, 1, 0) * vScale;
	m_Base = -m_Tip;
}

void CapsuleCollisionShape::applyTransform(const Transform& transform)
{
	f32 vScale = (m_Height - m_Radius * 2) / 2;
	auto rotation = glm::mat3(transform.getRotation());
	m_Tip = rotation * glm::vec3(0, 1, 0) * vScale;
	m_Base = -m_Tip;
	auto translation = transform.getPosition();
	m_Tip += translation;
	m_Base += translation;
}

bool CapsuleCollisionShape::testCollision(const Ref<CollisionShape> other) const
{
	switch (other->c_Type)
	{
	case CollisionShapeType::CAPSULE:
	{
		const CapsuleCollisionShape* otherCapsule = reinterpret_cast<CapsuleCollisionShape*>(other.get());
		return testCollisionCapsuleCapsule(this, otherCapsule);
	}
	default:
	break;
	}
	return false;
}

glm::vec3 closestPointOnLineSegment(glm::vec3 a, glm::vec3 b, glm::vec3 point)
{
	glm::vec3 AB = b - a;
	f32 t = glm::dot(point - a, AB) / glm::dot(AB, AB);
	return a + std::clamp(t, 0.0f, 1.0f) * AB;
}

// https://wickedengine.net/2020/04/26/capsule-collision-detection/
bool testCollisionCapsuleCapsule(const CapsuleCollisionShape* c0, const CapsuleCollisionShape* c1)
{
	// Capsule C0:
	glm::vec3 c0LineEndOffset = glm::normalize(c0->m_Tip - c0->m_Base) * c0->m_Radius;
	glm::vec3 c0A = c0->m_Base + c0LineEndOffset;
	glm::vec3 c0B = c0->m_Tip - c0LineEndOffset;

	// Capsule C1:
	glm::vec3 c1LineEndOffset = glm::normalize(c1->m_Tip - c1->m_Base) * c1->m_Radius;
	glm::vec3 c1A = c1->m_Base + c1LineEndOffset;
	glm::vec3 c1B = c1->m_Tip - c1LineEndOffset;

	// Vectors between line endpoints:
	glm::vec3 v0 = c1A - c0A;
	glm::vec3 v1 = c1B - c0A;
	glm::vec3 v2 = c1A - c0B;
	glm::vec3 v3 = c1B - c0B;

	// Squared distances:
	f32 d0 = glm::dot(v0, v0);
	f32 d1 = glm::dot(v1, v1);
	f32 d2 = glm::dot(v2, v2);
	f32 d3 = glm::dot(v3, v3);

	// Select best potential endpoint on capsule C0:
	glm::vec3 bestC0;
	if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
	{
		bestC0 = c0B;
	}
	else
	{
		bestC0 = c0A;
	}

	// Select point on capsule C1 line segment nearest to best potential endpoint on C0 capsule:
	glm::vec3 bestC1 = closestPointOnLineSegment(c1A, c1B, bestC0);

	// Now do the same for capsule C0 segment:
	bestC0 = closestPointOnLineSegment(c0A, c0B, bestC1);

	glm::vec3 penetration_normal = bestC0 - bestC1;
	f32 len = glm::length(penetration_normal);
	// penetration_normal /= len;  // normalize
	f32 penetration_depth = c0->m_Radius + c1->m_Radius - len;
	// bool intersects = penetration_depth > 0;

	return penetration_depth > 0;
}

} // namespace vulture
