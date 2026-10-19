#pragma once

#include "Event.h"
#include "StatTypes.h"

struct Stats
{
	// A newborn sprite: whole, but with no game yet and few clock cycles to spare.
	int Health = 100;
	int MaxHealth = 100;
	int Rizz = 10;
	int MaxRizz = 100;
	int Stamina = 25;
	int MaxStamina = 100;
	int Greed = 0;
	int MaxGreed = 9999;
};

class Player
{
public:
	Player() = default;
	
	int  GetStat(StatType stat) const;
	bool IsDead() const;

	void ChangeStat(StatType stat, int amount);

	Event<> OnPlayerDied;
	Event<StatType, int, int> OnStatChanged;

private:
	int& FieldForStat(StatType stat);
	int  MaxForStat(StatType stat) const;

	Stats stats;
};

