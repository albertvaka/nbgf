#include "window.h"

#include <raylib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef _IMGUI
#include "rlImGui.h"
#endif

#include "debug.h"
#include "camera.h"
#include "rlImGui.h"	// include the API header

namespace Window
{
    RenderTexture2D windowRenderTexture;

    int Init() {
        SetTraceLogLevel(TraceLogLevel::LOG_WARNING);

        SetConfigFlags(FLAG_WINDOW_HIDDEN | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);

        // HACK: Raylib doesn't support getting the monitor size until a window is created
        InitWindow(200, 200, NULL);
#ifdef __EMSCRIPTEN__
        int scale = 1;
#else
        int scale = (int)GetScaleFactor();
        if (scale <= 0) {
            Debug::out << "Warning: Game resolution (" << GAME_WIDTH << "*" << GAME_HEIGHT << ") is larger than the window resolution";
            scale = 1;
        }
        Debug::out << "Scaling to x" << scale;
 #endif
        CloseWindow();

        InitWindow(GAME_WIDTH*scale, GAME_HEIGHT*scale, WINDOW_TITLE);

        windowRenderTexture = LoadRenderTexture(GAME_WIDTH, GAME_HEIGHT);
        SetTextureFilter(windowRenderTexture.texture, TEXTURE_FILTER_BILINEAR);

        SetExitKey(KeyboardKey::KEY_NULL);

        SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

        ClearWindowState(FLAG_WINDOW_HIDDEN);

        return 0;
    }

    void SetFullScreen(bool b) {
/*
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
        }
#endif
*/
        if (IsWindowFullscreen() != b) {
            ToggleFullscreen();
        }
    }
    /*
    void ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int width = event.window.data1;
                    int height = event.window.data2;
                    GPU_SetWindowResolution(width, height);

                    // Workaround: Re-read the width and height for scaling instead of using the data fields in the event.
                    // On high-dpi mode, the width and height reported in the event are not the real ones.
                    // See: https://github.com/grimfang4/sdl-gpu/issues/188
                    SDL_GL_GetDrawableSize(window, &width, &height);

                    const float scaleW = width/float(Window::GAME_WIDTH);
                    const float scaleH = height/float(Window::GAME_HEIGHT);
                    const float aspect = Window::GAME_WIDTH/float(Window::GAME_HEIGHT);
                    Rectangle rect;
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
    */

    namespace DrawPrimitive {

        void Point(float x, float y, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            DrawPixel(x, y, { r,g,b,a });
        }

        void Rectangle(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            if (thickness < 0)
            {
                DrawRectangle(x1, y1, x2-x1, y2-y1, {r,g,b,a});
            }
            else
            {
                DrawRectangleLinesEx({ x1, y1, x2-x1, y2-y1 }, thickness, { r,g,b,a });
            }
        }

        void Line(float x1, float y1, float x2, float y2, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            DrawLineEx({ x1, y1 }, {x2, y2 }, thickness, { r,g,b,a });
        }

        void Circle(float x, float y, float radius, float thickness, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            if (thickness < 0) {
                DrawCircle(x, y, radius, { r,g,b,a });
            }
            else {
                // Raylib DrawCircleLines doesn't support thickness (draws pixel by pixel)
                DrawRing({ x, y }, radius - thickness / 2.f, radius + thickness / 2.f, 0.f, 360.f, 64, { r,g,b,a });
            }
        }

    }

    /*
    namespace DrawRaw {
        unsigned short vertex_count = 0;
        unsigned int index_count = 0;
        float vertices[MAX_VERTICES * MAX_COMPONENTS_PER_VERTEX];
        unsigned short indices[MAX_INDICES];
    }
    */
}
