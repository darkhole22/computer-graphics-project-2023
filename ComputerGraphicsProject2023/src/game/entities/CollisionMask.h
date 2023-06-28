#pragma once

#include "vulture/scene/physics/HitBox.h"

namespace game {

using namespace vulture;

// Convenience aliases.

constexpr u64 PLAYER_MASK = BitMask::BIT0;
constexpr u64 ENEMY_MASK = BitMask::BIT1;
constexpr u64 PLAYER_BULLET_MASK = BitMask::BIT2;
constexpr u64 POWER_UP_MASK = BitMask::BIT3;
constexpr u64 EXPLOSION_MASK = BitMask::BIT4;

}
