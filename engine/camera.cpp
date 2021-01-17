#include "camera.h"
#include "raw_input.h"

namespace Camera {
    GPU_Camera camera;
    GPU_Camera gui_camera;
}

namespace Camera {

    //Useful for debug pourposes
    void MoveCameraWithArrows(float dt, float velocity) {
        vec c = Center();
        float zoom = Zoom();
        if (Keyboard::IsKeyPressed(SDL_SCANCODE_RIGHT))
        {
            c.x += velocity * dt * 10 / zoom;
        }
        if (Keyboard::IsKeyPressed(SDL_SCANCODE_LEFT))
        {
            c.x -= velocity * dt * 10 / zoom;
        }
        if (Keyboard::IsKeyPressed(SDL_SCANCODE_DOWN))
        {
            c.y += velocity * dt * 10 / zoom;
        }
        if (Keyboard::IsKeyPressed(SDL_SCANCODE_UP))
        {
            c.y -= velocity * dt * 10 / zoom;
        }
        SetCenter(c);
    }

    void ChangeZoomWithPlusAndMinus(float dt, float zoomVel)
    {
        float zoom = Zoom();
        if (Keyboard::IsKeyPressed(SDL_SCANCODE_EQUALS) || Keyboard::IsKeyPressed(SDL_SCANCODE_KP_PLUS))
        {
            zoom += zoomVel * dt;
            SetZoom(zoom);
        }
        if (Keyboard::IsKeyPressed(SDL_SCANCODE_MINUS) || Keyboard::IsKeyPressed(SDL_SCANCODE_KP_MINUS)) {
            zoom -= zoomVel * dt;
            if (zoom < 0.01f) zoom = 0.01f;
            SetZoom(zoom);
        }
    }

    void RotateWithPagUpDown(float dt, float velocity) {
        float r = GetRotationDegs();
        if (Keyboard::IsKeyPressed(SDL_SCANCODE_PAGEDOWN))
        {
            r += velocity * dt;
        }
        if (Keyboard::IsKeyPressed(SDL_SCANCODE_PAGEUP))
        {
            r -= velocity * dt;
        }
        SetRotationDegs(r);
    }

}
