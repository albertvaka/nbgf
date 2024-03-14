#include "input_conf.h"

#include "input.h"
#include "player.h"

#include <functional>

const int Input::kMaxPlayers = 2;

static int keyboard_player_id = 0; // Keyboard controls player one
static bool aimingWithMouse = true;
static vec lastAnalogAim;

std::function<bool(int)> Input::action_mapping[magic_enum::enum_count<GameKeys>()];
std::function<vec(int)> Input::analog_mapping[magic_enum::enum_count<AnalogInput>()];
KeyStates Input::action_states[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()] = { { RELEASED } };
float Input::action_times[Input::kMaxPlayers][magic_enum::enum_count<GameKeys>()] = { { 0 } };
vec Input::analog_states[Input::kMaxPlayers][magic_enum::enum_count<AnalogInput>()];

void Input::MapGameKeys()
{
    action_mapping[(int)GameKeys::MENU_UP] = [](int p) // p is the player id
    {
        return GamePad::AnalogStick::Left.get(p).y < -50.0f ||
               GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_UP) || (
               (p == keyboard_player_id) && (
                   Keyboard::IsKeyPressed(SDL_SCANCODE_W) ||
                   Keyboard::IsKeyPressed(SDL_SCANCODE_UP)
               )
        );
    };
    action_mapping[(int)GameKeys::MENU_DOWN] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).y > -50.0f ||
               GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_DOWN) || (
               (p == keyboard_player_id) && (
                   Keyboard::IsKeyPressed(SDL_SCANCODE_S) ||
                   Keyboard::IsKeyPressed(SDL_SCANCODE_DOWN)
               )
        );
    };
    action_mapping[(int)GameKeys::JUMP] = [](int p)
    {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_A) || (
               (p == keyboard_player_id) && (
                   Keyboard::IsKeyPressed(SDL_SCANCODE_Z) ||
                   Keyboard::IsKeyPressed(SDL_SCANCODE_P) ||
                   Keyboard::IsKeyPressed(SDL_SCANCODE_SPACE)
               )
        );
    };
    action_mapping[(int)GameKeys::CROUCH] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p).y > 50.0f ||
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
    action_mapping[(int)GameKeys::ACTION] = [](int p) {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_B) || (
               (p == keyboard_player_id) && (
                   Keyboard::IsKeyPressed(SDL_SCANCODE_E) ||
                   Keyboard::IsKeyPressed(SDL_SCANCODE_SPACE)
               )
        );
    };
    action_mapping[(int)GameKeys::SHOOT] = [](int p) {
        return GamePad::Trigger::Right.IsPressed(p) || (
               (p == keyboard_player_id) && Mouse::IsPressed()
        );
    };
    action_mapping[(int)GameKeys::ATTACK] = [](int p) {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_X) || (
               (p == keyboard_player_id) && (
                   Keyboard::IsKeyPressed(SDL_SCANCODE_O) ||
                   Keyboard::IsKeyPressed(SDL_SCANCODE_X)
               )
        );
    };
    action_mapping[(int)GameKeys::DASH] = [](int p) {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) || (
               (p == keyboard_player_id) && (
                   Keyboard::IsKeyPressed(SDL_SCANCODE_C) ||
                   Keyboard::IsKeyPressed(SDL_SCANCODE_I)
               )
        );
    };

    action_mapping[(int)GameKeys::MENU_ACCEPT] = [](int p)
    {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_A) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_O)
                    || Keyboard::IsKeyPressed(SDL_SCANCODE_X)
                    || Keyboard::IsKeyPressed(SDL_SCANCODE_RETURN)
                    || Keyboard::IsKeyPressed(SDL_SCANCODE_KP_ENTER)
                    || Keyboard::IsKeyPressed(SDL_SCANCODE_SPACE)
               )
        );
    };
    action_mapping[(int)GameKeys::MENU_CANCEL] = [](int p)
    {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_B) || (
                (p == keyboard_player_id) && (
                    Keyboard::IsKeyPressed(SDL_SCANCODE_ESCAPE)
                    )
                );
    };
    action_mapping[(int)GameKeys::PAUSE] = [](int p)
    {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_START) || (
//            (p == keyboard_player_id) && (
                Keyboard::IsKeyPressed(SDL_SCANCODE_ESCAPE)
//              )
            );
    };
    action_mapping[(int)GameKeys::START] = [](int p)
        {
            return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_START) || (
                //            (p == keyboard_player_id) && (
                Keyboard::IsKeyPressed(SDL_SCANCODE_RETURN) || Keyboard::IsKeyPressed(SDL_SCANCODE_KP_ENTER)
                //              )
                );
        };


    // Analog

    analog_mapping[(int)AnalogInput::MOVE] = [](int p)
    {
        vec ret = vec::Zero;
        if (p == keyboard_player_id) {
            if (Keyboard::IsKeyPressed(SDL_SCANCODE_W) ||
                Keyboard::IsKeyPressed(SDL_SCANCODE_UP)) {
                   ret.y = -1;
            }
            if (Keyboard::IsKeyPressed(SDL_SCANCODE_S) ||
                Keyboard::IsKeyPressed(SDL_SCANCODE_DOWN)) {
                   ret.y = 1;
            }
            if (Keyboard::IsKeyPressed(SDL_SCANCODE_A) ||
                Keyboard::IsKeyPressed(SDL_SCANCODE_LEFT)) {
                   ret.x = -1;
            }
            if (Keyboard::IsKeyPressed(SDL_SCANCODE_D) ||
                Keyboard::IsKeyPressed(SDL_SCANCODE_RIGHT)) {
                   ret.x = 1;
            }
        }
        return ret != vec::Zero ? ret : (GamePad::AnalogStick::Left.get(p)*(1.1f/100.f)).Truncated(1.f);
    };

    analog_mapping[(int)AnalogInput::AIM] = [](int p)
    {
        vec joystick = GamePad::AnalogStick::Right.get(p);
        if (p == keyboard_player_id) {
            if (joystick != vec::Zero) {
                aimingWithMouse = false;
            }
            if (Mouse::IsPressed()) {
                aimingWithMouse = true;
            }
            if (aimingWithMouse) {
                return Mouse::GetPositionInWorld();
            }
        }
        if (joystick != vec::Zero) {
            lastAnalogAim = joystick;
        }
        // For 2 players, lastAnalogAim needs to be turned into an array.
        Player* player = Player::instance();
        vec playerPos = player ? player->pos : vec::Zero;
        return playerPos + lastAnalogAim;
    };
}

