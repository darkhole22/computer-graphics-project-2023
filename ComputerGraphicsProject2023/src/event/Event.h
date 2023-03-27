#pragma once

#include <vector>
#include <functional>

template<class _Event>
class EventHandler
{
public:
	using _CallbackType = std::function<void(const _Event&)>;

	EventHandler() {}

	inline void addCallback(_CallbackType callback)
	{
		m_callbacks.push_back(callback);
	}

	inline void removeCallback(_CallbackType callback)
	{
		// TODO remove callback m_callbacks.erase(callback);
	}

	inline void emit(const _Event& event) const
	{
		for (auto& callback : m_callbacks)
		{
			callback(event);
		}
	}

private:
	std::vector<_CallbackType> m_callbacks;
};

#define EVENT(E) private: EventHandler<E> m_EventHandler_##E;\
	inline void emit(const E& e) { m_EventHandler_##E .emit(e); }\
	public: inline void addCallback(EventHandler<E>::_CallbackType c)\
		 { m_EventHandler_##E .addCallback(c); }\
	inline void removeCallback(EventHandler<E>::_CallbackType c)\
		{ m_EventHandler_##E .removeCallback(c); }
