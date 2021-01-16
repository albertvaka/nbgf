#pragma once

#include <functional>

#include "raw_input.h"
#include "../src/input_conf.h"

// Action-based input
struct Input {

	static const int kMaxPlayers = 4;

	static bool IsPressed(int player, GameKeys k) {
		return (action_states[player][int(k)] == PRESSED || action_states[player][int(k)] == JUST_PRESSED);
	}

	static bool IsPressedAnyPlayer(GameKeys k) {
		for (int i = 0; i < kMaxPlayers; i++) {
			if (IsPressed(i, k)) return true;
		}
		return false;
	}

	static bool IsJustPressed(int player, GameKeys k) {
		return (action_states[player][int(k)] == JUST_PRESSED);
	}

	static bool IsJustPressedAnyPlayer(GameKeys k) {
		for (int i = 0; i < kMaxPlayers; i++) {
			if (IsJustPressed(i, k)) return true;
		}
		return false;
	}

	static bool IsJustPressed(int player, GameKeys k, float interval) {
		return action_states[player][int(k)] == JUST_PRESSED || (action_states[player][int(k)] == PRESSED && action_times[player][int(k)] < interval);
	}

	static bool IsReleased(int player, GameKeys k) {
		return (action_states[player][int(k)] == RELEASED || action_states[player][int(k)] == JUST_RELEASED);
	}

	static bool IsJustReleased(int player, GameKeys k) {
		return (action_states[player][int(k)] == JUST_RELEASED);
	}

	static bool IsJustReleased(int player, GameKeys k, float interval) {
		return action_states[player][int(k)] == JUST_RELEASED || (action_states[player][int(k)] == RELEASED && action_times[player][int(k)] < interval);
	}

	static void ConsumeJustPressed(int player, GameKeys k) {
		action_states[player][int(k)] = PRESSED;
		action_times[player][int(k)] += 1000.f;
	}

	static void ConsumeJustReleased(int player, GameKeys k) {
		action_states[player][int(k)] = RELEASED;
		action_times[player][int(k)] += 1000.f;
	}

	static void Init();
	static void Update(float dt);

private:
	static int keyboard_player_id; //max 1 player on keyboard
	static void MapGameKeysToGamePad();
	static void MapGameKeysToKeyboard();
	static std::function<bool()> kb_map[magic_enum::enum_count<GameKeys>()];
	static std::function<bool(int)> gp_map[magic_enum::enum_count<GameKeys>()];

	static KeyStates action_states[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()];
	static float action_times[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()];
};

