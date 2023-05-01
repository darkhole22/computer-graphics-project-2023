#include "Tween.h"

// #define VU_LOGGER_TRACE_ENABLED
#include "vulture/core/Logger.h"

namespace vulture {

Tween::Tween()
{
	m_Tweener = makeRef<SequentialTweener>();
}

bool Tween::isRunning() const
{
	return m_Tweener && !m_Tweener->isFinisced() && m_Valid;
}

bool Tween::isValid() const
{
	return m_Valid;
}

void Tween::play()
{
	m_IsPaused = false;
}

void Tween::pause()
{
	m_IsPaused = true;
}

void Tween::stop()
{
	m_Valid = false;
}

Tween* Tween::loop(u64 loops)
{
	m_LoopCount = loops;
	return this;
}

Ref<IntervalTweener> Tween::addIntervalTweener(float duration)
{
	return m_Tweener->addIntervalTweener(duration);
}

Ref<ParallelTweener> Tween::addParallelTweener()
{
	return m_Tweener->addParallelTweener();
}

Ref<CallbackTweener> Tween::addCallbackTweener(std::function<void()> callback)
{
	return m_Tweener->addCallbackTweener(callback);
}

Tween::~Tween()
{
	VUTRACE("Tween destructed.");
}

void Tween::step(float dt)
{
	if (isRunning() && !m_IsPaused &&
		(m_CurrentLoop < m_LoopCount || m_LoopCount == 0))
	{
		m_Tweener->step(dt);

		if (m_Tweener->isFinisced())
		{
			m_CurrentLoop++;
			if (m_LoopCount == 0 || m_CurrentLoop < m_LoopCount)
				m_Tweener->reset();
		}
	}
}

IntervalTweener::IntervalTweener(float duration) : c_Duration(duration)
{
}

void IntervalTweener::step(float dt)
{
	if (!isFinisced())
		m_Elapsed += dt;
}

bool IntervalTweener::isFinisced() const
{
	return m_Elapsed >= c_Duration;
}

void IntervalTweener::reset()
{
	m_Elapsed = 0.0f;
}

Ref<IntervalTweener> CollectionTweener::addIntervalTweener(float duration)
{
	auto tweener = makeRef<IntervalTweener>(duration);
	m_Tweeners.emplace_back(tweener);
	return tweener;
}

Ref<CallbackTweener> CollectionTweener::addCallbackTweener(std::function<void()> callback)
{
	auto tweener = makeRef<CallbackTweener>(callback);
	m_Tweeners.emplace_back(tweener);
	return tweener;
}

void SequentialTweener::step(float dt)
{
	if (!isFinisced())
	{
		auto& tweener = m_Tweeners[m_Index];

		tweener->step(dt);

		if (tweener->isFinisced())
			m_Index++;
	}
}

bool SequentialTweener::isFinisced() const
{
	return m_Index >= m_Tweeners.size();
}

void SequentialTweener::reset()
{
	for (auto& tweener : m_Tweeners)
		tweener->reset();
	m_Index = 0;
}

Ref<ParallelTweener> SequentialTweener::addParallelTweener()
{
	auto tweener = makeRef<ParallelTweener>();
	m_Tweeners.emplace_back(tweener);
	return tweener;
}

void ParallelTweener::step(float dt)
{
	if (!isFinisced())
	{
		for (auto& tweener : m_Tweeners)
			tweener->step(dt);
	}
}

bool ParallelTweener::isFinisced() const
{
	for (auto& tweener : m_Tweeners)
	{
		if (!tweener->isFinisced())
			return false;
	}
	return true;
}

void ParallelTweener::reset()
{
	for (auto& tweener : m_Tweeners)
		tweener->reset();
}

Ref<SequentialTweener> ParallelTweener::addSequentialTweener()
{
	auto tweener = makeRef<SequentialTweener>();
	m_Tweeners.emplace_back(tweener);
	return tweener;
}

CallbackTweener::CallbackTweener(std::function<void()> callback) : m_Callback(callback)
{
}

void CallbackTweener::step(float dt)
{
	if (!isFinisced())
	{
		m_Callback();
		m_Started = true;
	}
}

bool CallbackTweener::isFinisced() const
{
	return m_Started;
}

void CallbackTweener::reset()
{
	m_Started = false;
}

} // namespace vulture

