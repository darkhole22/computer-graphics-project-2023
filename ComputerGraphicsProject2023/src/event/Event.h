#pragma once

#include <vector>
#include <functional>

namespace computergraphicsproject {

template<class Event> class EventHandler
{
public:
    using Callback = std::function<void(const Event &)>;

    EventHandler() = default;

    inline void addCallback(Callback callback)
    {
        m_Callbacks.push_back(callback);
    }

    inline void emit(const Event &event) const
    {
        for (auto &callback: m_Callbacks)
        {
            callback(event);
        }
    }

private:
    std::vector<Callback> m_Callbacks;
};

#define EVENT(E)                                                                    \
    private:                                                                        \
    EventHandler<E> m_EventHandler_##E;                                             \
    inline void emit(const E& e) { m_EventHandler_##E .emit(e); }                   \
    public:                                                                         \
    inline void addCallback(EventHandler<E>::Callback cb) { m_EventHandler_##E .addCallback(cb); }
}
