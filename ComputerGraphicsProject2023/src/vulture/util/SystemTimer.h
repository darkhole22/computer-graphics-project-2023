#pragma once
#include "vulture/core/Core.h"

#include <chrono>

namespace vulture {

/**
 * @brief An enumeration representing different units of time.
 *
 * The TimeUnit enum class defines the units of time that can be used for measuring elapsed time.
 * The available units include nanoseconds, microseconds, milliseconds, and seconds.
 */
enum class TimeUnit
{
	NANOSECOND, MICROSECOND,
	MILLISECOND, SECOND
};

/**
 * @brief A class for measuring system time.
 *
 * The SystemTimer class provides functionality for measuring elapsed time using the system clock.
 * It can be used to restart the timer, retrieve the elapsed time, and specify the desired time unit.
 */
class SystemTimer
{
public:
	using TimePoint = std::chrono::high_resolution_clock::time_point;

	/**
	* @brief Constructs a SystemTimer object and initializes the start time.
	*/
	SystemTimer();

	/**
	 * @brief Restarts the timer and returns the elapsed time since the previous start.
	 * 
	 * @param unit The time unit to use for the elapsed time (default: TimeUnit::NANOSECOND).
	 * 
	 * @return The elapsed time since the previous start, converted to the specified time unit.
	 */
	u64 restart(TimeUnit unit = TimeUnit::NANOSECOND);

	/**
	 * @brief Retrieves the elapsed time since the start of the timer.
	 * 
	 * @param unit The time unit to use for the elapsed time (default: TimeUnit::NANOSECOND).
	 * 
	 * @return The elapsed time since the start of the timer, converted to the specified time unit.
	 */
	u64 elapsed(TimeUnit unit = TimeUnit::NANOSECOND) const;

	~SystemTimer() = default;
private:
	TimePoint m_Start;
};

} // namespace vulture
