#include "window.h"

#include "SDL_gpu.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef _IMGUI
#include "imgui_impl_sdl.h"
#endif

#include "debug.h"
#include "raw_input.h"
#include "camera.h"

namespace Window
{
    SDL_Window* window;
    GPU_Target* screenTarget;
    GPU_Target* currentDrawTarget;

    int Init() {
        GPU_SetDebugLevel(GPU_DEBUG_LEVEL_1);

#ifdef __EMSCRIPTEN__
        int scale = 1;
#else
        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);
        dm.h -= 64; // Account for some pixels used by the window decorations
        int scale = std::min(dm.w / GAME_WIDTH, dm.h / GAME_HEIGHT);
        Debug::out << "Scaling to x" << scale;
        //Debug::out << dm.w << " " << dm.h;
 #endif
        auto sdl_create_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
        screenTarget = GPU_Init(GAME_WIDTH * scale, GAME_HEIGHT * scale, sdl_create_window_flags);
        if (screenTarget == NULL) {
            Debug::out << "GPU_Init failed";
            return 1;
        }
        window = SDL_GetWindowFromID(screenTarget->context->windowID);
        SDL_SetWindowTitle(window, Window::WINDOW_TITLE);

        // SDL-gpu anchors images at the center by default, change it to the top-left corner
        GPU_SetDefaultAnchor(0.f, 0.f);

        // FIXME: Too late for this game, but we have the option to set the Y coordinates the right way
        //GPU_SetCoordinateMode(false);

        currentDrawTarget = screenTarget;

        GPU_EnableCamera(screenTarget, true);
        Camera::camera = GPU_GetDefaultCamera();
        Camera::gui_camera = GPU_GetDefaultCamera();
        Camera::gui_camera.use_centered_origin = false;
        Camera::SetTopLeft(0.f, 0.f);

        GPU_SetVirtualResolution(Window::screenTarget, Window::GAME_WIDTH, Window::GAME_HEIGHT);

        return 0;
    }

    void SetFullScreen(bool b) {
#ifdef __EMSCRIPTEN__
        EM_ASM({
            if ($0) {
                document.documentElement.requestFullscreen();
            } else {
                document.exitFullscreen();
            }
        },b);
        if (!b) {
            // Here to trigger a SDL_WINDOWEVENT
            SDL_SetWindowFullscreen(Window::window, 0);
        }
#else
        SDL_SetWindowFullscreen(Window::window, b ? SDL_WINDOW_FULLSCREEN : 0);
#endif
    }

    void ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT_FOCUS_LOST:
                Debug::out << "Lost focus";
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                Debug::out << "Gained focus";
                break;
            case SDL_CONTROLLERDEVICEADDED:
                GamePad::_Added(SDL_GameControllerOpen(event.jdevice.which));
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                GamePad::_Removed(SDL_GameControllerFromInstanceID(event.jdevice.which));
                break;
            case SDL_MOUSEWHEEL: {
                float wheel = event.wheel.y;
#ifdef __APPLE__
                if (Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT) || Keyboard::IsKeyPressed(SDL_SCANCODE_RSHIFT)) {
                    wheel = event.wheel.x; //shift makes the axis change on osx for some reason
                }
#endif
                Mouse::scrollWheel += wheel;
            }
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
                    GPU_SetViewport(Window::screenTarget,rect);
                    GPU_SetVirtualResolution(Window::screenTarget, Window::GAME_WIDTH, Window::GAME_HEIGHT);
                }
                break;
            }
#ifdef _IMGUI
            ImGui_ImplSDL2_ProcessEvent(&event);
#endif
        }
    }

	void BeginRenderToTexture(GPU_Image* targetTexture, bool useCamera) {
		GPU_Target* target = GPU_GetTarget(targetTexture);
		Window::currentDrawTarget = target;
		GPU_EnableCamera(target, useCamera);
		if (useCamera) {
			GPU_SetCamera(target, &Camera::camera);
		}
	}

    namespace DrawPrimitive {

        void Point(float x, float y, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            GPU_SetLineThickness(0);
            float d = thickness / 2;
            GPU_RectangleFilled(Window::currentDrawTarget, x - d, y - d, x + d, y + d, { r,g,b,a });
        }

        void Rectangle(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            if (thickness < 0)
            {
                GPU_SetLineThickness(0);
                GPU_RectangleFilled(Window::currentDrawTarget, x1, y1, x2, y2, {r,g,b,a});
            }
            else
            {
                GPU_SetLineThickness(thickness);
                GPU_Rectangle(Window::currentDrawTarget, x1, y1, x2, y2, { r,g,b,a });
            }
        }

        void Line(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            GPU_SetLineThickness(thickness);
            GPU_Line(Window::currentDrawTarget, x1, y1, x2, y2, { r,g,b,a });
        }

        void Circle(float x, float y, float radius, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            if (thickness < 0) {
                GPU_SetLineThickness(0);
                GPU_CircleFilled(Window::currentDrawTarget, x, y, radius, {r,g,b,a});
            }
            else {
                GPU_SetLineThickness(thickness);
                GPU_Circle(Window::currentDrawTarget, x, y, radius, {r,g,b,a});
            }
        }

        void Arc(float x, float y, float radius, float start_angle, float end_angle, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            if (thickness < 0) {
                GPU_SetLineThickness(0);
                GPU_ArcFilled(Window::currentDrawTarget, x, y, radius, start_angle, end_angle, { r,g,b,a });
            }
            else {
                GPU_SetLineThickness(thickness);
                GPU_Arc(Window::currentDrawTarget, x, y, radius, start_angle, end_angle, { r,g,b,a });
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
