#pragma once
#include "SystemTimer.h"

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

} // namespace vulture
