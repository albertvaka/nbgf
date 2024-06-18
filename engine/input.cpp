#include "input.h"

#include <raylib.h>

#include "magic_enum.h"
#include <functional>

#include "camera.h"
#include "window_conf.h"
#include "debug.h"

//int player_to_joystick[Input::kMaxPlayers] = { nullptr };
bool ignoreInput = false;

void Input::IgnoreInput(bool enable) {
	ignoreInput = enable;
}

vec GetMousePositionInWorld()
{
	return GameCamera::ScreenToWorld(GetMousePosition());
}

void Input::Update(float dt)
{
	for (int player = 0; player < Input::kMaxPlayers; ++player) {
		int gamepad_id = player;
		for (size_t k = 1; k < magic_enum::enum_count<GameKeys>(); k++) {  //Skip GameKeys::NONE
			bool isPressed = (player == 0 && ignoreInput)? false : (action_mapping[k] && action_mapping[k](gamepad_id));
			if (isPressed) {
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
		for (size_t k = 1; k < magic_enum::enum_count<AnalogInput>(); k++) {  //Skip AnalogInput::NONE
			if (analog_mapping[k]) {
				analog_states[player][k] = analog_mapping[k](gamepad_id);
			}
		}
	}

	// Convert mouse position to scaled, letterboxed game coordinates
	// From https://www.raylib.com/examples/core/loader.html?name=core_window_letterbox
	float scale = Window::GetScaleFactor();
	SetMouseOffset(-(GetScreenWidth() - (Window::GAME_WIDTH*scale))*0.5f, -(GetScreenHeight() - (Window::GAME_HEIGHT*scale))*0.5f);
	SetMouseScale(1/scale, 1/scale);
}

void Input::Init()
{
	Input::MapGameKeys();
}

