#pragma once

namespace game {

enum class PowerUpType
{
	None, HealthUp, DoubleScore, Bomb
};

class PowerUpData
{
public:
	virtual PowerUpType getType() const;

	virtual ~PowerUpData();
};

} // namespace game
