#pragma once

#include "dialogdriver.h"

const inline DialogCharacter protaChar = { "Warrior", AnimLib::PORTRAIT_WARRIOR, Assets::growlyVoice };
const inline DialogCharacter randomNpcChar = { "Random NPC", AnimLib::PORTRAIT_RANDOM_NPC, Assets::aiVoice };

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
	{"a", {protaChar, "I'm looking for a sweet potato. Are you a sweet potato in disguise?"}},
	{"b", {randomNpcChar, "I'm not madafaca. And I haven't seen any sweet potatoes here."}},
	{"c", {protaChar, "Bye bye1"}},
} };

inline Dialog saveGameDialog = { {
	{"a", {protaChar, "Game saved."}},
} };
