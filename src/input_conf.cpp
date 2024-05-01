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
    action_mapping[(int)GameKeys::UP] = [](int p) // p is the player id
    {
        return GetGamepadAxisMovement(p, GamepadAxis::GAMEPAD_AXIS_LEFT_Y) < -50.0f ||
            IsGamepadButtonPressed(p, GamepadButton::GAMEPAD_BUTTON_LEFT_FACE_UP) || (
                (p == keyboard_player_id) && (
                    IsKeyDown(KeyboardKey::KEY_W) ||
                    IsKeyDown(KeyboardKey::KEY_UP)
                    )
                );
    };
    action_mapping[(int)GameKeys::DOWN] = [](int p)
    {
        return GetGamepadAxisMovement(p, GamepadAxis::GAMEPAD_AXIS_LEFT_Y) > -50.0f ||
            IsGamepadButtonPressed(p, GamepadButton::GAMEPAD_BUTTON_LEFT_FACE_DOWN) || (
                (p == keyboard_player_id) && (
                    IsKeyDown(KeyboardKey::KEY_S) ||
                    IsKeyDown(KeyboardKey::KEY_DOWN)
                    )
                );
    };
    action_mapping[(int)GameKeys::LEFT] = [](int p)
    {
        return GetGamepadAxisMovement(p, GamepadAxis::GAMEPAD_AXIS_LEFT_X) < 0.0f ||
            IsGamepadButtonPressed(p, GamepadButton::GAMEPAD_BUTTON_LEFT_FACE_LEFT) || (
                (p == keyboard_player_id) && (
                    IsKeyDown(KeyboardKey::KEY_A) ||
                    IsKeyDown(KeyboardKey::KEY_LEFT)
                    )
                );
    };
    action_mapping[(int)GameKeys::RIGHT] = [](int p)
    {
        return GetGamepadAxisMovement(p, GamepadAxis::GAMEPAD_AXIS_LEFT_X) > 0.0f ||
            IsGamepadButtonPressed(p, GamepadButton::GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || (
                (p == keyboard_player_id) && (
                    IsKeyDown(KeyboardKey::KEY_D) ||
                    IsKeyDown(KeyboardKey::KEY_RIGHT)
                    )
                );
    };
    action_mapping[(int)GameKeys::SHOOT] = [](int p) {
        return IsGamepadButtonPressed(p, GamepadButton::GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
            IsGamepadButtonPressed(p, GamepadButton::GAMEPAD_BUTTON_RIGHT_TRIGGER_2) || (
                (p == keyboard_player_id) && (
                    IsMouseButtonDown(MouseButton::MOUSE_BUTTON_LEFT)
                    || IsKeyPressed(KeyboardKey::KEY_SPACE)
                    )
                );
    };
    action_mapping[(int)GameKeys::START] = [](int p)
    {
        return IsGamepadButtonPressed(p, GamepadButton::GAMEPAD_BUTTON_MIDDLE_RIGHT) || (
            (p == keyboard_player_id) && (
                IsKeyDown(KeyboardKey::KEY_ENTER) ||
                IsKeyDown(KeyboardKey::KEY_ESCAPE)
                )
            );
    };
    analog_mapping[(int)AnalogInput::MOVE] = [](int p)
    {
        vec ret = vec::Zero;
        if (p == keyboard_player_id) {
            if (IsKeyDown(KeyboardKey::KEY_W) ||
                IsKeyDown(KeyboardKey::KEY_UP)) {
                ret.y = -100;
            }
            if (IsKeyDown(KeyboardKey::KEY_S) ||
                IsKeyDown(KeyboardKey::KEY_DOWN)) {
                ret.y = 100;
            }
            if (IsKeyDown(KeyboardKey::KEY_A) ||
                IsKeyDown(KeyboardKey::KEY_LEFT)) {
                ret.x = -100;
            }
            if (IsKeyDown(KeyboardKey::KEY_D) ||
                IsKeyDown(KeyboardKey::KEY_RIGHT)) {
                ret.x = 100;
            }
        }
        return ret != vec::Zero ? ret : vec(GetGamepadAxisMovement(p, GamepadAxis::GAMEPAD_AXIS_LEFT_X), GetGamepadAxisMovement(p, GamepadAxis::GAMEPAD_AXIS_LEFT_Y));
    };
}
