#include "Game.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include "DiceInterlude.h"
#include <random>

void Game::Play()
{
	currentSceneId = SCENE_START;

	while (currentSceneId != SCENE_END && !player.IsDead())
	{
		const Scene& scene = scenes[currentSceneId];
		DisplayScene(scene);
		int choice = ReadChoice(scene);
		if (player.IsDead())
			break;   // an interlude can kill the player mid-prompt
		ApplyEffects(scene.options[choice].statEffects);
		currentSceneId = scene.options[choice].nextSceneId;
	}

	if (!player.IsDead())
		std::cout << "\nThe End. Thanks for playing!!\n";

}

void Game::DisplayScene(const Scene& scene) const
{
	std::cout << "\n------------------------------------------------------------\n\n";
	std::cout << scene.text << "\n\n";
	std::cout << "------------------------------------------------------------\n\n";

	std::cout << "  Stats: Health " << player.GetStat(StatType::Health)
		<< " | Rizz " << player.GetStat(StatType::Rizz)
		<< " | Stamina " << player.GetStat(StatType::Stamina)
		<< " | Greed " << player.GetStat(StatType::Greed) << "\n\n";
}

int Game::ReadChoice(const Scene& scene)
{
	while (true)
	{
		for (std::size_t i = 0; i < 2; ++i)
		{
			const Choice& opt = scene.options[i];
			std::cout << "  [" << (i + 1) << "] " << opt.text;

			if (!IsSelectable(opt))
			{
				const auto& reqs = opt.requirements;
				std::cout << "  (requires ";
				for (std::size_t j = 0; j < reqs.size(); ++j)
				{
					std::cout << NameOf(reqs[j].stat) << " " << reqs[j].minimum;
					std::cout << (j + 1 == reqs.size() ? ")" : ", ");
				}
			}
			std::cout << "\n";
		}
		std::cout << "  [r] Roll the dice (" << Dice::DICE_STAMINA_COST << " stamina)\n";
		
		std::cout << "\nInput: ";
		std::string input;
		std::getline(std::cin, input);

		if (input == "r" || input == "R")
		{
			TryDiceInterlude();
			if (player.IsDead()) return 0; // exit immediately if player dead
			continue; // re-prompt same scene
		}

		if (input == "1" && IsSelectable(scene.options[0])) return 0;
		if (input == "2" && IsSelectable(scene.options[1])) return 1;
		std::cout << "You can't do that.\n";
	}
}

bool Game::IsSelectable(const Choice& choice) const
{
	bool requirementsFulfilled = true;
	for (auto& requirement : choice.requirements)
	{
		if (player.GetStat(requirement.stat) < requirement.minimum) requirementsFulfilled = false;
	}

	return !choice.hasRequirements || requirementsFulfilled;
}

void Game::TryDiceInterlude()
{
	if (player.GetStat(StatType::Stamina) < Dice::DICE_STAMINA_COST)
	{
		std::cout << "You're too exhausted to gamble.\n";
		return;
	}

	auto [roll, rules] = Dice::RollDice();

	player.ChangeStat(StatType::Stamina, -Dice::DICE_STAMINA_COST);

	std::cout << "\nDice Rolled...\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "It rolls...\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "Still rolling...\n";
	std::this_thread::sleep_for(std::chrono::milliseconds(700));
	std::cout << "\nIt lands on " << roll << "!\n\n";

	std::vector<DiceInterlude> matchingInterludes = {};
	for (DiceRules rule : rules)
	{
		auto range = interludeMap.equal_range(rule);
		for (auto itr = range.first; itr != range.second; ++itr)
			matchingInterludes.push_back(itr->second);
	}

	if (matchingInterludes.empty())
	{
		std::cout << "Oops. You get nothing. That's why we don't gamble, kids.\n";
		return;
	}

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(0, static_cast<int>(matchingInterludes.size()) - 1);
	int selection = distribution(generator);

	std::cout << "Dice event: " << matchingInterludes[selection].name << "!\n\n";
	std::cout << matchingInterludes[selection].text << "\n";
	ApplyEffects(matchingInterludes[selection].effects);
	std::cout << "\n";
}

void Game::ApplyEffects(const std::vector<StatEffect>& effects)
{
	for (const StatEffect& effect : effects)
		player.ChangeStat(effect.stat, effect.amount);
}
