#pragma once

#include <string>
#include <vector>
#include "StatTypes.h"

constexpr int SCENE_END = -1;
constexpr int SCENE_START = 1;

struct Choice
{
	std::string text;
	int nextSceneId;
	std::vector<StatEffect> statEffects;
	std::vector<Requirement> requirements;
	bool hasRequirements = false;
};

struct Scene
{
	int id;
	std::string text;
	Choice options[2];
};
