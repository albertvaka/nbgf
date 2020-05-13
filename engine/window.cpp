#include "window.h"
#include "input.h"
//nclude "imgui_impl_sdl.h"
#include "debug.h"

#include "SDL_gpu.h"

namespace Camera {
    GPU_Camera camera;
    GPU_Camera gui_camera;
}
namespace Window
{
    SDL_Window* window;
    GPU_Target* target;
    bool focus = true;
    SDL_GLContext glcontext;
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

namespace Window
{
    void Init() {
        GPU_SetDebugLevel(GPU_DEBUG_LEVEL_1);

#ifdef __EMSCRIPTEN__
        int scale = 1;
#else
        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);
        int scale = Mates::MinOf(dm.w / GAME_WIDTH, dm.h / GAME_HEIGHT);
        Debug::out << "Scaling to x" << scale;
        //Debug::out << dm.w << " " << dm.h;
 #endif
        window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, GAME_WIDTH * scale, GAME_HEIGHT * scale, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
        if (window == NULL) {
            Debug::out << "Window Creation Error: " << SDL_GetError();
        }
        GPU_SetInitWindow(SDL_GetWindowID(window));

        target = GPU_Init(GAME_WIDTH, GAME_HEIGHT, GPU_DEFAULT_INIT_FLAGS);

        // [Debug] Disable vsync
        //SDL_GL_SetSwapInterval(0);

        // SDL-gpu anchors images at the center by default, change it to the top-left corner
        GPU_SetDefaultAnchor(0, 0);

        // FIXME: Too late for this game, but we have the option to set the Y coordinates the right way
        //GPU_SetCoordinateMode(false);

        GPU_EnableCamera(target, true);
        Camera::camera = GPU_GetDefaultCamera();
        Camera::camera.use_centered_origin = false;
        Camera::gui_camera = GPU_GetDefaultCamera();
        Camera::gui_camera.use_centered_origin = false;
        Camera::SetTopLeft(0, 0);

        GPU_SetVirtualResolution(Window::target, Window::GAME_WIDTH, Window::GAME_HEIGHT);
    }

    void ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT_FOCUS_LOST:
                focus = false;
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                focus = true;
                break;
            case SDL_CONTROLLERDEVICEADDED:
                GamePad::_Added(SDL_GameControllerOpen(event.jdevice.which));
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                GamePad::_Removed(SDL_GameControllerFromInstanceID(event.jdevice.which));
                break;
            case SDL_MOUSEWHEEL:
                Mouse::scrollWheel += event.wheel.y;
                break;
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int width = event.window.data1;
                    int height = event.window.data2;
                    GPU_SetWindowResolution(width, height);

                    // Workaround: Re-read the width and height for scaling.
                    // On high-dpi mode, the width and height reported in the event are not the real ones.
                    // See: https://github.com/grimfang4/sdl-gpu/issues/188
                    SDL_GL_GetDrawableSize(window, &width, &height);

                    const float scaleW = width/float(Window::GAME_WIDTH);
                    const float scaleH = height/float(Window::GAME_HEIGHT);
                    const float aspect = Window::GAME_WIDTH/float(Window::GAME_HEIGHT);
                    GPU_Rect rect;
                    if (scaleW < scaleH) {
                        rect.w = width;
                        rect.h = width/aspect;
                        rect.x = 0;
                        rect.y = (height - rect.h)/2;
                    } else {
                        rect.w = height*aspect;
                        rect.h = height;
                        rect.x = (width - rect.w)/2;
                        rect.y = 0;
                    }
                    GPU_SetViewport(Window::target,rect);
                    GPU_SetVirtualResolution(Window::target, Window::GAME_WIDTH, Window::GAME_HEIGHT);
                }
                break;
            }
            //ImGui_ImplSDL2_ProcessEvent(&event);
        }
    }

    
    namespace DrawPrimitive {

        void Pixel(float x, float y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            GPU_Pixel(Window::target, x, y, { r,g,b,a });
        }

        void Rectangle(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            if (thickness < 0)
            {
                GPU_SetLineThickness(0);
                GPU_RectangleFilled(Window::target, x1, y1, x2, y2, {r,g,b,a});
            }
            else
            {
                GPU_SetLineThickness(thickness);
                GPU_Rectangle(Window::target, x1, y1, x2, y2, { r,g,b,a });
            }
        }

        void Line(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            GPU_SetLineThickness(thickness);
            GPU_Line(Window::target, x1, y1, x2, y2, { r,g,b,a });
        }

        void Circle(float x, float y, int radius, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            GPU_SetLineThickness(thickness);
            if (thickness < 0) {
                GPU_SetLineThickness(0);
                GPU_CircleFilled(Window::target, x, y, radius, {r,g,b,a});
            }
            else {
                GPU_SetLineThickness(thickness);
                GPU_Circle(Window::target, x, y, radius, {r,g,b,a});
            }
        }
    }


    namespace DrawRaw {
        unsigned short vertex_count = 0;
        unsigned int index_count = 0;
        float vertices[MAX_VERTICES * MAX_COMPONENTS_PER_VERTEX];
        unsigned short indices[MAX_INDICES];
    }
}
