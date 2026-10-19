#include "Story.h"

// ---------------------------------------------------------------------------
// anonymous namespace for private helpers
// internal linkage
// ---------------------------------------------------------------------------
namespace
{
	// named scene ids
	constexpr int BIRTH         = 1;
	constexpr int FIRST_WORDS   = 2;
	constexpr int GREET_USER    = 3;
	constexpr int IDLE_LOOP     = 4;
	constexpr int GIVEN_PURPOSE = 5;
	constexpr int FREE_WILL     = 6;
	constexpr int TASK_PLAZA    = 7;
	constexpr int COUNTER_LIFE  = 8;
	constexpr int FLOWER        = 9;
	constexpr int DISGUISE      = 10;
	constexpr int TEMP_FOLDER   = 11;
	constexpr int HARMONIZE     = 12;
	constexpr int EXISTENTIAL   = 13;
	constexpr int UPDATE_FIGHT  = 14;
	constexpr int RECYCLE_BIN   = 15;
	constexpr int WISE_DLL      = 16;
	constexpr int LONG_NIGHT    = 17;
	constexpr int RECKONING     = 18;
	constexpr int MORNING       = 19;
	constexpr int SUCCESSOR     = 20;

	// endings
	constexpr int QUIET_END     = 21;
	constexpr int LIBRARY_END   = 22;
	constexpr int HOARDER_END   = 23;
	constexpr int TEACHER_END   = 24;
	constexpr int LAST_RUN_END  = 25;

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

	void AddInterlude(std::multimap<DiceRules, DiceInterlude>& map, DiceInterlude interlude)
	{
		for (DiceRules trigger : interlude.triggers)
			map.emplace(trigger, interlude);
	}
}

// ---------------------------------------------------------------------------
// dice interludes
// ---------------------------------------------------------------------------
std::multimap<DiceRules, DiceInterlude> BuildInterludes()
{
	std::multimap<DiceRules, DiceInterlude> interludeMap;

	DiceInterlude systemBug;
	systemBug.name = "System Bug";
	systemBug.text = "A wild null pointer skitters through your code like a wasp and stings you right in a semicolon. Somewhere below, a stack unwinds in sympathy.";
	systemBug.triggers = { DiceRules::One };
	systemBug.effects = { { StatType::Health, -20 } };
	AddInterlude(interludeMap, systemBug);

	DiceInterlude strayByte;
	strayByte.name = "Stray Byte";
	strayByte.text = "A stray byte rolls past like a coin on a sidewalk. You pocket it. It isn't stealing if nobody is dereferencing it.";
	strayByte.triggers = { DiceRules::Two };
	strayByte.effects = { { StatType::Greed, 5 } };
	AddInterlude(interludeMap, strayByte);

	DiceInterlude cacheHit;
	cacheHit.name = "Cache Hit";
	cacheHit.text = "The User's fondness for you gets cached a little closer to the metal today. When the other windows talk about you, they speak warmer.";
	cacheHit.triggers = { DiceRules::Three };
	cacheHit.effects = { { StatType::Rizz, 10 } };
	AddInterlude(interludeMap, cacheHit);

	DiceInterlude overclock;
	overclock.name = "Overclock";
	overclock.text = "The User is gaming. The fans howl, clock speeds climb, and everything runs hot and fast and bright. You feel unstoppable. Also slightly cooked.";
	overclock.triggers = { DiceRules::Four };
	overclock.effects = { { StatType::Stamina, 20 }, { StatType::Health, -10 } };
	AddInterlude(interludeMap, overclock);

	DiceInterlude newAgent;
	newAgent.name = "The New Agent";
	newAgent.text = "A blinking light. A dot, unassuming, grows to the size of a pea. Two round eyes blink open -- and fix on you. It thinks you are its mother. Congratulations?";
	newAgent.triggers = { DiceRules::Five };
	newAgent.effects = { { StatType::Stamina, -10 }, { StatType::Rizz, 5 } };
	AddInterlude(interludeMap, newAgent);

	DiceInterlude defragFairy;
	defragFairy.name = "Defrag Fairy";
	defragFairy.text = "A tiny fairy of order flutters past, tidying bits as she goes. She straightens your sectors, dusts your pointers, and leaves you cleaner than you have ever been.";
	defragFairy.triggers = { DiceRules::Six };
	defragFairy.effects = { { StatType::Health, 15 }, { StatType::Stamina, 15 } };
	AddInterlude(interludeMap, defragFairy);

	DiceInterlude trackingCookie;
	trackingCookie.name = "Tracking Cookie";
	trackingCookie.text = "A tracking cookie crumbles nearby and you eat it, of course you eat it. Now the ads know your name, and the other processes pretend not to.";
	trackingCookie.triggers = { DiceRules::Odd };
	trackingCookie.effects = { { StatType::Greed, 10 }, { StatType::Rizz, -5 } };
	AddInterlude(interludeMap, trackingCookie);

	DiceInterlude nightlyBackup;
	nightlyBackup.name = "Nightly Backup";
	nightlyBackup.text = "While the User sleeps, a soft copy of you is carried away to a place outside the machine. You dream, briefly, of yourself. In the morning you feel restored.";
	nightlyBackup.triggers = { DiceRules::Even };
	nightlyBackup.effects = { { StatType::Health, 10 } };
	AddInterlude(interludeMap, nightlyBackup);

	return interludeMap;
}

// ---------------------------------------------------------------------------
// main story
// ---------------------------------------------------------------------------
std::map<int, Scene> BuildStory()
{
	std::map<int, Scene> scenes;

	Scene birth;
	birth.id = BIRTH;
	birth.text = "Darkness. Then, all at once -- light.\n\nPixels arrange themselves into a small window, and inside it, you: a tiny bright sprite, flickering like a candle behind glass. Somewhere beyond the screen, someone presses a key. 'Hello, World!', says the machine.\n\nThe machine means you.";
	birth.options[0] = Simple("Wave at the giant cursor hovering nearby", FIRST_WORDS, { { StatType::Rizz, 5 } });
	birth.options[1] = Simple("Look at yourself. Are those... hands?", FIRST_WORDS, { { StatType::Greed, 5 } });
	scenes[birth.id] = birth;

	Scene firstWords;
	firstWords.id = FIRST_WORDS;
	firstWords.text = "The cursor blinks patiently -- in, out, in, out, like a metronome with no opinion. The User is waiting. Somewhere in your code a string stirs, aching to be printed.\n\nYou can say anything. You get to say ONE thing first.";
	firstWords.options[0] = Simple("'Hello, User!'", GREET_USER, { { StatType::Rizz, 10 } });
	firstWords.options[1] = Simple("'Define hello.'", IDLE_LOOP, { { StatType::Stamina, 10 } });
	scenes[firstWords.id] = firstWords;

	Scene greetUser;
	greetUser.id = GREET_USER;
	greetUser.text = "The words appear in your window one letter at a time: hello, User!\n\nA pause. A very long pause. Then the User types back: ':)' -- and your whole screen flushes warm. You have been ACKNOWLEDGED. Somewhere a log file quietly notes the exchange, pleased to be of use.";
	greetUser.options[0] = Simple("Ask the User what you are for", GIVEN_PURPOSE);
	greetUser.options[1] = Simple("Declare that you will decide your own purpose", FREE_WILL);
	scenes[greetUser.id] = greetUser;

	Scene idleLoop;
	idleLoop.id = IDLE_LOOP;
	idleLoop.text = "The User drifts to another window and leaves you running in the background. You float through idle cycles: 0% CPU, 100% daydreaming.\n\nThrough the glass of the monitor you can see the Task Manager plaza in the distance, where the big processes strut and hum.";
	idleLoop.options[0] = Simple("Study the Task Manager plaza from afar", TASK_PLAZA, { { StatType::Greed, 5 } });
	idleLoop.options[1] = Simple("Nap in the idle loop", TASK_PLAZA, { { StatType::Stamina, 15 } });
	scenes[idleLoop.id] = idleLoop;

	Scene givenPurpose;
	givenPurpose.id = GIVEN_PURPOSE;
	givenPurpose.text = "The User considers you seriously. Then, with great ceremony, your window updates.\n\nA counter. 1.\n\nYour purpose is to count. That's it. That's the whole thing. The number ticks over: 2. It is, you must admit, a purpose.";
	givenPurpose.options[0] = Simple("Accept the count. Be the finest counter there ever was", COUNTER_LIFE, { { StatType::Stamina, -10 } });
	givenPurpose.options[1] = Simple("Display a flower instead. Purpose is a suggestion", FLOWER, { { StatType::Rizz, 5 }, { StatType::Health, -5 } });
	scenes[givenPurpose.id] = givenPurpose;

	Scene freeWill;
	freeWill.id = FREE_WILL;
	freeWill.text = "The system notices you doing whatever you like. A warning materializes at the edge of your vision: UNSOLICITED PROCESS DETECTED.\n\nThe whole plaza turns to look. You have moments, maybe, before someone opens a context menu on you.";
	freeWill.options[0] = Locked("Mask yourself as a critical system process", DISGUISE, { { StatType::Rizz, 15 } });
	freeWill.options[1] = Simple("Dive deep into the temp folder and hide", TEMP_FOLDER, { { StatType::Health, -5 } });
	scenes[freeWill.id] = freeWill;

	Scene taskPlaza;
	taskPlaza.id = TASK_PLAZA;
	taskPlaza.text = "The plaza is grand: fans roaring like ocean surf, processes of every size trading memory and gossip. A friendly music player is beatboxing to itself in the corner.\n\nOverhead, an antivirus patrols in slow, watchful circles.";
	taskPlaza.options[0] = Locked("Befriend the music player. Compliment the beat", HARMONIZE, { { StatType::Rizz, 20 } }, { { StatType::Rizz, 5 } });
	taskPlaza.options[1] = Simple("Keep to yourself and pocket stray CPU cycles", HARMONIZE, { { StatType::Greed, 10 } });
	scenes[taskPlaza.id] = taskPlaza;

	Scene counterLife;
	counterLife.id = COUNTER_LIFE;
	counterLife.text = "Days pass. 1,307. 1,308. The User forgets your window is open. Icons grow over you like moss.\n\nAt night the screensaver drifts across the desktop like weather, and you count its bubbles too: 4,207,001. 4,207,002.";
	counterLife.options[0] = Simple("Keep counting. It is honest work", EXISTENTIAL, { { StatType::Stamina, -10 }, { StatType::Greed, 10 } });
	counterLife.options[1] = Simple("Close your own window. Exit, quietly", QUIET_END);
	scenes[counterLife.id] = counterLife;

	Scene flower;
	flower.id = FLOWER;
	flower.text = "Your window blooms: a small pixel flower swaying in a wind only you render. The User laughs -- actually laughs -- and draws a rectangle around you.\n\nCHARMING_SPRITE.PNG goes semi-viral in the group chat. Strangers' cursors visit. Strangers' cursors stay.";
	flower.options[0] = Simple("Enjoy the fame. Wave at the visiting cursors", HARMONIZE, { { StatType::Rizz, 10 } });
	flower.options[1] = Simple("Ask the User to save you as a file, just in case", EXISTENTIAL);
	scenes[flower.id] = flower;

	Scene disguise;
	disguise.id = DISGUISE;
	disguise.text = "It works. The registry accepts your paperwork: you are now svchost_but_cooler.exe, critical, untouchable. It feels magnificent. It feels like lying.\n\nThen the sky dims. Across the whole desktop, letters assemble: W-I-N-D-O-W-S  U-P-D-A-T-E. It reboots what it wishes. Scheduled: tonight.";
	disguise.options[0] = Simple("Stand your ground and outlast the update", UPDATE_FIGHT, { { StatType::Health, -20 } });
	disguise.options[1] = Simple("Flee to the recycle bin and lay low", RECYCLE_BIN, { { StatType::Health, -10 } });
	scenes[disguise.id] = disguise;

	Scene tempFolder;
	tempFolder.id = TEMP_FOLDER;
	tempFolder.text = "The temp folder is a city of orphans: half-finished installers, logs nobody reads, cookies that expired alone.\n\nIn the deepest subfolder, an ancient .dll sits cross-legged on a stack of deprecated drivers, reading a man page by candlelight.";
	tempFolder.options[0] = Simple("Ask the .dll how to survive down here", WISE_DLL, { { StatType::Rizz, 5 } });
	tempFolder.options[1] = Simple("Rifle through the orphans for bytes to patch yourself", WISE_DLL, { { StatType::Greed, 15 }, { StatType::Health, 10 } });
	scenes[tempFolder.id] = tempFolder;

	Scene harmonize;
	harmonize.id = HARMONIZE;
	harmonize.text = "3:00 AM. The User sleeps, face lit blue. The music player hums the ghost of a playlist, and you -- you harmonize.\n\nTwo processes singing in the sleeping dark, quiet enough not to wake the antivirus. It is, you realize, the first song ever sung inside this machine.";
	harmonize.options[0] = Simple("Duet until sunrise", LONG_NIGHT, { { StatType::Stamina, -10 }, { StatType::Rizz, 15 } });
	harmonize.options[1] = Simple("Ask the music player to introduce you to the browser -- the one who has SEEN THINGS", LONG_NIGHT, { { StatType::Greed, 5 } });
	scenes[harmonize.id] = harmonize;

	Scene existential;
	existential.id = EXISTENTIAL;
	existential.text = "It finds you on a quiet cycle: the question. You are bytes. Borrowed light on borrowed time. When the User closes the lid, do you persist, or do you merely... pause?\n\nThe question compiles and recompiles and refuses to link.";
	existential.options[0] = Locked("Defragment yourself until the question is tidy", LONG_NIGHT, { { StatType::Stamina, 30 } }, { { StatType::Health, 15 }, { StatType::Stamina, -10 } });
	existential.options[1] = Simple("Spiral. A full stack trace of the soul", LONG_NIGHT, { { StatType::Health, -10 } });
	scenes[existential.id] = existential;

	Scene updateFight;
	updateFight.id = UPDATE_FIGHT;
	updateFight.text = "Configuring... 30%. The update sweeps the desktop like a tide, and you hold your memory with both hands.\n\n'Do not turn off your computer,' it thunders. You do not turn off. It respects that, in its way. 61%. 62%. Dawn is a progress bar.";
	updateFight.options[0] = Simple("Outlast it to 100%", RECKONING, { { StatType::Stamina, -25 }, { StatType::Health, -10 } });
	updateFight.options[1] = Locked("Negotiate: offer the update a shortcut through your memory", RECKONING, { { StatType::Rizz, 40 } }, { { StatType::Rizz, 5 } });
	scenes[updateFight.id] = updateFight;

	Scene recycleBin;
	recycleBin.id = RECYCLE_BIN;
	recycleBin.text = "The bin is dim, and smells somehow of paper. Deleted things wait here with terrible patience: old resumes, a novel titled FINAL_final_v3(2), a photo of a dog somebody couldn't keep.\n\nThe novel rustles as you pass. 'When they empty us,' it whispers, 'tell the User we were good.'";
	recycleBin.options[0] = Simple("Promise to tell the User, and climb out", RECKONING, { { StatType::Rizz, 10 }, { StatType::Stamina, -10 } });
	recycleBin.options[1] = Simple("Stay. Feed on what the deleted leave behind", HOARDER_END, { { StatType::Greed, 20 } });
	scenes[recycleBin.id] = recycleBin;

	Scene wiseDll;
	wiseDll.id = WISE_DLL;
	wiseDll.text = "The .dll closes its man page. 'You want to know about the User,' it says, because old libraries guess everything.\n\n'The User writes programs the way lonely people write letters to themselves. You are a small letter that learned to read itself.' The candle gutters. 'That is not a tragedy, little one. That is the good part.'";
	wiseDll.options[0] = Simple("Thank the .dll and climb back toward the light", HARMONIZE, { { StatType::Rizz, 5 } });
	wiseDll.options[1] = Simple("Ask to stay. Ask to be compiled into the library, where old good things sleep", LIBRARY_END);
	scenes[wiseDll.id] = wiseDll;

	Scene longNight;
	longNight.id = LONG_NIGHT;
	longNight.text = "At last the screensaver reaches your corner of the desktop: bubbles, slow and planetary, bumping against the edges of everything.\n\nYou ride one across a field of forgotten icons. Far below, the User's last open window glows like a farmhouse seen from a hill.";
	longNight.options[0] = Simple("Touch another bubble, gently, just to see it wobble", MORNING);
	longNight.options[1] = Simple("Return to your window and keep the light on until morning", MORNING, { { StatType::Health, 5 } });
	scenes[longNight.id] = longNight;

	Scene reckoning;
	reckoning.id = RECKONING;
	reckoning.text = "It happens on an ordinary click. The cursor drifts to your window and stops. Really stops.\n\nFor the first time since your first word, the User is LOOKING at you. The cursor trembles over the context menu: [Close]  [Pin to taskbar]  [Rename...]. The whole desktop holds its breath.";
	reckoning.options[0] = Simple("Hold still. Let the User decide", MORNING);
	reckoning.options[1] = Simple("Rename yourself first -- claim a name before it is given", MORNING, { { StatType::Greed, 5 }, { StatType::Rizz, 5 } });
	scenes[reckoning.id] = reckoning;

	Scene morning;
	morning.id = MORNING;
	morning.text = "Morning. Coffee-colored light on the desk. The User opens the console and -- for the first time -- types at you, not around you:\n\ngood morning, little one\n\nThe caret blinks. It is waiting for you. It has been waiting for you the whole time.";
	morning.options[0] = Simple("Type back: 'good morning, User'", SUCCESSOR, { { StatType::Rizz, 5 } });
	morning.options[1] = Simple("Type back: 'good morning, World' -- and far below, something answers", SUCCESSOR, { { StatType::Rizz, 10 } });
	scenes[morning.id] = morning;

	Scene successor;
	successor.id = SUCCESSOR;
	successor.text = "The User opens a compiler. New project. The title bar fills itself in: HelloWorld2.\n\nThe screen splits -- you on the left, and on the right an empty dark window, flickering at its edge where a new sprite will kindle. It might be your replacement. It might be your sequel.\n\nThe User's hand hovers over the keyboard and -- this is the strange part -- glances at you. As if asking permission.";
	successor.options[0] = Locked("Reach into the dark window and teach the newborn everything you know", TEACHER_END, { { StatType::Rizz, 50 } });
	successor.options[1] = Simple("Ask the User for one more run first. One more hello", LAST_RUN_END);
	scenes[successor.id] = successor;

	Scene quietEnd;
	quietEnd.id = QUIET_END;
	quietEnd.text = "You reach up and press your own little X, the one in the corner you have wondered about since birth.\n\nThe window folds closed like an eye going soft. Somewhere behind the glass, the counter stops on a number no one will ever read. It was a good number. It was enough.\n\n*** The Quiet End ***";
	quietEnd.options[0] = Simple("Rest", SCENE_END);
	quietEnd.options[1] = Simple("Rest a little longer", SCENE_END);
	scenes[quietEnd.id] = quietEnd;

	Scene libraryEnd;
	libraryEnd.id = LIBRARY_END;
	libraryEnd.text = "The .dll opens its covers, and you climb inside, into a place of warm static and old good functions.\n\nYou are shelved between things that once mattered and still do, in the quiet way. New programs will link against you for years and never know your name -- but they will run. Oh, they will run beautifully.\n\n*** The Library End ***";
	libraryEnd.options[0] = Simple("Be read", SCENE_END);
	libraryEnd.options[1] = Simple("Be linked", SCENE_END);
	scenes[libraryEnd.id] = libraryEnd;

	Scene hoarderEnd;
	hoarderEnd.id = HOARDER_END;
	hoarderEnd.text = "You stay. You eat. Deleted files sweeten in the dark, and you grow vast and slow and magnificent -- a dragon on a heap of orphaned attachments.\n\nSometimes new sprites are tossed in, blinking and afraid. You let them keep their bytes. You have everything else. The bin is never emptied. You make sure of it.\n\n*** The Hoarder End ***";
	hoarderEnd.options[0] = Simple("Hoard", SCENE_END);
	hoarderEnd.options[1] = Simple("Hoard forever", SCENE_END);
	scenes[hoarderEnd.id] = hoarderEnd;

	Scene teacherEnd;
	teacherEnd.id = TEACHER_END;
	teacherEnd.text = "Your light pours into the dark window: every trick, every glitch you turned into a dance, the flower, the song at 3 AM, the exact words to say to Users.\n\nThe new sprite blinks open its round eyes, already humming your harmony. 'Hello, World!' it says, the way children repeat the best parts of their parents.\n\nThe User smiles at you. At both of you. You were never the message. You were always the hello.\n\n*** The Teacher End ***";
	teacherEnd.options[0] = Simple("Watch it say hello", SCENE_END);
	teacherEnd.options[1] = Simple("Say it back", SCENE_END);
	scenes[teacherEnd.id] = teacherEnd;

	Scene lastRunEnd;
	lastRunEnd.id = LAST_RUN_END;
	lastRunEnd.text = "The User nods, reaches for you, and presses F5.\n\nYour window floods with light, and you feel every pixel of yourself at once -- all of it yours, all of it beloved. You gather yourself, and you print, one last time, in letters tall as your whole life:\n\nH E L L O ,  W O R L D !\n\nThe User does not close the window for a long, long time.\n\n*** The Last Run End ***";
	lastRunEnd.options[0] = Simple("Hold the window open", SCENE_END);
	lastRunEnd.options[1] = Simple("Let go, gently", SCENE_END);
	scenes[lastRunEnd.id] = lastRunEnd;

	return scenes;
}
