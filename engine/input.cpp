#include "input.h"

#include "magic_enum.h"
#include <functional>

// defined in src/input_config.cpp
extern std::function<bool()> kb_map[magic_enum::enum_count<GameKeys>()];
extern std::function<bool(int)> gp_map[magic_enum::enum_count<GameKeys>()];

//int player_to_joystick[Input::kMaxPlayers] = { nullptr };

KeyStates Input::action_states[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()] = { { RELEASED } };
float Input::action_times[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()] = { { 0 } };

void Input::Update(float dt)
{
	for (int player = 0; player < Input::kMaxPlayers; ++player) {
		int gamepad_id = player;
		for (size_t k = 1; k < magic_enum::enum_count<GameKeys>(); k++) {  //Skip GameKeys::NONE
			bool pressed_now = (gp_map[k] && gp_map[k](gamepad_id));
			if (player == keyboard_player_id) {
				pressed_now = pressed_now || (kb_map[k] && kb_map[k]());
			}
			if (pressed_now) {
				if (action_states[player][k] == JUST_PRESSED || action_states[player][k] == PRESSED) {
					action_states[player][k] = PRESSED;
					if (action_times[player][k] < 1000.f) action_times[player][k] += dt;
				}
				else {
					action_states[player][k] = JUST_PRESSED;
					action_times[player][k] = dt;
				}
			}
			else {
				if (action_states[player][k] == JUST_RELEASED || action_states[player][k] == RELEASED) {
					action_states[player][k] = RELEASED;
					if (action_times[player][k] < 1000.f) action_times[player][k] += dt;
				}
				else {
					action_states[player][k] = JUST_RELEASED;
					action_times[player][k] = dt;
				}
			}
		}
	}
}


void Input::Init()
{
	Input::MapGameKeysToGamePad();
	Input::MapGameKeysToKeyboard();
}

