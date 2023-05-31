#pragma once

#include "vulture/event/Event.h"
#include "vulture/util/String.h"

namespace vulture {

struct TimerTimeoutEvent {};

/**
 * @brief A class representing a timer with timeout events.
 *
 * The Timer class allows you to create timers with a specified wait time.
 * It provides methods to control the timer's behavior, such as starting, stopping, pausing, and resetting.
 */
class Timer
{
	EVENT(TimerTimeoutEvent)
public:
	/**
	 * @brief Constructs a Timer object with the specified wait time.
	 *
	 * This should be called only if you want to manage the Timer manually.
	 * Consider using Scene::makeTimer() instead.
	 *
	 * @param waitTime The wait time in seconds.
	 * @param oneShot Indicates whether the timer is a one-shot timer (default: true).
	 */
	Timer(f32 waitTime, bool oneShot = true);

	/**
	 * @brief Advances the timer by the specified delta time.
	 * 
	 * @param dt The time step to increment the timer by.
	 */
	void step(f32 dt);

	/**
	 * @brief Checks if the timer is currently running.
	 * 
	 * @return true if the timer is running, false otherwise.
	 */
	bool isRunning() const;

	/**
	 * @brief Pauses the timer.
	 *
	 * The timer will stop counting down until it is resumed.
	 * If the timer was already paused, this method has no effect.
	 */
	void pause();

	/**
	 * @brief Resumes the timer from the paused state.
	 *
	 * If the timer was not paused, this method has no effect.
	 */
	void play();

	/**
	 * @brief Stops the timer and resets its internal state.
	 *
	 * The timer will no longer be running after calling this method.
	 */
	void stop();

	/**
     * @brief Resets the timer with a new wait time.
	 * If the provided wait time is negative the previous
	 * waitTime will be used instead.
	 * 
     * @param waitTime The new wait time in seconds.
     */
	void reset(f32 waitTime = -1);

	/**
	 * @brief Returns the remaining time on the timer.
	 * 
	 * @return The remaining time in seconds.
	 */
	f32 remainingTime() const { return m_RemainingTime; }

	~Timer() = default;
private:
	f32 m_WaitTime;
	f32 m_RemainingTime;
	bool m_Paused = false;
	bool m_OneShot;
};

} // namespace vulture
