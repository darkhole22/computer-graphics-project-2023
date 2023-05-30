#pragma once

#include "vulture/event/Event.h"
#include "vulture/util/String.h"

namespace vulture {

struct TimerTimeoutEvent {};

class Timer
{
	EVENT(TimerTimeoutEvent)
public:
	Timer(f32 waitTime, bool oneShot = true);

	void step(f32 dt);

	bool isRunning() const;
	void pause();
	void play();
	void stop();
	void reset(f32 waitTime);
	f32 remaningTime() const { return m_RemaningTime; }

	~Timer() = default;
private:
	f32 m_WaitTime;
	f32 m_RemaningTime;
	bool m_Paused = false;
	bool m_OneShot;
};

} // namespace vulture
