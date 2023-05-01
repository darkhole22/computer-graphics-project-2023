#pragma once

#include "vulture/core/Core.h"

#include <vector>
#include <algorithm>
#include <functional>

namespace vulture {

/**
* @brief Tweener object performs a specific animation tasks,
* like interpolating a value or calling a function.
*/
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
template <class Type> class MethodTweener;

/**
* @brief A Tween is a class that manages tweeners. It can be used to animate various types
* of objects, properties, and functions.
*/
class Tween
{
public:
	/**
	* @brief Construct a Tween.
	* This shoul be called only if you want to manage the Tween manually.
	* Consider using Scene::makeTween() instead.
	*/
	Tween();

	/**
	* @brief Step the animationof the Tween by a specified amount.
	* If the Tween is not running or it is stopped this has no effect.
	* 
	* This shoul be called only if you are managing the Tween manually.
	* 
	* @param dt: the delta time of the animation in seconds.
	*/
	void step(float dt);

	/**
	* @brief Checks if the Tween is runing.
	* 
	* @returns true if the tween is running.
	*/
	bool isRunning() const;

	/**
	* @brief Checks if the Tween is beeing managed by the scene.
	*
	* @returns true if the tween is valid.
	*/
	bool isValid() const;

	/**
	* @brief Pauses the animation at their current state.
	*/
	void pause();

	/**
	* @brief Starts playing the animation from their current state.
	*/
	void play();

	/**
	* @brief Stop the Tween. If the tween is stopped it cannot restart.
	* Consider using reset().
	*/
	void stop();

	/*
	* @brief Reset the animation.
	* 
	* @param autoStart: If true the animation will automatically start, otherwise the animation will be stopped.
	* Default: true.
	*/
	void reset(bool autoStart = true);

	/**
	* @brief Sets the number of times the animation should loop.
	* If the loops parameter is set to 0 (default), the animation will loop indefinitely.
	* 
	* @param loops the number of loop of the animation. If 0 the animation will loop indefinitely.
	* 
	* @return a pointer to itself.
	*/
	Tween* loop(u64 loops = 0);

	/**
	* @brief Creates and append a new IntervalTweener.
	*
	* @param duration: the duration of the animation in seconds.
	*
	* @return a referance to the created Tweener.
	*/
	Ref<IntervalTweener> addIntervalTweener(float duration);

	/**
	* @brief Creates and append a new ValueTweener.
	* This ValueTweener doesen't manage the memory of the provided value. 
	*
	* @param value: a pointer to the value to modify. This should be remain a valid pinter for the entire lifetime of the Tween.
	* @param finalValue: the value at the end of the animation.
	* @param duration: the duration of the animation in seconds.
	*
	* @return a referance to the created Tweener.
	*/
	template<class Type>
	Ref<ValueTweener<Type>> addValueTweener(Type* value, Type finalValue, float duration);

	/**
	* @brief Creates and append a new ParallelTweener.
	*
	* @return a referance to the created Tweener.
	*/
	Ref<ParallelTweener> addParallelTweener();

	/**
	* @brief Creates and append a new CallbackTweener.
	*
	* @param callback: the callback that will be called.
	*
	* @return a referance to the created Tweener.
	*/
	Ref<CallbackTweener> addCallbackTweener(std::function<void()> callback);

	/**
	* @brief Creates and append a new MethodTweener.
	* This Tweener will call the provided method each step with an interpolated value as paramether.
	*
	* @param callback: the callback that will be called.
	* @param initialValue: the value at the start of the animation.
	* @param finalValue: the value at the end of the animation.
	* @param duration: the duration of the animation in seconds.
	*
	* @return a referance to the created Tweener.
	*/
	template <class Type>
	Ref<MethodTweener<Type>> addMethodTweener(std::function<void(Type)> callback, Type initialValue, Type finalValue, float duration);

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

	template <class Type>
	Ref<MethodTweener<Type>> addMethodTweener(std::function<void(Type)> callback, Type initialValue, Type finalValue, float duration)
	{
		auto tweener = makeRef<MethodTweener<Type>>(callback, initialValue, finalValue, duration);
		m_Tweeners.emplace_back(tweener);
		return tweener;
	}

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

template<class Type>
inline Ref<MethodTweener<Type>> Tween::addMethodTweener(std::function<void(Type)> callback, Type initialValue, Type finalValue, float duration)
{
	return m_Tweener->addMethodTweener(callback, initialValue, finalValue, duration);
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

template <class Type>
class MethodTweener : public Tweener
{
public:
	MethodTweener(std::function<void(Type)> callback, Type initialValue, Type finalValue, float duration) :
		m_Callback(callback), c_InitialValue(initialValue), m_Value(initialValue), m_Interpolator(&m_Value, finalValue, duration)
	{
	}

	void step(float dt) override
	{
		if (!isFinisced())
		{
			m_Interpolator.step(dt);
			m_Callback(m_Value);
		}
	}

	bool isFinisced() const override
	{
		return m_Interpolator.isFinisced();
	}

	void reset() override
	{
		m_Value = c_InitialValue;
		m_Interpolator.reset();
	}
private:
	std::function<void(Type)> m_Callback;
	Type c_InitialValue;
	Type m_Value;
	ValueTweener<Type> m_Interpolator;
};

} // namespace vulture
