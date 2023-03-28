#pragma once

#include <vector>
#include <tuple>
#include <functional>

typedef uint32_t CallbackReference;

template<class _Event>
class EventHandler
{
public:
	using _CallbackType = std::function<void(const _Event&)>;

	EventHandler() {}

	inline CallbackReference addCallback(_CallbackType callback)
	{
		CallbackReference reference = m_LastReference++;
		m_Callbacks.push_back({ reference, callback });
		return reference;
	}

	inline void removeCallback(CallbackReference callbackReference)
	{
		auto it = std::remove_if(m_Callbacks.begin(), m_Callbacks.end(), 
			[&](const std::pair<CallbackReference, _CallbackType>& p) {
			return p.first == callbackReference;
		});
		m_Callbacks.erase(it, m_Callbacks.end());
	}

	inline void emit(const _Event& event) const
	{
		for (auto& [ref, callback] : m_Callbacks)
		{
			callback(event);
		}
	}

private:
	CallbackReference m_LastReference = 0;
	std::vector<std::pair<CallbackReference, _CallbackType>> m_Callbacks;
	// std::vector<void(const _Event&)> m_Functions;
};

#define EVENT_CLASS template<class _Event> inline void removeCallback(CallbackReference c) { static_assert(false); }

#define EVENT(E) private: EventHandler<E> m_EventHandler_##E;\
	inline void emit(const E& e) { m_EventHandler_##E .emit(e); }\
	public: inline CallbackReference addCallback(EventHandler<E>::_CallbackType c)\
		 { return m_EventHandler_##E .addCallback(c); }\
	template<> inline void removeCallback<E>(CallbackReference c)\
		{ m_EventHandler_##E .removeCallback(c); }
