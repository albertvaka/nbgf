#include "raw_input.h"

#include <SDL.h>

#include "camera.h"

const GamePad::AnalogStick GamePad::AnalogStick::Left(SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY);
const GamePad::AnalogStick GamePad::AnalogStick::Right(SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY);
GamePad::Trigger::LeftTrigger GamePad::Trigger::Left;
GamePad::Trigger::RightTrigger GamePad::Trigger::Right;
SDL_GameController* GamePad::joysticks[max_gamepads];
KeyStates GamePad::button_states[max_gamepads][SDL_CONTROLLER_BUTTON_MAX];

KeyStates Mouse::button_states[magic_enum::enum_count<Button>()] = { RELEASED };
float Mouse::scrollWheel = 0;
vec Mouse::pos(0, 0);
vec Mouse::oldPos(0, 0);

Uint8 Keyboard::state_buffers[2][SDL_NUM_SCANCODES] = { { 0 } };
Uint8* Keyboard::state = state_buffers[0];
Uint8* Keyboard::prev_state = state_buffers[1];

KeyStates GamePad::calculateJustPressed(bool pressed, KeyStates state)
{
    if (pressed)
	{
        if (state == JUST_PRESSED || state == PRESSED)
		{
            state = PRESSED;
        }
        else
		{
            state = JUST_PRESSED;
        }
    }
    else
	{
        if (state == JUST_RELEASED || state == RELEASED)
		{
            state = RELEASED;
        }
        else
		{
            state = JUST_RELEASED;
        }
    }
    return state;
}

void GamePad::_UpdateInputState__XboxNormal(SDL_GameController* joystick, int player)
{
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
    {
        bool pressed = SDL_GameControllerGetButton(joystick, SDL_GameControllerButton(i));
        button_states[player][i] = calculateJustPressed(pressed, button_states[player][i]);
    }
    {
        bool pressed = (Trigger::Left.get(player) > 50);
        Trigger::Left.state[player] = calculateJustPressed(pressed, Trigger::Left.state[player]);
    }
    {
        bool pressed = (Trigger::Right.get(player) > 50);
        Trigger::Right.state[player] = calculateJustPressed(pressed, Trigger::Right.state[player]);
    }
}


void GamePad::_Added(SDL_GameController* joystick) {
    Debug::out << "Added joystick " << joystick;
    for (int player = 0; player < max_gamepads; player++) {
        if (!joysticks[player]) {
            joysticks[player] = joystick;
            break;
        }
    }
}

void GamePad::_Removed(SDL_GameController* joystick) {
    Debug::out << "Removed joystick " << joystick;
    for (int player = 0; player < max_gamepads; player++) {
        if (joystick == joysticks[player]) {
            joysticks[player] = nullptr;
            break;
        }
    }
}

void GamePad::_UpdateInputState() {
    for (int player = 0; player < max_gamepads; player++) {
        SDL_GameController* joystick = joysticks[player];
        if (joystick) {
            _UpdateInputState__XboxNormal(joystick, player);
        }
        else {
            for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
                button_states[player][i] = calculateJustPressed(false, button_states[player][i]);
            }
        }
    }
}

void Mouse::_UpdateInputState()
{
    oldPos = pos;
    int int_x, int_y;
    int pressed = SDL_GetMouseState(&int_x, &int_y);
    pos.x = int_x;
    pos.y = int_y;

    // Convert mouse position to scaled, letterboxed game coordinates
    // --------------------------------------------------------------

#ifdef __EMSCRIPTEN__
    // In emscripten, mouse position is not in the same coordinates as the viewport, convert them first

    // Normalize mouse coordinates
    pos.x /= Window::target->context->window_w;
    pos.y /= Window::target->context->window_h;

    // Expand to viewport coordinates
    pos.x *= Window::target->context->drawable_w; // equals to the size of the viewport + margins: 2*Window::target->viewport.x + Window::target->viewport.w;
    pos.y *= Window::target->context->drawable_h;
#endif

    // Remove margins
    pos.x -= Window::target->viewport.x;
    pos.y -= Window::target->viewport.y;

    // Normalize to viewport coordinates without margin
    pos.x /= Window::target->viewport.w;
    pos.y /= Window::target->viewport.h;

    // Expand to game coordinates
    pos.x *= Window::GAME_WIDTH;
    pos.y *= Window::GAME_HEIGHT;


    // Update mouse button states
    // --------------------------

    for (size_t i = 1; i < magic_enum::enum_count<Button>(); i++) //skip NONE
    {
        if (pressed & SDL_BUTTON(i))
        {
            if (button_states[i] == JUST_PRESSED || button_states[i] == PRESSED)
            {
                button_states[i] = PRESSED;
            }
            else
            {
                button_states[i] = JUST_PRESSED;
            }
        }
        else
        {
            if (button_states[i] == JUST_RELEASED || button_states[i] == RELEASED)
            {
                button_states[i] = RELEASED;
            }
            else
            {
                button_states[i] = JUST_RELEASED;
            }
        }
    }

}

vec Mouse::GetPositionInWorld()
{
    return (GetPositionInWindow() / Camera::camera.zoom_x) + vec(Camera::camera.x,Camera::camera.y);
}

void Keyboard::_UpdateInputState()
{
    //if(Window::HasFocus())

    auto aux = prev_state;
    prev_state = state;
    state = aux;
    memcpy(state, SDL_GetKeyboardState(NULL), SDL_NUM_SCANCODES);

}
