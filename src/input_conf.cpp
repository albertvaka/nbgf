#include "input_conf.h"

#include "input.h"
#include "player.h"

#include "magic_enum.h"
#include <functional>

static int keyboard_player_id = 0; // Keyboard controls player one

static bool aimingWithMouse = true;
static vec lastAnalogAim;

std::function<bool(int)> Input::action_mapping[magic_enum::enum_count<GameKeys>()];
std::function<vec(int)> Input::analog_mapping[magic_enum::enum_count<AnalogInput>()];
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
        return GamePad::AnalogStick::Left.get(p).y > 0.0f ||
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
    analog_mapping[(int)AnalogInput::AIM] = [](int p)
    {
        vec joystick = GamePad::AnalogStick::Right.get(p);
        if (joystick != vec::Zero) {
            aimingWithMouse = false;
        }
        if (Mouse::IsPressed()) {
            aimingWithMouse = true;
        }
        if (p == keyboard_player_id && aimingWithMouse) {
            return Mouse::GetPositionInWorld();
        }
        if (joystick != vec::Zero) {
            lastAnalogAim = joystick;
        }
        return Player::instance()->pos + lastAnalogAim;
    };
}
