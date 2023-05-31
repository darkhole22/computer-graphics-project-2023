#pragma once
#include "SystemTimer.h"

// #define VU_DEBUG_TIMER_DISABLE

namespace vulture {

class ScopeTimer
{
	NO_COPY(ScopeTimer)
public:
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
