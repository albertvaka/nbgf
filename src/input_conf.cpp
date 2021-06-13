#include "input_conf.h"

#include "input.h"

#include "magic_enum.h"
#include <functional>

static int keyboard_player_id = 0; // Keyboard controls player one

std::function<bool(int)> Input::action_mapping[magic_enum::enum_count<GameKeys>()];
std::function<vec(int)> Input::analog_mapping[magic_enum::enum_count<AnalogInput>()];
void Input::MapGameKeys()
{
    action_mapping[(int)GameKeys::UP] = [](int p) // p is the player id
    {
        return GamePad::AnalogStick::Left.get(p).y < 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_UP) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_UP)
                    )
                );
    };
    action_mapping[(int)GameKeys::DOWN] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).y > 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_DOWN) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_DOWN)
                    )
                );
    };
    action_mapping[(int)GameKeys::LEFT] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).x < 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_LEFT) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_LEFT)
                    )
                );
    };
    action_mapping[(int)GameKeys::RESTART] = [](int p)
    {
        return (p == keyboard_player_id) && (Keyboard::IsKeyPressed(SDL_SCANCODE_R));
    };
    action_mapping[(int)GameKeys::RIGHT] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).x > 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_RIGHT)
                    )
                );
    };
    action_mapping[(int)GameKeys::UP2] = [](int p) // now for the left node
    {
        return GamePad::AnalogStick::Left.get(p).y < 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_UP) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_W)
                    )
                );
    };
    action_mapping[(int)GameKeys::DOWN2] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).y > 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_DOWN) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_S)
                    )
                );
    };
    action_mapping[(int)GameKeys::LEFT2] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).x < 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_LEFT) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_A)
                    )
                );
    };
    action_mapping[(int)GameKeys::RIGHT2] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).x > 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_D)
                    )
                );
    };
    action_mapping[(int)GameKeys::SHOOT] = [](int p) {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_X) ||
            GamePad::Trigger::Right.IsPressed(p) || (
                (p == keyboard_player_id) && Mouse::IsPressed()
                );
    };
    action_mapping[(int)GameKeys::START] = [](int p)
    {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_START) || (
            (p == keyboard_player_id) && (
                Keyboard::IsKeyPressed(SDL_SCANCODE_RETURN) ||
                Keyboard::IsKeyPressed(SDL_SCANCODE_ESCAPE)
                )
            );
    };
    analog_mapping[(int)AnalogInput::MOVE] = [](int p)
    {
        vec ret = vec::Zero;
        if (p == keyboard_player_id) {
            if (Keyboard::IsKeyPressed(SDL_SCANCODE_W) ||
                Keyboard::IsKeyPressed(SDL_SCANCODE_UP)) {
                ret.y = -100;
            }
            if (Keyboard::IsKeyPressed(SDL_SCANCODE_S) ||
                Keyboard::IsKeyPressed(SDL_SCANCODE_DOWN)) {
                ret.y = 100;
            }
            if (Keyboard::IsKeyPressed(SDL_SCANCODE_A) ||
                Keyboard::IsKeyPressed(SDL_SCANCODE_LEFT)) {
                ret.x = -100;
            }
            if (Keyboard::IsKeyPressed(SDL_SCANCODE_D) ||
                Keyboard::IsKeyPressed(SDL_SCANCODE_RIGHT)) {
                ret.x = 100;
            }
        }
        return ret != vec::Zero ? ret : GamePad::AnalogStick::Left.get(p);
    };
}
