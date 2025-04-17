#pragma once

#include "magic_enum.h"

#include "vec.h"

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
	static const int MAX_GAMEPADS = 4;

	static bool IsButtonPressed(int player, SDL_GameControllerButton b) { return (button_states[player][b] == PRESSED || button_states[player][b] == JUST_PRESSED); }
	static bool IsButtonJustPressed(int player, SDL_GameControllerButton b) { return (button_states[player][b] == JUST_PRESSED); }
	static bool IsButtonReleased(int player, SDL_GameControllerButton b) { return (button_states[player][b] == RELEASED || button_states[player][b] == JUST_RELEASED); }
	static bool IsButtonJustReleased(int player, SDL_GameControllerButton b) { return (button_states[player][b] == JUST_RELEASED); }

	static int ConnectedGamepads();

	struct Trigger
	{
		static Trigger Left;
		static Trigger Right;
		float get(int player) const
		{ //Pos between 0 and 100
			SDL_GameController* joystick = joysticks[player];
			if (!joystick) return 0;
			float a = SDL_GameControllerGetAxis(joystick, axis) / 327.67f;
			return a > 0.1f ? a : 0;
		}
		bool IsPressed(int player) const { return (state[player] == PRESSED || state[player] == JUST_PRESSED); }
		bool IsJustPressed(int player) const { return (state[player] == JUST_PRESSED); }
		bool IsReleased(int player) const { return (state[player] == RELEASED || state[player] == JUST_RELEASED); }
		bool IsJustReleased(int player) const { return (state[player] == JUST_RELEASED); }
	private:
		Trigger(SDL_GameControllerAxis axis) : axis(axis) { }
		SDL_GameControllerAxis axis;
		KeyStates state[MAX_GAMEPADS];
		friend struct GamePad;
	};

	struct AnalogStick
	{
		const static AnalogStick Left;
		const static AnalogStick Right;
		vec get(int player, float dead_area = 30.f) const
		{ //Pos between -100 and 100
			SDL_GameController* joystick = joysticks[player];
			if (!joystick) return vec();
			float x = SDL_GameControllerGetAxis(joystick, axisX) / 327.67f;
			float y = SDL_GameControllerGetAxis(joystick, axisY) / 327.67f;
			vec pos(x, y);
			return pos.Length() < dead_area ? vec::Zero : pos;
		}
	private:
		AnalogStick(SDL_GameControllerAxis mx, SDL_GameControllerAxis my) : axisX(mx), axisY(my) { }
		SDL_GameControllerAxis axisX, axisY;
	};

	static void _UpdateInputState__Xbox(SDL_GameController* joy, int player);
	static void _UpdateInputState();
	static void _Added(SDL_GameController* joystick);
	static void _Removed(SDL_GameController* joystick);

private:
	GamePad() { }

	static KeyStates button_states[MAX_GAMEPADS][SDL_CONTROLLER_BUTTON_MAX];
	static SDL_GameController* joysticks[MAX_GAMEPADS];

	static KeyStates calculateJustPressed(bool pressed, KeyStates state);

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
		//if (!Window::HasFocus()) return false;
		return (button_states[b] == PRESSED || button_states[b] == JUST_PRESSED);
	}

	static bool IsJustPressed(Button b = Button::Left)
	{
		//if (!Window::HasFocus()) return false;
		return (button_states[b] == JUST_PRESSED);
	}

	static bool IsReleased(Button b = Button::Left)
	{
		//if (!Window::HasFocus()) return false;
		return (button_states[b] == RELEASED || button_states[b] == JUST_RELEASED);
	}

	static bool IsJustReleased(Button b = Button::Left)
	{
		//if (!Window::HasFocus()) return false;
		return (button_states[b] == JUST_RELEASED);
	}

	static float GetScrollWheelMovement() { return scrollWheel; }

	static vec GetPositionInWindow() { return pos; }
	static vec GetDeltaMovement() { return pos - oldPos; }

	static vec GetPositionInWorld();

	static void _UpdateInputState();

private:
	static KeyStates button_states[magic_enum::enum_count<Button>()];
	static vec pos, oldPos;

};
