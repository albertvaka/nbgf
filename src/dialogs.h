#pragma once

#include "dialogdriver.h"

const inline DialogCharacter protaChar = { "Warrior", AnimLib::PORTRAIT_WARRIOR, Assets::growlyVoice };
const inline DialogCharacter randomNpcChar = { "Random NPC", AnimLib::PORTRAIT_RANDOM_NPC, Assets::aiVoice };

namespace Color {
	static constexpr TextColor BLACK(0, 0, 0);
	static constexpr TextColor WHITE(255, 255, 255);
	static constexpr TextColor PINK(250, 10, 250);
	static constexpr TextColor YELLOW(250, 250, 10);
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
	{"a", {protaChar, "I'm looking for a " + Color::PINK + "sweet potato" + Color::WHITE + ". Are you a " + Color::PINK + "sweet potato" + Color::WHITE + " in disguise? "}},
	{"b", {randomNpcChar, "I'm not " + Color::YELLOW + "madafaca"+ Color::WHITE + ". And I haven't seen any " + Color::PINK + "sweet potatoes" + Color::WHITE + " here."}},
	{"c", {protaChar, "Bye bye1"}},
} };

inline Dialog saveGameDialog = { {
	{"a", {protaChar, "Game saved."}},
} };
