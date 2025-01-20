#pragma once

#include <iosfwd>
#include <ostream>
#include <raylib.h>
#include <functional>

namespace Debug {
#ifdef _DEBUG
    inline bool Draw = false;
    inline bool FrameByFrame = false;
    inline bool FastForward = false;
    inline bool CameraFixed = false;
#else 
    static const bool Draw = false;
    static const bool FrameByFrame = false;
    static const bool FastForward = false;
    static const bool CameraFixed = false;
#endif
    extern std::ostream& _forwarded_out;
    extern void timed(std::function<void()> f);
};

// Delegate from: https://stackoverflow.com/questions/49332013/adding-a-new-line-after-stdostream-output-without-explicitly-calling-it
struct DebugStreamDelegate
{
    DebugStreamDelegate() = default;
    ~DebugStreamDelegate();
    template <typename T>
    DebugStreamDelegate& operator<<(T&& val) noexcept {
        Debug::_forwarded_out << std::forward<T>(val);
        return *this;
    }
    DebugStreamDelegate& operator=(const DebugStreamDelegate&) = delete;
    DebugStreamDelegate(const DebugStreamDelegate&) = delete;
};
struct DebugStream {
    template <typename T>
    DebugStreamDelegate operator<<(T&& val) noexcept {
        Debug::_forwarded_out << GetTime() << ": " << std::forward<T>(val);
        return DebugStreamDelegate();
    }
};

namespace Debug {
    extern DebugStream out;
};

#define COLOR_UINT8_RGB_RED 255,0,0
#define COLOR_UINT8_RGB_GREEN 0,255,0
#define COLOR_UINT8_RGB_BLUE 0,0,255
#define COLOR_UINT8_RGB_CYAN 0,255,255
#define COLOR_UINT8_RGB_MAGENTA 255,0,255
#define COLOR_UINT8_RGB_YELLOW 255,255,0

