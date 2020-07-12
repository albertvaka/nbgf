#pragma once

#include "magic_enum.h"

#include "window.h"

#include <SDL.h>

// Static stuff here:
//
// - Keyboard
// - Mouse
// - GamePad
//

enum KeyStates { JUST_RELEASED, RELEASED, JUST_PRESSED, PRESSED };

struct GamePad
{

private:
	GamePad() { }

	static const int max_gamepads = 4;

	static KeyStates button_states[max_gamepads][SDL_CONTROLLER_BUTTON_MAX];
	static SDL_GameController* joysticks[max_gamepads];

	static KeyStates calculateJustPressed(bool pressed, KeyStates state);

public:

	static inline int ConnectedGamepads() {
		int a = 0;
		for (SDL_GameController* c : joysticks) {
			if (c != nullptr) {
				a++;
			}
		}
		return a;
	}

	struct Trigger
	{
		struct TriggerBase
		{
			friend struct GamePad;
			bool IsPressed(int player) { return (state[player] == PRESSED || state[player] == JUST_PRESSED); }
			bool IsJustPressed(int player) { return (state[player] == JUST_PRESSED); }
			bool IsReleased(int player) { return (state[player] == RELEASED || state[player] == JUST_RELEASED); }
			bool IsJustReleased(int player) { return (state[player] == JUST_RELEASED); }
		private:
			KeyStates state[max_gamepads];
		};
		struct LeftTrigger : public TriggerBase
		{
			float get(int player) const
			{ //Pos between 0 and 100
				//if (player > Input::kMaxPlayers) return vec();
				SDL_GameController* joystick = joysticks[player];
				if (!joystick) return 0;
				float a = SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 327.67f;
				return a > 0.1 ? a : 0;
			}
		};
		struct RightTrigger : public TriggerBase
		{
			float get(int player) const
			{ //Pos between 0 and 100
				//if (player > Input::kMaxPlayers) return vec();
				SDL_GameController* joystick = joysticks[player];
				if (!joystick) return 0;
				float a = SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 327.67f;
				return a > 0.1 ? a : 0;
			}
		};
		static LeftTrigger Left;
		static RightTrigger Right;
	};

	struct AnalogStick
	{
		const static AnalogStick Left;
		const static AnalogStick Right;
		vec get(int player, float dead_area = 0) const
		{ //Pos between -100 and 100
			//if (player > Input::kMaxPlayers) return vec();
			SDL_GameController* joystick = joysticks[player];
			if (!joystick) return vec();
			float a = SDL_GameControllerGetAxis(joystick, x) / 327.67f;
			float b = SDL_GameControllerGetAxis(joystick, y) / 327.67f;
			return vec(abs(a) > dead_area ? a : 0, abs(b) > dead_area ? b : 0);
		}
	private:
		AnalogStick(SDL_GameControllerAxis mx, SDL_GameControllerAxis my) : x(mx), y(my) { }
		SDL_GameControllerAxis x, y;
	};

	static bool IsButtonPressed(int player, SDL_GameControllerButton b) { return (button_states[player][b] == PRESSED || button_states[player][b] == JUST_PRESSED); }
	static bool IsButtonJustPressed(int player, SDL_GameControllerButton b) { return (button_states[player][b] == JUST_PRESSED); }
	static bool IsButtonReleased(int player, SDL_GameControllerButton b) { return (button_states[player][b] == RELEASED || button_states[player][b] == JUST_RELEASED); }
	static bool IsButtonJustReleased(int player, SDL_GameControllerButton b) { return (button_states[player][b] == JUST_RELEASED); }

	static void _UpdateInputState__XboxNormal(SDL_GameController* joy, int player);
	static void _UpdateInputState();
	static void _Added(SDL_GameController* joystick);
	static void _Removed(SDL_GameController* joystick);
};

//KEYBOARD ACCESS
//===============

struct Keyboard
{
	static bool IsKeyPressed(SDL_Scancode k) {
		return state[k];
	}
	static bool IsKeyJustPressed(SDL_Scancode k) {
		return state[k] && !prev_state[k];
	}
	static bool IsKeyReleased(SDL_Scancode k) {
		return !state[k];
	}
	static bool IsKeyJustReleased(SDL_Scancode k) {
		return !state[k] && prev_state[k];
	}
	static Uint8* state;
	static Uint8* prev_state;
	static Uint8 state_buffers[2][SDL_NUM_SCANCODES];
	static void _UpdateInputState();
};



//MOUSE ACCESS
//============
struct Mouse
{
	static float scrollWheel;
	enum Button {
		None = 0,
		Left = SDL_BUTTON_LEFT, //1
		Middle = SDL_BUTTON_MIDDLE, //2
		Right = SDL_BUTTON_RIGHT, //3
	};

	static bool IsPressed(Button b = Button::Left)
	{
		if (!Window::HasFocus()) return false;
		return (button_states[b] == PRESSED || button_states[b] == JUST_PRESSED);
	}

	static bool IsJustPressed(Button b = Button::Left)
	{
		if (!Window::HasFocus()) return false;
		return (button_states[b] == JUST_PRESSED);
	}

	static bool IsReleased(Button b = Button::Left)
	{
		if (!Window::HasFocus()) return false;
		return (button_states[b] == RELEASED || button_states[b] == JUST_RELEASED);
	}

	static bool IsJustReleased(Button b = Left)
	{
		if (!Window::HasFocus()) return false;
		return (button_states[b] == JUST_RELEASED);
	}

	static float GetScrollWheelMovement() { return scrollWheel; }

	static vec GetPositionInWindow() { return pos; }
	static vec GetDeltaMovement() { return oldPos-pos; }

	static vec GetPositionInWorld();

	static void _UpdateInputState();

private:
	static KeyStates button_states[magic_enum::enum_count<Button>()];
	static vec pos, oldPos;

};
