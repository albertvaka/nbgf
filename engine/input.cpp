#include "input.h"

#include "magic_enum.h"
#include <functional>

std::function<bool()> kb_map[magic_enum::enum_count<GameKeys>()];
std::function<bool(int)> gp_map[magic_enum::enum_count<GameKeys>()];

inline void RemapGamePadInput()
{
	gp_map[GameKeys::UP] = [](int p) {
		return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_X) ||
			GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_B) ||
			GamePad::AnalogStick::Left.get(p, 30.f).y < -50.0f ||
			GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_UP);
	};
	gp_map[GameKeys::DOWN] = [](int p) {
		return GamePad::AnalogStick::Left.get(p, 30.f).y > 50.0f ||
			GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	};
	gp_map[GameKeys::LEFT] = [](int p) {
		return GamePad::AnalogStick::Left.get(p, 20.f).x < 0.0f ||
			GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	};
	gp_map[GameKeys::RIGHT] = [](int p) {
		return GamePad::AnalogStick::Left.get(p, 20.f).x > 0.0f ||
			GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	};
	gp_map[GameKeys::ACTION] = [](int p) { return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_A); };
	gp_map[GameKeys::START] = [](int p) { return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_START); };
}

inline void RemapKeyboardInput()
{
	kb_map[GameKeys::UP] = []() {
		return Keyboard::IsKeyPressed(SDL_SCANCODE_W) || Keyboard::IsKeyPressed(SDL_SCANCODE_UP);
	};
	kb_map[GameKeys::DOWN] = []() {
		return Keyboard::IsKeyPressed(SDL_SCANCODE_S) || Keyboard::IsKeyPressed(SDL_SCANCODE_DOWN);
	};
	kb_map[GameKeys::LEFT] = []() {
		return Keyboard::IsKeyPressed(SDL_SCANCODE_A) || Keyboard::IsKeyPressed(SDL_SCANCODE_LEFT);
	};
	kb_map[GameKeys::RIGHT] = []() {
		return Keyboard::IsKeyPressed(SDL_SCANCODE_D) || Keyboard::IsKeyPressed(SDL_SCANCODE_RIGHT);
	};
	kb_map[GameKeys::ACTION] = []() {
		return Keyboard::IsKeyPressed(SDL_SCANCODE_E) || Keyboard::IsKeyPressed(SDL_SCANCODE_SPACE);
	};
	kb_map[GameKeys::START] = []() {
		return Keyboard::IsKeyPressed(SDL_SCANCODE_RETURN) || Keyboard::IsKeyPressed(SDL_SCANCODE_ESCAPE);
	};
}



//int player_to_joystick[Input::kMaxPlayers] = { nullptr };

// Assumes first player is on Keyboard.
int Input::keyboard_player_id = 0;
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
	RemapGamePadInput();
	RemapKeyboardInput();
}

