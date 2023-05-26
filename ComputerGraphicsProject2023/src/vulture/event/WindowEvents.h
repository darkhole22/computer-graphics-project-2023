#pragma once

#include <cstdint>

namespace vulture {

struct KeyEvent
{
    int32_t keyCode;
};

struct KeyPressedEvent : public KeyEvent
{
    bool isHolding = false;
};

struct KeyReleasedEvent : public KeyEvent {};

struct WindowResizedEvent
{
	u32 width;
	u32 height;
};

} // namespace vulture