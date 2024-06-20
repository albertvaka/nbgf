
#if _WIN32
#pragma comment(lib, "Shcore.lib")
#include <ShellScalingApi.h>
#endif

void EnableHighDPI() {
#if _WIN32
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif
}