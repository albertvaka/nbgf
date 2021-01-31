#include "input_conf.h"

#include "input.h"

#include "magic_enum.h"
#include <functional>

std::function<bool(int)> Input::action_mapping[magic_enum::enum_count<GameKeys>()];
void Input::MapGameKeys()
{
    action_mapping[(int)GameKeys::UP] = [](int p) // p is the player number, 0-based
    {
        return GamePad::AnalogStick::Left.get(p, 30.f).y < -50.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_W) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_UP);
    };
    action_mapping[(int)GameKeys::DOWN] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p, 30.f).y > 50.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_S) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_DOWN);
    };
    action_mapping[(int)GameKeys::LEFT] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p, 20.f).x < 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_A) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_LEFT);
    };
    action_mapping[(int)GameKeys::RIGHT] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p, 20.f).x > 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_D) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_RIGHT);
    };
    action_mapping[(int)GameKeys::ACTION] = [](int p)
    {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_A) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_E) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_SPACE);
    };
    action_mapping[(int)GameKeys::RUN] = [](int p)
    {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_A) ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_B) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_E) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_SPACE);
    };
    action_mapping[(int)GameKeys::START] = [](int p)
    { 
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_START) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_RETURN) ||
            Keyboard::IsKeyPressed(SDL_SCANCODE_ESCAPE);
    };
}
