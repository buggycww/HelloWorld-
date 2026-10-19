#pragma once

#include <map>
#include "Scene.h"
#include "DiceInterlude.h"

// All game content lives in these two builders -- the engine (Game) never changes
// when the story does.
std::map<int, Scene> BuildStory();
std::multimap<DiceRules, DiceInterlude> BuildInterludes();
