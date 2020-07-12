#include "camera.h"
#include "raw_input.h"

namespace Camera {
    GPU_Camera camera;
    GPU_Camera gui_camera;
}

namespace Camera {

    //Useful for debug pourposes
    void MoveCameraWithArrows(float velocity, float dt) {
        vec c = GetTopLeft();
        float zoom = GetZoom();
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
        SetTopLeft(c);
    }

    void ChangeZoomWithPlusAndMinus(float zoomVel, float dt)
    {
        float zoom = GetZoom();
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
}
