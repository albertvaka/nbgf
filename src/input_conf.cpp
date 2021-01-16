#include "input_conf.h"

#include "input.h"

#include "magic_enum.h"
#include <functional>

int Input::keyboard_player_id = 0; // Means that the keyboard will control player one

std::function<bool(int)> Input::gp_map[magic_enum::enum_count<GameKeys>()];
void Input::MapGameKeysToGamePad() {
	gp_map[GameKeys::UP] = [](int p) { // p is the player id
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

std::function<bool()> Input::kb_map[magic_enum::enum_count<GameKeys>()];
void Input::MapGameKeysToKeyboard()
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

