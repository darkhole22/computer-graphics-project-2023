#include "Timer.h"

namespace vulture {

Timer::Timer(f32 waitTime, bool oneShot) :
	m_WaitTime(waitTime), m_RemainingTime(waitTime), m_OneShot(oneShot)
{}

void Timer::step(f32 dt)
{
	if (m_Paused && m_RemainingTime > 0) return;

	m_RemainingTime -= dt;

	if (m_RemainingTime <= 0)
	{
		m_RemainingTime = m_OneShot ? 0 : m_WaitTime;
		emit(TimerTimeoutEvent{});
	}
}

bool Timer::isRunning() const
{
	return m_RemainingTime > 0;
}

void Timer::pause()
{
	m_Paused = true;
}

void Timer::play()
{
	m_Paused = false;
}

void Timer::stop()
{
	if (m_RemainingTime > 0)
	{
		m_RemainingTime = 0;
		emit(TimerTimeoutEvent{});
	}
}

void Timer::reset(f32 waitTime)
{
	if (waitTime > 0)
	{
		m_WaitTime = waitTime;
	}
	m_RemainingTime = m_WaitTime;
}

} // namespace vulture
