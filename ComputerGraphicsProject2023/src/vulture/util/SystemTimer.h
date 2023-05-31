#pragma once
#include "vulture/core/Core.h"

#include <chrono>

namespace vulture {

enum class TimeUnit
{
	NANOSECOND, MICROSECOND,
	MILLISECOND, SECOND
};

class SystemTimer
{
public:
	using TimePoint = std::chrono::high_resolution_clock::time_point;

	SystemTimer();

	u64 restart(TimeUnit unit = TimeUnit::NANOSECOND);

	u64 elapsed(TimeUnit unit = TimeUnit::NANOSECOND) const;

	~SystemTimer() = default;
private:
	TimePoint m_Start;
};

} // namespace vulture
