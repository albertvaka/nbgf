#include "input_conf.h"

#include "input.h"

#include "magic_enum.h"
#include <functional>

int Input::keyboard_player_id = 0; // Means that the keyboard will control player one

std::function<bool(int)> Input::gp_map[magic_enum::enum_count<GameKeys>()];
void Input::MapGameKeysToGamePad() 
{
    gp_map[(int)GameKeys::MENU_UP] = [](int p) // p is the player id
    { 
        return GamePad::AnalogStick::Left.get(p, 30.f).y < -50.0f ||
               GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_UP);
    };
    gp_map[(int)GameKeys::MENU_DOWN] = [](int p) 
    {
        return GamePad::AnalogStick::Left.get(p, 30.f).y > -50.0f ||
               GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    };
    gp_map[(int)GameKeys::JUMP] = [](int p) 
    {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_X) ||
               GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_B) ||
               GamePad::AnalogStick::Left.get(p, 30.f).y < -50.0f ||
               GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_UP);
    };
    gp_map[(int)GameKeys::CROUCH] = [](int p) 
    {
        return GamePad::AnalogStick::Left.get(p, 30.f).y > 50.0f ||
               GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    };
    gp_map[(int)GameKeys::LEFT] = [](int p) 
    {
        return GamePad::AnalogStick::Left.get(p, 20.f).x < 0.0f ||
               GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    };
    gp_map[(int)GameKeys::RIGHT] = [](int p)
    {
        return GamePad::AnalogStick::Left.get(p, 20.f).x > 0.0f ||
               GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    };
    gp_map[(int)GameKeys::ACTION] = [](int p) {
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_A); 
    };
    gp_map[(int)GameKeys::START] = [](int p) 
    { 
        return GamePad::IsButtonPressed(p, SDL_CONTROLLER_BUTTON_START); 
    };
}

std::function<bool()> Input::kb_map[magic_enum::enum_count<GameKeys>()];
void Input::MapGameKeysToKeyboard()
{
    kb_map[(int)GameKeys::JUMP] = kb_map[(int)GameKeys::MENU_UP] = []()
    {
        return Keyboard::IsKeyPressed(SDL_SCANCODE_W) ||
               Keyboard::IsKeyPressed(SDL_SCANCODE_UP);
    };
    kb_map[(int)GameKeys::CROUCH] = kb_map[(int)GameKeys::MENU_DOWN] = []()
    {
        return Keyboard::IsKeyPressed(SDL_SCANCODE_S) || 
               Keyboard::IsKeyPressed(SDL_SCANCODE_DOWN);
    };
    kb_map[(int)GameKeys::LEFT] = []() 
    {
        return Keyboard::IsKeyPressed(SDL_SCANCODE_A) ||
               Keyboard::IsKeyPressed(SDL_SCANCODE_LEFT);
    };
    kb_map[(int)GameKeys::RIGHT] = []()
    {
        return Keyboard::IsKeyPressed(SDL_SCANCODE_D) || 
               Keyboard::IsKeyPressed(SDL_SCANCODE_RIGHT);
    };
    kb_map[(int)GameKeys::ACTION] = []()
    {
        return Keyboard::IsKeyPressed(SDL_SCANCODE_E) || 
               Keyboard::IsKeyPressed(SDL_SCANCODE_SPACE);
    };
    kb_map[(int)GameKeys::START] = []() 
    {
        return Keyboard::IsKeyPressed(SDL_SCANCODE_RETURN) || 
               Keyboard::IsKeyPressed(SDL_SCANCODE_ESCAPE);
    };
}

