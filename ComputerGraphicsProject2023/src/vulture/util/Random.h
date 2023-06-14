#pragma once

#include <random>
#include "vulture/util/Types.h"

namespace game {

using namespace vulture;

class Random
{
public:
	inline static f32 next(f32 start = 0.0f, f32 end = 1.0f)
	{
		return std::uniform_real_distribution<f32>(start, end)(m_Rng);
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