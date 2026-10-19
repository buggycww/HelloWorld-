#include "Dice.h"
#include <random>

std::tuple<int, std::vector<DiceRules>> Dice::RollDice()
{
    std::vector<DiceRules> rules = {};

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(1, 6);
    int roll = distribution(generator);

    if (Dice::GetFaceRule(roll) != DiceRules::None) rules.push_back(Dice::GetFaceRule(roll));
    roll % 2 == 0 ? rules.push_back(DiceRules::Even) : rules.push_back(DiceRules::Odd);

    return { roll, rules };
}

DiceRules Dice::GetFaceRule(int roll)
{
    switch (roll)
    {
    case 1:
        return DiceRules::One;
    case 2:
        return DiceRules::Two;
    case 3:
        return DiceRules::Three;
    case 4:
        return DiceRules::Four;
    case 5:
        return DiceRules::Five;
    case 6:
        return DiceRules::Six;
    }
    return DiceRules::None;
}
