#pragma once
#include "SystemTimer.h"

// #define VU_DEBUG_TIMER_DISABLE

namespace vulture {

/**
 * @brief A class for measuring the execution time of a scope.
 *
 * The ScopeTimer class allows you to measure the execution time of a scope
 * by starting a timer when the ScopeTimer object is created and stopping it when the object is destroyed.
 * The class is non-copyable to prevent unintended copying of timer instances.
 */
class ScopeTimer
{
	NO_COPY(ScopeTimer)
public:
	/**
	 * @brief Constructs a ScopeTimer object with the specified message.
	 * 
	 * @param message The message associated with the timer (e.g., scope description).
	 */
	ScopeTimer(const String& message);

	~ScopeTimer();
private:
	String m_Message;
	const SystemTimer c_Timer;
};

#if defined(VU_DEBUG_BUILD) && !defined(VU_DEBUG_TIMER_DISABLE)
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define DEBUG_TIMER(message) ScopeTimer TOKENPASTE2(timer_, __LINE__){message}
#else
#define DEBUG_TIMER(message)
#endif
} // namespace vulture
