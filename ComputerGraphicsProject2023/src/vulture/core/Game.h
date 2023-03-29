#pragma once

namespace vulture {

class Game
{
public:
    virtual void setup() = 0;
    virtual void update(float dt) = 0;
};

} // namespace vulture