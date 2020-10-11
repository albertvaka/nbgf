#include "parallax.h"

#include "camera.h"

#include "debug.h"
#include "mates.h"
#include "imgui.h"
#include "collide.h"

void Parallax::Draw() const
{
	Bounds camera = Camera::GetBounds();
	if (!Collide(bounds, camera)) {
		return;
	}

	// depths go between 0 (moves with camera) and 1 (doesn't move)
	// We increment at the end of the loop, so the last value after adding depthIncrement won't be used.
	// For this reason we divide between (layers_count-1), so we get to depth = depth_max (eg: 1)
	float depth = depth_min;
	float depthIncrement = (depth_max-depth_min) / (layers_count-1);
	for (int i = 0; i < layers_count; i++) {
		GPU_Image* texture = layers[i];
		float scale = bounds.height / texture->h; // Stretch so it covers the full height on the Y axis, on the X axis we will tile the texture.
		float moveWithCameraOffset = 0;
		float doNotMoveOffset = (bounds.Center().x - camera.Center().x );
		float offset_layer = Mates::Lerp(doNotMoveOffset, moveWithCameraOffset, depth) / scale;
		float final_offset = base_offset + offset_layer;
		Window::Draw(texture, bounds.TopLeft())
			.withScale(scale, scale)
			.withRect(final_offset, 0, bounds.width / scale, bounds.height / scale); //Rect could be larger than the texture itself, we assume the texture is set on repeat

		depth += depthIncrement;
	}

}


void Parallax::DrawImGUI(const char* title)
{
	ImGui::Begin(title);
	ImGui::SliderFloat("Depth min:", &depth_min, 0, 1);
	ImGui::SliderFloat("Depth max:", &depth_max, 0, 1);
	ImGui::SliderFloat("Base offset:", &base_offset, -500, 500);
	ImGui::End();
}