#include "window.h"

#include "SDL_gpu.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#ifdef _IMGUI
#include "imgui_impl_sdl2.h"
#endif

#include "debug.h"
#include "raw_input.h"
#include "input.h"
#include "camera.h"

namespace Window
{
    SDL_Window* window;
    GPU_Target* screenTarget;
    GPU_Target* currentDrawTarget;

    vec GetUsableScreenSize() {
#ifdef __EMSCRIPTEN__
        int width, height;
        const char* canvas_id = "#canvas"; // Default ID
        emscripten_get_canvas_element_size(canvas_id, &width, &height);
        return vec(width, height);
#else
        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);
        dm.h -= 64; // Account for some pixels used by the window decorations (FIXME: this isn't very precise and not true in fullscreen mode)
        return vec(dm.w, dm.h);
#endif
    }


    int Init() {
        GPU_SetDebugLevel(GPU_DEBUG_LEVEL_1);

        vec screenSize = GetUsableScreenSize();
#ifdef GROW_TO_FILL_SCREEN
        float scale = screenSize.y / float(GAME_HEIGHT);
        GAME_WIDTH = screenSize.x / scale;
        Debug::out << "Scaling to x" << scale;
#else
        int scale = std::min(screenSize.x / GAME_WIDTH, screenSize.y / GAME_HEIGHT);
        if (scale <= 0) {
            Debug::out << "Warning: Game resolution (" << GAME_WIDTH << "*" << GAME_HEIGHT << ") is larger than the window resolution (" << screenSize.x << "*" << screenSize.y << ")";
            scale = 1;
        }
        Debug::out << "Scaling to x" << scale;
#endif
        GPU_SetPreInitFlags(GPU_INIT_DISABLE_AUTO_VIRTUAL_RESOLUTION);

        auto sdl_create_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN;
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

        SDL_ShowWindow(window);

        // Start with both buffers fully black
        Clear(0, 0, 0);
        GPU_Flip(screenTarget);
        Clear(0, 0, 0);

        // Disable drag&drop events
        SDL_EventState(SDL_DROPFILE,  SDL_DISABLE);
        SDL_EventState(SDL_DROPTEXT, SDL_DISABLE);

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
        SDL_SetWindowFullscreen(Window::window, b ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
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
                Input::PreferredUserInputDevice = Input::InputDevice::GamePad;
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
                Input::PreferredUserInputDevice = event.wheel.which == SDL_TOUCH_MOUSEID ? Input::InputDevice::Touch : Input::InputDevice::Keyboard;
            }
            break;
            case SDL_FINGERDOWN:
            case SDL_FINGERMOTION: {
                Input::PreferredUserInputDevice = Input::InputDevice::Touch;
            }
            break;
            case SDL_KEYDOWN: {
                Input::PreferredUserInputDevice = Input::InputDevice::Keyboard;
            }
            break;
            case SDL_MOUSEMOTION: {
                Input::PreferredUserInputDevice = event.motion.which == SDL_TOUCH_MOUSEID ? Input::InputDevice::Touch : Input::InputDevice::Keyboard;
            }
            break;
            case SDL_MOUSEBUTTONDOWN: {
                // SDL emulates clicks for touch events (unless disabled),
                // so we need to check it's not an emulated click.
                Input::PreferredUserInputDevice = event.button.which == SDL_TOUCH_MOUSEID ? Input::InputDevice::Touch : Input::InputDevice::Keyboard;
            }
            break;
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYAXISMOTION: {
                Input::PreferredUserInputDevice = Input::InputDevice::GamePad;
            }
            break;
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int width = event.window.data1;
                    int height = event.window.data2;
                    Debug::out << "Window size changed to " << width << " " << height;

                    GPU_SetWindowResolution(width, height);

                    // Workaround: Re-read the width and height for scaling instead of using the data fields in the event.
                    // On high-dpi mode, the width and height reported in the event are not the real ones.
                    // See: https://github.com/grimfang4/sdl-gpu/issues/188
                    SDL_GL_GetDrawableSize(window, &width, &height);

#ifdef GROW_TO_FILL_SCREEN
                    // Adjust GAME_WIDTH to maintain aspect ratio with the window
                    GAME_WIDTH = width / GetViewportScale();

                    // Set viewport to fill the entire window
                    GPU_Rect rect;
                    rect.x = 0;
                    rect.y = 0;
                    rect.w = width;
                    rect.h = height;
#else
                    const float aspect = Window::GAME_WIDTH/float(Window::GAME_HEIGHT);
                    const float scaleW = width/float(Window::GAME_WIDTH);
                    const float scaleH = height/float(Window::GAME_HEIGHT);
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
#endif
                    GPU_SetViewport(Window::screenTarget, rect);
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
