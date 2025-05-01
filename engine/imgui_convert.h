#pragma once

#include "imgui.h"
#include "SDL_gpu.h"
#include "vec.h"
#include "window.h"

ImTextureID im(GPU_Image* image) {
	return (ImTextureID)(GPU_GetTextureHandle(image));
}
ImVec2 im(vec v) {
	return ImVec2(v.x, v.y);
}
ImVec4 im(SDL_Color c) {
	return ImVec4(c.r, c.g, c.b, c.a);
}
namespace ImGui {
	// Deprecated, call within Begin
	void SetWindowPosInGameCoords(vec pos, vec unscaledOffset = vec::Zero, ImGuiCond cond = 0) {
		ImGui::SetWindowPos(im(Window::GetViewportMarginsScaled()/ImGui::GetIO().DisplayFramebufferScale.x + pos * Window::GetViewportScale()/ImGui::GetIO().DisplayFramebufferScale.x + unscaledOffset), cond);
	}

	// Call before Begin
	void SetNextWindowPosInGameCoords(vec pos, vec unscaledOffset = vec::Zero, ImGuiCond cond = 0) {
		ImGui::SetNextWindowPos(im(Window::GetViewportMarginsScaled()/ImGui::GetIO().DisplayFramebufferScale.x + pos * Window::GetViewportScale()/ImGui::GetIO().DisplayFramebufferScale.x + unscaledOffset), cond);
	}
}
