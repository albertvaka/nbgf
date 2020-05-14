#pragma once

#include "raw_input.h"

enum GameKeys
{
	NONE = 0,
	UP, DOWN, LEFT, RIGHT,
	START,
	ACTION,
};

// Action-based input
struct Input {

	static const int kMaxPlayers = 4;

	static bool IsPressed(int player, GameKeys k) {
		return (action_states[player][k] == PRESSED || action_states[player][k] == JUST_PRESSED);
	}

	static bool IsPressedAnyPlayer(GameKeys k) {
		for (int i = 0; i < kMaxPlayers; i++) {
			if (IsPressed(i, k)) return true;
		}
		return false;
	}

	static bool IsJustPressed(int player, GameKeys k) {
		return (action_states[player][k] == JUST_PRESSED);
	}

	static bool IsJustPressedAnyPlayer(GameKeys k) {
		for (int i = 0; i < kMaxPlayers; i++) {
			if (IsJustPressed(i, k)) return true;
		}
		return false;
	}

	static bool IsJustPressed(int player, GameKeys k, float interval) {
		return action_states[player][k] == JUST_PRESSED || (action_states[player][k] == PRESSED && action_times[player][k] < interval);
	}

	static bool IsReleased(int player, GameKeys k) {
		return (action_states[player][k] == RELEASED || action_states[player][k] == JUST_RELEASED);
	}

	static bool IsJustReleased(int player, GameKeys k) {
		return (action_states[player][k] == JUST_RELEASED);
	}

	static bool IsJustReleased(int player, GameKeys k, float interval) {
		return action_states[player][k] == JUST_RELEASED || (action_states[player][k] == RELEASED && action_times[player][k] < interval);
	}

	static void ConsumeJustPressed(int player, GameKeys k) {
		action_states[player][k] = PRESSED;
		action_times[player][k] += 1000.f;
	}

	static void ConsumeJustReleased(int player, GameKeys k) {
		action_states[player][k] = RELEASED;
		action_times[player][k] += 1000.f;
	}

	static void Init();
	static void Update(float dt);

private:
	static int keyboard_player_id; //max 1 player on keyboard

	static KeyStates action_states[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()];
	static float action_times[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()];
};

