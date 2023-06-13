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
private:
	static std::random_device m_Device;
	static std::mt19937 m_Rng;
};

} // namespace game