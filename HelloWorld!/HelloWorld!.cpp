// HelloWorld!.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <string>
#include "Story.h"
#include "Game.h"

namespace
{
	// validate story
	bool StoryIsValid(const std::map<int, Scene>& scenes)
	{
		bool ok = true;

		for (const auto& entry : scenes)
		{
			for (const Choice& option : entry.second.options)
			{
				if (option.nextSceneId != SCENE_END && scenes.count(option.nextSceneId) == 0)
				{
					std::cout << "Story error: scene " << entry.first
						<< " points to missing scene " << option.nextSceneId << "\n";
					ok = false;
				}
			}
		}

		return ok;
	}
}

int main()
{
	std::cout << "============================================================\n";
	std::cout << "                  H e l l o   W o r l d !\n";
	std::cout << "============================================================\n";
	std::cout << "        A tiny story about a program that wakes up.\n";
	std::cout << "          Its life is shaped by the choices you make.\n\n";
	std::cout << "       [options: 1 / 2]      [r] roll the dice\n\n";
	std::cout << "============================================================\n";

	std::map<int, Scene> scenes = BuildStory();
	std::multimap<DiceRules, DiceInterlude> interludes = BuildInterludes();

	if (!StoryIsValid(scenes))
	{
		std::cout << "The story is broken. Fix the scenes listed above before playing.\n";
		return 1;
	}

	std::cout << "\nPress Enter to begin your story...";
	std::string start;
	if (!std::getline(std::cin, start))
		return 0;

	Game game(std::move(scenes), std::move(interludes));
	game.Play();

	std::cout << "\nPress Enter to close the window...";
	std::string wait;
	std::getline(std::cin, wait);

	return 0;
}
