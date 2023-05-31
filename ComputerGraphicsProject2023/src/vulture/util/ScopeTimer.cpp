#include "ScopeTimer.h"
#include "vulture/core/Logger.h"

namespace vulture {

ScopeTimer::ScopeTimer(const String& message) :
	m_Message(message)
{}

ScopeTimer::~ScopeTimer()
{
	u64 nano = c_Timer.elapsed();
	u64 micro = nano / 1000;
	u64 milli = micro / 1000;
	u64 sec = milli / 1000;
	VUINFO("%s: %llus %3llums %3lluus %3lluns", m_Message.cString(),
		   sec, milli % 1000, micro % 1000, nano % 1000);
}

} // namespace vulture
