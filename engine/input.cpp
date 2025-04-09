#include "input.h"

#include "magic_enum.h"
#include <functional>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

//int player_to_joystick[Input::kMaxPlayers] = { nullptr };
bool ignoreInput = false;

void Input::IgnoreInput(bool enable) {
	ignoreInput = enable;
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


#ifdef __EMSCRIPTEN__
EM_JS(bool, JavaScriptIsTouchScreenPrimaryInput, (), {
    // Check if the device has touch capability
    if ('ontouchstart' in window || navigator.maxTouchPoints > 0) {
        // Check if the device is likely a mobile device
        const isMobile = /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
        // Check if the device has a keyboard
        const hasKeyboard = navigator.keyboard && navigator.keyboard.getLayoutMap;
        return isMobile && !hasKeyboard;
    }
    return false;
});
#endif


Input::InputDevice GetInitialInputDevice() {
#ifdef __EMSCRIPTEN__
	if (JavaScriptIsTouchScreenPrimaryInput()) {
		return Input::InputDevice::Touch;
	}
#endif
	if (SDL_GameControllerGetAttached(0)) {
		return Input::InputDevice::GamePad;
	}
	return Input::InputDevice::Keyboard;
}

Input::InputDevice Input::PreferredUserInputDevice = GetInitialInputDevice();
