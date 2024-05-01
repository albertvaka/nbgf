#include "input.h"

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


/*
void Mouse::_UpdateInputState()
{
	oldPos = pos;
	Vector2 v = GetMousePosition();
	pos.x = v.x;
	pos.y = v.y;

	// Convert mouse position to scaled, letterboxed game coordinates
	// --------------------------------------------------------------

#if defined(__EMSCRIPTEN__) || defined(__APPLE__)
	// In emscripten, mouse position is not in the same coordinates as the viewport, convert them first
	// Same happens in MacOS when on a retina display with auto-scaling

	// Normalize mouse coordinates
	pos.x /= Window::screenTarget->context->window_w;
	pos.y /= Window::screenTarget->context->window_h;

	// Expand to viewport coordinates
	pos.x *= Window::screenTarget->context->drawable_w; // drawable_w equals to the size of the viewport + margins, ie: 2*Window::screenTarget->viewport.x + Window::screenTarget->viewport.w;
	pos.y *= Window::screenTarget->context->drawable_h;
#endif

	// Remove margins
	pos.x -= Window::screenTarget->viewport.x;
	pos.y -= Window::screenTarget->viewport.y;

	// Normalize to viewport coordinates without margin
	pos.x /= Window::screenTarget->viewport.w;
	pos.y /= Window::screenTarget->viewport.h;

	// Expand to game coordinates
	pos.x *= Window::GAME_WIDTH;
	pos.y *= Window::GAME_HEIGHT;
}
*/


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
}


void Input::Init()
{
	Input::MapGameKeys();
}

