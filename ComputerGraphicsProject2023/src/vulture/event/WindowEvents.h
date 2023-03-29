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

} // namespace vulture