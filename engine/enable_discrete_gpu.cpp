
#ifdef _WIN32

// Change this to toggle using the discrete GPU on Windows
static const unsigned long UseDiscreteGpu = 0;

extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = UseDiscreteGpu;
	__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = UseDiscreteGpu;
}
#endif
