#pragma once

#include "Scene.h"
#include <map>
#include <vector>
#include "DiceInterlude.h"
#include "Dice.h"

// anonymous namespace: private to this file
// internal linkage
namespace
{
	Choice Simple(std::string text, int next, std::vector<StatEffect> effects = {})
	{
		Choice c;
		c.text = std::move(text);
		c.nextSceneId = next;
		c.statEffects = std::move(effects);
		return c;
	}

	Choice Locked(std::string text, int next, std::vector<Requirement> reqs, std::vector<StatEffect> effects = {})
	{
		Choice c = Simple(std::move(text), next, std::move(effects));
		if (!reqs.empty())
		{
			c.requirements = std::move(reqs);
			c.hasRequirements = true;
		}
		return c;
	}
}

std::multimap<DiceRules, DiceInterlude> BuildInterludes()
{
	std::multimap<DiceRules, DiceInterlude> interludeMap;

	// damage events
	DiceInterlude SystemBug;
	SystemBug.name = "System Bug";
	SystemBug.text = "Red flashes across the screen. ...";
	SystemBug.triggers = { {DiceRules::One} };
	SystemBug.effects = { {StatType::Health, -20} };

	for (DiceRules rule : SystemBug.triggers)
	{
		interludeMap.emplace(rule, SystemBug);
	}

	DiceInterlude NewAgent;
	SystemBug.name = "The birth of a new Agent";
	SystemBug.text = "A blinking light. A dot, unasumming, grows into the size of a pea. Two round eyes blink open ...";
	SystemBug.triggers = { {DiceRules::One} };
	SystemBug.effects = { {StatType::Health, -10}, {StatType::Rizz, -10} };

	for (DiceRules rule : NewAgent.triggers)
	{
		interludeMap.emplace(rule, NewAgent);
	}

	// reward events

	return interludeMap;
}

std::map<int, Scene> BuildStory()
{
	std::map<int, Scene> scenes;

	// redo this to fit the story
	Scene tavern;
	tavern.id = 1;
	tavern.text = "You wake face-down on the tavern floor. A stranger watches you with amusement.";
	tavern.options[0] = Simple("Stagger over and buy them a drink", 2);
	tavern.options[1] = Locked("Flash a dazzling smile", 3, { { StatType::Rizz, 40 } }, { { StatType::Stamina, -10 } });
	scenes[1] = tavern;

	return scenes;
}