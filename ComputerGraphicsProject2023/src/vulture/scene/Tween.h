#pragma once

#include "vulture/core/Core.h"

#include <vector>
#include <algorithm>
#include <functional>

namespace vulture {

class Tweener
{
public:
	virtual void step(float dt) = 0;
	virtual bool isFinisced() const = 0;
	virtual void reset() = 0;
};

template <class Type> class ValueTweener;
class IntervalTweener;
class SequentialTweener;
class ParallelTweener;
class CallbackTweener;

class Tween
{
public:
	Tween();
	void step(float dt);

	bool isRunning() const;
	bool isValid() const;

	void play();
	void pause();
	void stop();

	Tween* loop(u64 loops = 0);

	Ref<IntervalTweener> addIntervalTweener(float duration);

	template<class Type>
	Ref<ValueTweener<Type>> addValueTweener(Type* value, Type finalValue, float duration);

	Ref<ParallelTweener> addParallelTweener();

	Ref<CallbackTweener> addCallbackTweener(std::function<void()> callback);

	~Tween();
private:
	Ref<SequentialTweener> m_Tweener;
	u64 m_LoopCount = 1;
	u64 m_CurrentLoop = 0;
	bool m_IsPaused = false;
	bool m_Valid = true;

};

class IntervalTweener : public Tweener
{
public:
	IntervalTweener(float duration);

	void step(float dt) override;
	bool isFinisced() const override;
	void reset() override;
private:
	const float c_Duration;
	float m_Elapsed = 0;
};

template <class Type>
class ValueTweener : public Tweener
{
public:
	ValueTweener(Type* value, Type finalValue, float duration) :
		m_Value(value), m_InitialValue(*value), c_FinalValue(finalValue), c_Duration(duration) {}

	void step(float dt) override
	{
		if (!m_Started)
		{
			if (c_Duration == 0.0f)
				*m_Value = c_FinalValue;

			m_InitialValue = *m_Value;
			m_Started = true;
		}

		if (!isFinisced())
		{
			m_Elapsed += dt;
			float delta = m_Elapsed / c_Duration;
			delta = std::clamp(delta, 0.0f, 1.0f);
			// TODO different interpolation
			*m_Value = static_cast<Type>(m_InitialValue * (1 - delta)) + static_cast<Type>(c_FinalValue * delta);
		}
	}

	bool isFinisced() const override
	{
		return m_Elapsed >= c_Duration;
	}

	void reset() override
	{
		m_Elapsed = 0;
		m_Started = false;
	}

private:
	Type* const m_Value;
	Type m_InitialValue;
	const Type c_FinalValue;
	const float c_Duration;
	float m_Elapsed = 0;
	bool m_Started = false;
};

class CollectionTweener : public Tweener
{
public:
	Ref<IntervalTweener> addIntervalTweener(float duration);

	template <class Type>
	Ref<ValueTweener<Type>> addValueTweener(Type* value, Type finalValue, float duration)
	{
		auto tweener = makeRef<ValueTweener<Type>>(value, finalValue, duration);
		m_Tweeners.emplace_back(tweener);
		return tweener;
	}

	Ref<CallbackTweener> addCallbackTweener(std::function<void()> callback);

protected:
	std::vector<Ref<Tweener>> m_Tweeners;
};

class SequentialTweener : public CollectionTweener
{
public:
	void step(float dt) override;
	bool isFinisced() const override;
	void reset() override;

	Ref<ParallelTweener> addParallelTweener();

private:
	u64 m_Index = 0;
};

class ParallelTweener : public CollectionTweener
{
public:
	void step(float dt) override;
	bool isFinisced() const override;
	void reset() override;

	Ref<SequentialTweener> addSequentialTweener();
};

template<class Type>
Ref<ValueTweener<Type>> Tween::addValueTweener(Type* value, Type finalValue, float duration)
{
	return m_Tweener->addValueTweener(value, finalValue, duration);
}

class CallbackTweener : public Tweener
{
public:
	CallbackTweener(std::function<void()> callback);

	void step(float dt) override;
	bool isFinisced() const override;
	void reset() override;
private:
	std::function<void()> m_Callback;
	bool m_Started = false;
};

} // namespace vulture
