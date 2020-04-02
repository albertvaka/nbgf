#include "input.h"

#include "imgui.h"
#include "imgui-SFML.h"

int GamePad::player_to_joystick[GamePad::JoystickCountMax];

const GamePad::AnalogStick GamePad::AnalogStick::Left(sf::Joystick::Axis::X, sf::Joystick::Axis::Y);
const GamePad::AnalogStick GamePad::AnalogStick::Right(sf::Joystick::Axis::U, sf::Joystick::Axis::V);

GamePad::Trigger::LeftTrigger GamePad::Trigger::Left;
GamePad::Trigger::RightTrigger GamePad::Trigger::Right;

KeyStates GamePad::button_states[GamePad::JoystickCountMax][sf::Joystick::ButtonCount];
KeyStates Mouse::button_states[sf::Mouse::ButtonCount];
KeyStates Keyboard::key_states[magic_enum::enum_count<GameKeys>()];
float Keyboard::key_times[magic_enum::enum_count<GameKeys>()];
sf::Keyboard::Key key_map[magic_enum::enum_count<GameKeys>()];



static void _ProcessWindowEvents();

namespace Window
{

    sf::RenderWindow* window;
    bool focus = true;

    void SetWindowCaption(const std::string& s)
	{
        window->setTitle(s);
    }

    bool WindowHasFocus()
	{
        return focus;
    }

    void CloseWindow()
	{
        window->close();
    }

    void SetWindowSize(sf::Vector2u size, bool centerCamera)
	{
        window->setSize(size);
        _ProcessWindowEvents();
        if (centerCamera) Camera::SetCameraCenter(size / 2);
    }

    sf::Vector2u GetWindowSize()
	{
        return window->getSize();
    }

    Bounds GetWindowBounds()
	{
        return Bounds(0.f, 0.f, (float)window->getSize().x, (float)window->getSize().y);
    }

    bool IsMouseInsideWindow()
	{
        sf::Vector2i vec = sf::Mouse::getPosition(*window);
        if (vec.x < 0 || vec.x >(int)window->getSize().x) return false;
        if (vec.y < 0 || vec.y >(int)window->getSize().y) return false;
        return true;
    }
}

namespace Camera
{
    sf::View gameView;
    sf::View guiView;
    float zoom;


    void SetCameraCenter(const vec& center)
	{
        gameView.setCenter(center);
        Window::window->setView(gameView);
        _ProcessWindowEvents();
    }

    vec GetCameraCenter()
	{
        return gameView.getCenter();
    }

    Bounds GetCameraBounds()
	{
        sf::Vector2f viewSize = gameView.getSize();
        vec viewOrigin = gameView.getCenter() - (viewSize / 2.f);
        return Bounds(viewOrigin.x, viewOrigin.y, viewSize.x, viewSize.y);
    }


    void ClampCameraTo(const Bounds& limit)
	{
        vec c = GetCameraCenter();

        vec screenSize = Window::GetWindowSize();
        screenSize /= zoom;
        float halfScreenWidth = screenSize.x / 2.f;
        float halfScreenHeight = screenSize.y / 2.f;

        //TODO: Center if viewport is bigger than limits
        if (c.x + halfScreenWidth > limit.Right()) c.x = limit.Right() - halfScreenWidth;
        if (c.x - halfScreenWidth < limit.Left()) c.x = limit.Left() + halfScreenWidth;
        if (c.y + halfScreenHeight > limit.Bottom()) c.y = limit.Bottom() - halfScreenHeight;
        if (c.y - halfScreenHeight < limit.Top()) c.y = limit.Top() + halfScreenHeight;

        SetCameraCenter(c);
    }

    void ResetCamera()
	{
        gameView.setSize(sf::Vector2f(Window::window->getSize()));
        gameView.setCenter(vec(Window::window->getSize()) / 2);
        zoom = 1.f;
        gameView.zoom(1.f / zoom);
        gameView.setViewport(sf::FloatRect(0, 0, 1, 1));
        //GUI View is never moved so it shouldn't be necessary to reset it
        Window::window->setView(gameView);
        _ProcessWindowEvents();
    }

    void ResetGuiCamera()
	{
        guiView.setSize(sf::Vector2f(Window::window->getSize()));
        guiView.setCenter(vec(Window::window->getSize()) / (2*GameData::GUI_ZOOM));
        guiView.zoom(1.f / GameData::GUI_ZOOM);
        guiView.setViewport(sf::FloatRect(0, 0, 1, 1));
    }

    void SetZoom(float z)
	{
        gameView.zoom(zoom);
        zoom = z;
        gameView.zoom(1.f / zoom);
        Window::window->setView(gameView);
        _ProcessWindowEvents();
    }

    float GetZoom()
	{
        return zoom;
    }

    void StartGuiDraw()
	{
        Window::window->setView(guiView);
    }

    void EndGuiDraw()
	{
        Window::window->setView(gameView);
    }
}

KeyStates GamePad::calculateJustPressed(bool pressed, KeyStates state)
{
    if (pressed)
	{
        //FIXME: No hauria d'anar per frames, sino per temps.
        // Si porta apretat < X milis, esta justpressed
        // Per fer tricks com deixar saltar "tard" podriem incrementar aquest temps (parametre extra a justpressed?)
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

void GamePad::_UpdateInputState__MandoSteam(int joy, int player)
{
    //TODO: Fix this
    for (int i = 0; i < sf::Joystick::ButtonCount; i++)
    {
        bool pressed = (sf::Joystick::isButtonPressed(joy, i));
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

void GamePad::_UpdateInputState__XboxNormal(int joy, int player)
{
    for (int i = 0; i < sf::Joystick::ButtonCount; i++)
    {
        bool pressed = (sf::Joystick::isButtonPressed(joy, i));
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


void Mouse::_UpdateInputState()
{
    for (int i = 0; i < sf::Mouse::ButtonCount; i++)
	{
        if (sf::Mouse::isButtonPressed((sf::Mouse::Button)i))
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

sf::Vector2i Mouse::GetPositionInWindow()
{
    return sf::Mouse::getPosition(*Window::window); //window arg is needed for relative coords
}

vec Mouse::GetPositionInWorld()
{
    vec displacement = Camera::gameView.getCenter() - (Camera::gameView.getSize() / 2.f);
    return (vec(GetPositionInWindow())/Camera::zoom) + displacement;
}


void GamePad::_UpdateInputState()
    {
        int player = 0;
        for (int joystick = 0; joystick < JoystickCountMax; ++joystick)
        {
            if (!sf::Joystick::isConnected(joystick))
            {
                continue;
            }
            player_to_joystick[player] = joystick;


            sf::Joystick::Identification id_joy = sf::Joystick::getIdentification(joystick);

            const int ID_MANDO_STEAM = 999999;
            switch (id_joy.productId)
            {
            case ID_MANDO_STEAM:
            {
                _UpdateInputState__MandoSteam(joystick, player);
            } break;

            default:
            {
                _UpdateInputState__XboxNormal(joystick, player);
            } break;
            }

            player++;

        }

        while (player < JoystickCountMax)
        {
            player_to_joystick[player] = -1;
            for (int i = 0; i < sf::Joystick::ButtonCount; i++)
            {
                button_states[player][i] = calculateJustPressed(false, button_states[player][i]);
            }
            player++;
        }

}

void Keyboard::_UpdateInputState(float dt)
{
    for (size_t i = 0; i < magic_enum::enum_count<GameKeys>(); i++)
	{
        if (sf::Keyboard::isKeyPressed(key_map[i]) && Window::WindowHasFocus())
		{
            if (key_states[i] == JUST_PRESSED || key_states[i] == PRESSED)
			{
                key_states[i] = PRESSED;
                if (key_times[i] < 1000.f) key_times[i] += dt;
            }
            else
			{
                key_states[i] = JUST_PRESSED;
                key_times[i] = dt;
            }
        }
        else
		{
            if (key_states[i] == JUST_RELEASED || key_states[i] == RELEASED)
			{
                key_states[i] = RELEASED;
                if (key_times[i] < 1000.f) key_times[i] += dt;
            }
            else
			{
                key_states[i] = JUST_RELEASED;
                key_times[i] = dt;
            }
        }
    }

}

static void _ProcessWindowEvents()
{

    sf::Event sfmlevent;
    while (Window::window->pollEvent(sfmlevent))
	{
        ImGui::SFML::ProcessEvent(sfmlevent);
        switch (sfmlevent.type) {
        case sf::Event::LostFocus:
            Window::focus = false;
            break;
        case sf::Event::GainedFocus:
            Window::focus = true;
            break;
        case sf::Event::Closed:
            Window::window->close();
            break;
        case sf::Event::Resized: //To disable sfml's automatic scaling
        {
            // GameView: scale from center
            vec currentCenter = Camera::gameView.getCenter();
            Camera::gameView.setSize(sf::Vector2f(Window::window->getSize()));
            Camera::gameView.setCenter(currentCenter);
            Camera::gameView.zoom(1.f / Camera::zoom);
        }
        {
            // GuiView: scale from top-left corner
            vec oldSize = Camera::guiView.getSize() * (GameData::GUI_ZOOM);
            vec newSize = sf::Vector2f(Window::window->getSize());
            Camera::guiView.setSize(newSize);
            Camera::guiView.zoom(1.f / GameData::GUI_ZOOM);
            Camera::guiView.setCenter(vec(Camera::guiView.getCenter()) + (newSize-oldSize)/(2*GameData::GUI_ZOOM));
        }
        break;
        default:
            break;
        }
    }
}
namespace Input
{
    void Update(sf::Time deltaTime)
	{
        ImGui::SFML::Update(*Window::window, deltaTime);
        _ProcessWindowEvents();
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard) {
            Keyboard::_UpdateInputState(deltaTime.asSeconds());
        }
        if (!io.WantCaptureMouse) {
            Mouse::_UpdateInputState();
        }
        GamePad::_UpdateInputState();
    }
    void Init(sf::RenderWindow & renderwindow)
	{

        Window::window = &renderwindow;
        Camera::ResetCamera();
        Camera::ResetGuiCamera();
        RemapInput();
        for (size_t i = 0; i < magic_enum::enum_count<GameKeys>(); i++) Keyboard::key_states[i] = RELEASED;
        for (size_t i = 0; i < sf::Mouse::ButtonCount; i++) Mouse::button_states[i] = RELEASED;

    }
}
