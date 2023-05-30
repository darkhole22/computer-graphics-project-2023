#pragma once
#include "vulture/core/Core.h"

#include <chrono>

namespace vulture {

class ScopeTimer
{
	NO_COPY(ScopeTimer)
public:
	ScopeTimer(const String& message);

	~ScopeTimer();
private:
	String m_Message;
	const std::chrono::high_resolution_clock::time_point c_Start;
};

} // namespace vulture
