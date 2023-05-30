#include "Timer.h"

namespace vulture {

Timer::Timer(f32 waitTime, bool oneShot) :
	m_WaitTime(waitTime), m_RemaningTime(waitTime), m_OneShot(oneShot)
{}

void Timer::step(f32 dt)
{
	if (m_Paused && m_RemaningTime > 0) return;

	m_RemaningTime -= dt;

	if (m_RemaningTime <= 0)
	{
		m_RemaningTime = m_OneShot ? 0 : m_WaitTime;
		emit(TimerTimeoutEvent{});
	}
}

bool Timer::isRunning() const
{
	return m_RemaningTime > 0;
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
	m_RemaningTime = 0;
	emit(TimerTimeoutEvent{});
}

void Timer::reset(f32 waitTime)
{
	if (waitTime > 0)
	{
		m_WaitTime = waitTime;
	}
	m_RemaningTime = m_WaitTime;
}

} // namespace vulture
