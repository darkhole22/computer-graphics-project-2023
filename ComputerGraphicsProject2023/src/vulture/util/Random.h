#pragma once

#include <random>
#include <limits>

#include "vulture/util/Types.h"

namespace game {

using namespace vulture;

class Random
{
public:
	template<class Type = f32>
	inline static Type next(Type start, Type end)
	{}

	template<class Type = f32>
	inline static Type next()
	{}

	template<>
	inline static f32 next<f32>(f32 start, f32 end)
	{
		return std::uniform_real_distribution<f32>(start, end)(m_Rng);
	}

	template<>
	inline static f32 next<f32>()
	{
		return next<f32>(0.0f, 1.0f);
	}

	template<>
	inline static i32 next<i32>(i32 start, i32 end)
	{
		return std::uniform_int_distribution<i32>(start, end)(m_Rng);
	}

	template<>
	inline static i32 next<i32>()
	{
		return next<i32>(0, (std::numeric_limits<i32>::max)());
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