#pragma once

#include <iosfwd>
#include <utility>

namespace Debug {
#ifdef _DEBUG
    extern bool Draw;
#else 
    static const bool Draw = false;
#endif
    extern std::ostream& _forwarded_out;
};

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
        Debug::_forwarded_out << std::forward<T>(val);
        return DebugStreamDelegate();
    }
};

namespace Debug {
    extern DebugStream out;
};
