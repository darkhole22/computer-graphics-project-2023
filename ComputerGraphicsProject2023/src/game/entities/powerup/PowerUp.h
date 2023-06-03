#pragma once

namespace game {

enum class PowerUpType
{
	None, HealthUp
};

class PowerUpData
{
public:
	virtual PowerUpType getType() const;

	virtual ~PowerUpData();
};

} // namespace game
