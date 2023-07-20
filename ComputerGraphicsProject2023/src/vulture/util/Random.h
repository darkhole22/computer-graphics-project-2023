#pragma once

#include <random>
#include <limits>

#include "vulture/util/Types.h"

namespace game {

using namespace vulture;

class Random
{
public:
	inline static f32 next(f32 start, f32 end)
	{
		return std::uniform_real_distribution<f32>(start, end)(m_Rng);
	}

	inline static f32 next()
	{
		return next(0.0f, 1.0f);
	}

	inline static i32 nextInt(i32 start, i32 end)
	{
		return std::uniform_int_distribution<i32>(start, end)(m_Rng);
	}

	inline static i32 nextInt()
	{
		return nextInt(0, (std::numeric_limits<i32>::max)());
	}

	inline static glm::vec2 nextAnnulusPoint(f32 outerRadius, f32 innerRadius)
	{
		f32 theta = next() * glm::two_pi<f32>();
		outerRadius *= outerRadius;
		innerRadius *= innerRadius;
		f32 d = std::sqrt(Random::next() * (outerRadius - innerRadius) + innerRadius);
		return { d * cos(theta) , d * sin(theta) };
	}

	inline static glm::vec2 nextAnnulusPoint(f32 outerRadius = 1.0f)
	{
		return nextAnnulusPoint(outerRadius, outerRadius * 0.1f);
	}

private:
	static std::random_device m_Device;
	static std::mt19937 m_Rng;
};

} // namespace game