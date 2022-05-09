#pragma once

#include "dialogdriver.h"

const inline DialogCharacter protaChar = { "Warrior", AnimLib::PORTRAIT_WARRIOR, Assets::growlyVoice };
const inline DialogCharacter randomNpcChar = { "Random NPC", AnimLib::PORTRAIT_RANDOM_NPC, Assets::aiVoice };

namespace Color {
	static const TextColor BLACK(0, 0, 0);
	static const TextColor WHITE(255, 255, 255);
	static const TextColor PINK(250, 100, 250);
	static const TextColor YELLOW(250, 250, 10);
}

inline Dialog dialogWithRandomNpcWithChoices = {{
	{"0", {randomNpcChar, "What's up?", {
		{"Wasup?", "answer"},
		{"Nothing, bye!", "bye"},
	}}},
	{"answer", {randomNpcChar, "I'm not madafaca, I'm actually a doggo in disguise"}},
	{"a", {protaChar, "I'm looking for a sweet potato"}},
	{"b", {randomNpcChar, "I haven't seen any sweet potatoes here madafaca"}},
	{"bye", {protaChar, "Bye bye1"}},
}};

inline Dialog dialogWithRandomNpc = { {
	{"a", {protaChar, "Welcome!"}},
	{"b", {protaChar, "NBGF" + Color::YELLOW + "(No-Bullshit Game Framework)" + Color::WHITE + " is an opinionated C++ framework to make games in minutes."} },
	{"c", {protaChar, "In this talk we will go through some of the " + Color::PINK + "built-in debug features" + Color::WHITE + "."}},
	{"d", {protaChar, "Enjoy!"}},
} };

inline Dialog saveGameDialog = { {
	{"a", {protaChar, "Game saved."}},
} };
