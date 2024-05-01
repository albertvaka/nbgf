#pragma once

#include <functional>

#include <raylib.h>

#include "magic_enum.h"
#include "vec.h"

#include "../src/input_conf.h"

enum KeyStates { JUST_RELEASED, RELEASED, JUST_PRESSED, PRESSED };

vec GetMousePositionInWorld();

// Action-based input
struct Input {

	static const int kMaxPlayers;

	static vec GetAnalog(int player, AnalogInput k) {
		//SDL_assert(player < kMaxPlayers);
		return analog_states[player][int(k)];
	}

	static bool IsPressed(int player, GameKeys k) {
		//SDL_assert(player < kMaxPlayers);
		return (action_states[player][int(k)] == PRESSED || action_states[player][int(k)] == JUST_PRESSED);
	}

	static bool IsPressedAnyPlayer(GameKeys k) {
		for (int i = 0; i < kMaxPlayers; i++) {
			if (IsPressed(i, k)) return true;
		}
		return false;
	}

	static bool IsJustPressedAnyPlayer(GameKeys k) {
		for (int i = 0; i < kMaxPlayers; i++) {
			if (IsJustPressed(i, k)) return true;
		}
		return false;
	}

	static bool IsReleased(int player, GameKeys k) {
		//SDL_assert(player < kMaxPlayers);
		return (action_states[player][int(k)] == RELEASED || action_states[player][int(k)] == JUST_RELEASED);
	}

	// True for one frame after the action is pressed
	static bool IsJustPressed(int player, GameKeys k) {
		//SDL_assert(player < kMaxPlayers);
		return (action_states[player][int(k)] == JUST_PRESSED);
	}

	// True for `interval` time after the key is pressed
	static bool IsJustPressed(int player, GameKeys k, float interval) {
		//SDL_assert(player < kMaxPlayers);
		return action_states[player][int(k)] == JUST_PRESSED || (action_states[player][int(k)] == PRESSED && action_times[player][int(k)] < interval);
	}

	// True for one frame after the action is pressed
	static bool IsJustReleased(int player, GameKeys k) {
		//SDL_assert(player < kMaxPlayers);
		return (action_states[player][int(k)] == JUST_RELEASED);
	}

	// True for `interval` time after the key is pressed
	static bool IsJustReleased(int player, GameKeys k, float interval) {
		//SDL_assert(player < kMaxPlayers);
		return action_states[player][int(k)] == JUST_RELEASED || (action_states[player][int(k)] == RELEASED && action_times[player][int(k)] < interval);
	}

	// Consume a just pressed event so the next call to IsJustPressed for that action returns false
	static void ConsumeJustPressed(int player, GameKeys k) {
		//SDL_assert(player < kMaxPlayers);
		action_states[player][int(k)] = PRESSED;
		action_times[player][int(k)] += 1000.f;
	}

	// Consume a just released event so the next call to IsJustReleased for that action returns false
	static void ConsumeJustReleased(int player, GameKeys k) {
		//SDL_assert(player < kMaxPlayers);
		action_states[player][int(k)] = RELEASED;
		action_times[player][int(k)] += 1000.f;
	}

	// Called from main.cpp
	static void Update(float dt);
	static void Init();

	static void IgnoreInput(bool enable);

private:
	static void MapGameKeys();
	static std::function<bool(int)> action_mapping[magic_enum::enum_count<GameKeys>()];
	static std::function<vec(int)> analog_mapping[magic_enum::enum_count<AnalogInput>()];
	static KeyStates action_states[][magic_enum::enum_count<GameKeys>()];
	static float action_times[][magic_enum::enum_count<GameKeys>()];
	static vec analog_states[][magic_enum::enum_count<AnalogInput>()];
};

