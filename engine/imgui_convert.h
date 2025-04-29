#pragma once

#include "imgui.h"
#include "SDL_gpu.h"
#include "vec.h"

ImTextureID im(GPU_Image* image) {
	return (ImTextureID)(GPU_GetTextureHandle(image));
}
ImVec2 im(vec v) {
	return ImVec2(v.x, v.y);
}
ImVec4 im(SDL_Color c) {
	return ImVec4(c.r, c.g, c.b, c.a);
}
