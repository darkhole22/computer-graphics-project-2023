#include "SystemTimer.h"

namespace vulture {

SystemTimer::SystemTimer() :
	m_Start(std::chrono::high_resolution_clock::now())
{}

u64 SystemTimer::restart(TimeUnit unit)
{
	u64 result = elapsed(unit);
	m_Start = std::chrono::high_resolution_clock::now();
	return result;
}

u64 SystemTimer::elapsed(TimeUnit unit) const
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	u64 result = std::chrono::duration<u64, std::chrono::nanoseconds::period>(currentTime - m_Start).count();;
	switch (unit)
	{
	case vulture::TimeUnit::SECOND:
	result /= 1000000000;
	break;
	case vulture::TimeUnit::MILLISECOND:
	result /= 1000000;
	break;
	case vulture::TimeUnit::MICROSECOND:
	result /= 1000;
	break;
	case vulture::TimeUnit::NANOSECOND:
	default:
	break;
	}
	return result;
}

} // namespace vulture
