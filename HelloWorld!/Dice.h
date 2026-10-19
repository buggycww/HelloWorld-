#pragma once

#include <vector>
#include <tuple>

enum class DiceRules
{
	None,
	Odd,
	Even,
	One, Two, Three, Four, Five, Six
};

class Dice
{
public:
	static std::tuple<int, std::vector<DiceRules>> RollDice();
	static constexpr int numFaces = 6;
	static constexpr int DICE_STAMINA_COST = 10;

private:
	static DiceRules GetFaceRule(int roll);
};


