#include "input_conf.h"

#include "input.h"

#include <functional>

const int Input::kMaxPlayers = 2;

static int keyboard_player_id = 0; // Keyboard controls player one

std::function<bool(int)> Input::action_mapping[magic_enum::enum_count<GameKeys>()];
std::function<vec(int)> Input::analog_mapping[magic_enum::enum_count<AnalogInput>()];
KeyStates Input::action_states[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()] = { { RELEASED } };
float Input::action_times[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()] = { { 0 } };
vec Input::analog_states[Input::kMaxPlayers][magic_enum::enum_count<AnalogInput>()];

void Input::MapGameKeys()
{
    action_mapping[(int)GameKeys::UP] = [](int p) // p is the player id
    {
        return GamePad::AnalogStick::Left.get(p).y < 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_UP) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_W) ||
                    Keyboard::IsKeyPressed(SDL_SCANCODE_UP)
                    )
                );
    };
    action_mapping[(int)GameKeys::DOWN] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).y > 0.0f||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_DOWN) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_S) ||
                    Keyboard::IsKeyPressed(SDL_SCANCODE_DOWN)
                    )
                );
    };
    action_mapping[(int)GameKeys::LEFT] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).x < 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_LEFT) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_A) ||
                    Keyboard::IsKeyPressed(SDL_SCANCODE_LEFT)
                    )
                );
    };
    action_mapping[(int)GameKeys::RIGHT] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).x > 0.0f ||
            GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_D) ||
                    Keyboard::IsKeyPressed(SDL_SCANCODE_RIGHT)
                    )
                );
    };
    action_mapping[(int)GameKeys::DRIFT] = [](int p) {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_X) ||
            GamePad::Trigger::Right.IsPressed(p) || (
                (p == keyboard_player_id) && (Keyboard::IsKeyPressed(SDL_SCANCODE_SPACE))
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
