#pragma once

#include <string>
#include <vector>
#include "Dice.h"
#include "StatTypes.h"

struct DiceInterlude
{
	std::string name;
	std::string text;
	std::vector<DiceRules> triggers;
	std::vector<StatEffect> effects;
};

