#pragma once

namespace game {

enum class PowerUpType
{
	None, HealthUp, DoubleExp
};

class PowerUpData
{
public:
	virtual PowerUpType getType() const;

	virtual ~PowerUpData();
};

} // namespace game
