#include "camera.h"

namespace GameCamera {
    Camera2D camera = { {Window::GAME_WIDTH / 2, Window::GAME_HEIGHT / 2}, {0.f, 0.f}, 0, 1.f };

    //Useful for debug pourposes
    void MoveCameraWithArrows(float dt, float velocity) {
        vec c = Center();
        float zoom = Zoom();
        if (IsKeyDown(KeyboardKey::KEY_RIGHT))
        {
            c.x += velocity * dt * 10 / zoom;
        }
        if (IsKeyDown(KeyboardKey::KEY_LEFT))
        {
            c.x -= velocity * dt * 10 / zoom;
        }
        if (IsKeyDown(KeyboardKey::KEY_DOWN))
        {
            c.y += velocity * dt * 10 / zoom;
        }
        if (IsKeyDown(KeyboardKey::KEY_UP))
        {
            c.y -= velocity * dt * 10 / zoom;
        }
        SetCenter(c);
    }

    void ChangeZoomWithPlusAndMinus(float dt, float zoomVel)
    {
        float zoom = Zoom();
        if (IsKeyDown(KeyboardKey::KEY_EQUAL) || IsKeyDown(KeyboardKey::KEY_KP_ADD))
        {
            zoom += zoomVel * dt;
            SetZoom(zoom);
        }
        if (IsKeyDown(KeyboardKey::KEY_MINUS) || IsKeyDown(KeyboardKey::KEY_KP_SUBTRACT)) {
            zoom -= zoomVel * dt;
            if (zoom < 0.01f) zoom = 0.01f;
            SetZoom(zoom);
        }
    }

    void RotateWithPagUpDown(float dt, float velocity) {
        float r = GetRotationDegs();
        if (IsKeyDown(KeyboardKey::KEY_PAGE_DOWN))
        {
            r += velocity * dt;
        }
        if (IsKeyDown(KeyboardKey::KEY_PAGE_UP))
        {
            r -= velocity * dt;
        }
        SetRotationDegs(r);
    }

}


namespace GuiCamera {
    Camera2D gui_camera = { {0.f, 0.f}, {0.f, 0.f}, 0, 1.f };
}