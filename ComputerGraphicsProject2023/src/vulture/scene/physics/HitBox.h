#pragma once

#include "vulture/core/Core.h"
#include "vulture/event/Event.h"
#include "CollisionShape.h"

#include <vector>
#include <unordered_set>

namespace vulture {

#define BIT(n) (1uLL << n)
enum MaskBit : u64
{
	NOME = 0,
	BIT0 = BIT(0), BIT1 = BIT(1), BIT2 = BIT(2), BIT3 = BIT(3),
	BIT4 = BIT(4), BIT5 = BIT(5), BIT6 = BIT(6), BIT7 = BIT(7),
	BIT8 = BIT(8), BIT9 = BIT(9), BIT10 = BIT(10), BIT11 = BIT(11),
	BIT12 = BIT(12), BIT13 = BIT(13), BIT14 = BIT(14), BIT15 = BIT(15),
	BIT16 = BIT(16), BIT17 = BIT(17), BIT18 = BIT(18), BIT19 = BIT(19),
	BIT20 = BIT(20), BIT21 = BIT(21), BIT22 = BIT(22), BIT23 = BIT(23),
	BIT24 = BIT(24), BIT25 = BIT(25), BIT26 = BIT(26), BIT27 = BIT(27),
	BIT28 = BIT(28), BIT29 = BIT(29), BIT30 = BIT(30), BIT31 = BIT(31),
	BIT32 = BIT(32), BIT33 = BIT(33), BIT34 = BIT(34), BIT35 = BIT(35),
	BIT36 = BIT(36), BIT37 = BIT(37), BIT38 = BIT(38), BIT39 = BIT(39),
	BIT40 = BIT(40), BIT41 = BIT(41), BIT42 = BIT(42), BIT43 = BIT(43),
	BIT44 = BIT(44), BIT45 = BIT(45), BIT46 = BIT(46), BIT47 = BIT(47),
	BIT48 = BIT(48), BIT49 = BIT(49), BIT50 = BIT(50), BIT51 = BIT(51),
	BIT52 = BIT(52), BIT53 = BIT(53), BIT54 = BIT(54), BIT55 = BIT(55),
	BIT56 = BIT(56), BIT57 = BIT(57), BIT58 = BIT(58), BIT59 = BIT(59),
	BIT60 = BIT(60), BIT61 = BIT(61), BIT62 = BIT(62), BIT63 = BIT(63)
};
#undef BIT

struct HitBoxEntered
{
	void* data;
};

struct HitBoxExited
{
	void* data;
};

class HitBox
{
	EVENT(HitBoxEntered)
	EVENT(HitBoxExited)
public:
	u64 layerMask = MaskBit::NOME;
	u64 collisionMask = MaskBit::NOME;
	void* data = nullptr;
	Transform transform;

	explicit HitBox(Ref<CollisionShape> shape);

	void addCollisionShape(Ref<CollisionShape> shape);

	auto begin() { return m_Shapes.begin(); }
	auto end() { return m_Shapes.end(); }
private:
	using HitBoxsSet = std::unordered_set<Ref<HitBox>>;

	std::vector<Ref<CollisionShape>> m_Shapes;
	HitBoxsSet m_PreviousCollidingHitBoxs;
	HitBoxsSet m_CurrentCollidingHitBoxs;

	void applyTransform();
	void registerCollidingHitbox(Ref<HitBox> hitbox);
	void update();
public:
	friend class CollisionEngine;
};

} // namespace vulture