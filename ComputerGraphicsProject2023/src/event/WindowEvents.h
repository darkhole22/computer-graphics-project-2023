#pragma once

#include <cstdint>

namespace computergraphicsproject {

struct KeyEvent
{
    int32_t keyCode;
};

struct KeyPressedEvent : public KeyEvent
{
    bool isHolding = false;
};

struct KeyReleasedEvent : public KeyEvent {};

}