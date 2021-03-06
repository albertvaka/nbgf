#pragma once

#include <iosfwd>
#include <ostream>

namespace Debug {
#ifdef _DEBUG
    inline bool Draw = false;
    inline bool FrameByFrame = false;
    inline bool FastForward = false;
#else 
    static const bool Draw = false;
    static const bool FrameByFrame = false;
#endif
    extern std::ostream& _forwarded_out;
};

extern int lastTicks;

// Delegate from: https://stackoverflow.com/questions/49332013/adding-a-new-line-after-stdostream-output-without-explicitly-calling-it
struct DebugStreamDelegate
{
    DebugStreamDelegate() = default;
    ~DebugStreamDelegate();
    template <typename T>
    DebugStreamDelegate& operator<<(T&& val) {
        Debug::_forwarded_out << std::forward<T>(val);
        return *this;
    }
    DebugStreamDelegate& operator=(const DebugStreamDelegate&) = delete;
    DebugStreamDelegate(const DebugStreamDelegate&) = delete;
};
struct DebugStream {
    template <typename T>
    DebugStreamDelegate operator<<(T&& val) {
        Debug::_forwarded_out << lastTicks << ": " << std::forward<T>(val);
        return DebugStreamDelegate();
    }
};

namespace Debug {
    extern DebugStream out;
};
