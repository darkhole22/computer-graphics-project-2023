#pragma once

#include "vulture/scene/physics/HitBox.h"

namespace game {

using namespace vulture;

// Convenience aliases.

constexpr u64 PLAYER_MASK = BitMask::BIT0;
constexpr u64 ENEMY_MASK = BitMask::BIT1;
constexpr u64 PLAYER_BULLET_MASK = BitMask::BIT2;

}
