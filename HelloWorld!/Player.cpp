#include "Player.h"

int Player::GetStat(StatType stat) const
{
	switch (stat)
	{
	case StatType::Health:  
		return stats.Health;
	case StatType::Rizz:    
		return stats.Rizz;
	case StatType::Stamina: 
		return stats.Stamina;
	case StatType::Greed:   
		return stats.Greed;
	}
	return 0;
}

int& Player::FieldForStat(StatType stat)
{
	switch (stat)
	{
	case StatType::Health:  return stats.Health;
	case StatType::Rizz:    return stats.Rizz;
	case StatType::Stamina: return stats.Stamina;
	case StatType::Greed:   return stats.Greed;
	}
	return stats.Health;
}

int Player::MaxForStat(StatType stat) const
{
	switch (stat)
	{
	case StatType::Health:  return stats.MaxHealth;
	case StatType::Rizz:    return stats.MaxRizz;
	case StatType::Stamina: return stats.MaxStamina;
	case StatType::Greed:   return stats.MaxGreed;
	}
	return 0;
}

void Player::ChangeStat(StatType stat, int amount)
{
	int oldVal = GetStat(stat);
	int newVal = oldVal + amount;

	if (newVal < 0) newVal = 0;
	if (newVal > MaxForStat(stat)) newVal = MaxForStat(stat);

	if (newVal == oldVal)
		return;

	bool wasAlive = !IsDead();

	FieldForStat(stat) = newVal;
	OnStatChanged.Publish(stat, oldVal, newVal);

	if (wasAlive && IsDead())
		OnPlayerDied.Publish();
}

bool Player::IsDead() const
{
	return stats.Health <= 0;
}