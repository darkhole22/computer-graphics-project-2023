#pragma once

#include "vulture/util/Types.h"

#include <vector>
#include <unordered_map>
#include <functional>

namespace vulture {

template<class Event> class EventHandler
{
public:
	using Callback = std::function<void(const Event&)>;
	struct CallbackReference
	{
		u64 value = 0;
	};

	EventHandler() = default;

	inline CallbackReference addCallback(Callback callback)
	{
		m_NextReference.value++;
		m_Callbacks.insert({m_NextReference.value, callback });
		return CallbackReference{m_NextReference.value };
	}

	inline void emit(const Event& event) const
	{
		for (auto& [id, callback] : m_Callbacks)
		{
			callback(event);
		}
	}

	inline void removeCallback(CallbackReference ref)
	{
		m_Callbacks.erase(ref.value);
	}

	~EventHandler() = default;
private:
	CallbackReference m_NextReference;
	std::unordered_map<u64, Callback> m_Callbacks;
};

#define EVENT(E)                                                                        \
    private:                                                                            \
    EventHandler<E> m_EventHandler_##E;                                                 \
    inline void emit(const E& e) { m_EventHandler_##E .emit(e); }                       \
    public:                                                                             \
    inline EventHandler<E>::CallbackReference addCallback(EventHandler<E>::Callback cb) \
		{ return m_EventHandler_##E .addCallback(cb); }                                 \
	inline void removeCallback(EventHandler<E>::CallbackReference cr)                   \
			{ m_EventHandler_##E .removeCallback(cr); }

#define STATIC_EVENT(E)                                                                        \
    private:                                                                                   \
	EventHandler<E> m_EventHandler_##E;                                                        \
    public:                                                                                    \
	inline static void emit(const E& e) { if (s_Instance) s_Instance->m_EventHandler_##E .emit(e); }           \
	inline static EventHandler<E>::CallbackReference addCallback(EventHandler<E>::Callback cb) \
			{ return (s_Instance) ? s_Instance->m_EventHandler_##E .addCallback(cb) : EventHandler<E>::CallbackReference{}; }                        \
	inline static void removeCallback(EventHandler<E>::CallbackReference cr)                   \
			{ if (s_Instance) s_Instance->m_EventHandler_##E .removeCallback(cr); }

} // namespace vulture
