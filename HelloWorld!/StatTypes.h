#pragma once

#include <string>

enum class StatType 
{
	Health, 
	Rizz,
	Stamina,
	Greed
};

inline const char* NameOf(StatType stat)
{
	switch (stat)
	{
	case StatType::Health:  
		return "Health";
	case StatType::Rizz:    
		return "Rizz";
	case StatType::Stamina: 
		return "Stamina";
	case StatType::Greed:   
		return "Greed";
	}
	return "?";
}

struct StatEffect
{
	StatType stat;
	int amount;
};

struct Requirement
{
	StatType stat;
	int minimum;
};