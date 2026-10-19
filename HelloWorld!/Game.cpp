#include "Game.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>
#include "ConsoleUI.h"
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
		if (choice < 0)
			return;  // input stream closed; leave quietly instead of looping
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
	std::cout << "\n";
	ConsoleUI::WriteRule();
	std::cout << "\n";
	ConsoleUI::WriteWrapped(scene.text, "  ", "  ");
	std::cout << "\n";
	ConsoleUI::WriteRule();
	std::cout << "\n";

	const std::string stats = "Stats: Health " + std::to_string(player.GetStat(StatType::Health))
		+ " | Rizz " + std::to_string(player.GetStat(StatType::Rizz))
		+ " | Stamina " + std::to_string(player.GetStat(StatType::Stamina))
		+ " | Greed " + std::to_string(player.GetStat(StatType::Greed));
	ConsoleUI::WriteWrapped(stats, "  ", "  ");
	std::cout << "\n";
}

int Game::ReadChoice(const Scene& scene)
{
	while (true)
	{
		for (std::size_t i = 0; i < scene.options.size(); ++i)
		{
			const Choice& opt = scene.options[i];
			std::string text = opt.text;

			if (!IsSelectable(opt))
			{
				text += "  (requires ";
				for (std::size_t j = 0; j < opt.requirements.size(); ++j)
				{
					text += NameOf(opt.requirements[j].stat);
					text += " ";
					text += std::to_string(opt.requirements[j].minimum);
					text += (j + 1 == opt.requirements.size()) ? ")" : ", ";
				}
			}

			const std::string prefix = "  [" + std::to_string(i + 1) + "] ";
			ConsoleUI::WriteWrapped(text, prefix, std::string(prefix.size(), ' '));
		}

		std::cout << "  [r] Roll the dice (" << Dice::DICE_STAMINA_COST << " stamina)\n";
		std::cout << "\nInput: ";

		std::string input;
		if (!std::getline(std::cin, input))
			return -1;

		if (input == "r" || input == "R")
		{
			TryDiceInterlude();
			if (player.IsDead()) return 0; // exit immediately if player dead
			continue; // re-prompt same scene
		}

		if (input.size() == 1 && input[0] >= '1' && input[0] <= '9')
		{
			const std::size_t index = static_cast<std::size_t>(input[0] - '1');
			if (index < scene.options.size() && IsSelectable(scene.options[index]))
				return static_cast<int>(index);
		}

		std::cout << "You can't do that.\n\n";
	}
}

bool Game::IsSelectable(const Choice& choice) const
{
	bool requirementsFulfilled = true;
	for (const auto& requirement : choice.requirements)
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
	ConsoleUI::WriteWrapped(matchingInterludes[selection].text, "  ", "  ");
	ApplyEffects(matchingInterludes[selection].effects);
	std::cout << "\n";
}

void Game::ApplyEffects(const std::vector<StatEffect>& effects)
{
	for (const StatEffect& effect : effects)
		player.ChangeStat(effect.stat, effect.amount);
}
