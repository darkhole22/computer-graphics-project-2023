#include "EventBus.h"

namespace game {

Ref<EventBus> EventBus::s_Instance{};

void EventBus::init()
{
	s_Instance = Ref<EventBus>(new EventBus());
}

void EventBus::cleanup()
{
	s_Instance.reset();
}

}