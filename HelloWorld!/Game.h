#pragma once

#include <map>
#include <iostream>
#include "ConsoleUI.h"
#include "Scene.h"
#include "DiceInterlude.h"
#include "Player.h"

class Game
{
public:
	Game(std::map<int, Scene> scenes, std::multimap<DiceRules, DiceInterlude> interludeMap) :
		scenes(std::move(scenes)), interludeMap(std::move(interludeMap))
	{
		subs.push_back(player.OnPlayerDied.Subscribe(
			[]()
			{
				std::cout << "\n";
				ConsoleUI::WriteWrapped("You died. It's alright. The user starts another window... You were replaceable anyway.",
					"  ", "  ");
			}));

		subs.push_back(player.OnStatChanged.Subscribe(
			[](StatType s, int oldV, int newV)
			{
				int delta = newV - oldV;
				std::cout << "   [" << NameOf(s) << (delta > 0 ? " +" : " ") << delta << "]\n";
			}));
	}

	void Play();

private:
	void DisplayScene(const Scene& scene) const;
	int ReadChoice(const Scene& scene);   // returns -1 when input closes
	bool IsSelectable(const Choice& choice) const;
	void TryDiceInterlude();
	void ApplyEffects(const std::vector<StatEffect>& effects);

	std::map<int, Scene> scenes;
	std::multimap<DiceRules, DiceInterlude> interludeMap;
	Player player;
	std::vector<Subscription> subs;
	int currentSceneId;
};

