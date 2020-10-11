#include "parallax.h"

#include "camera.h"
#include "assets.h"

#include "debug.h"
#include "mates.h"
//#include "imgui.h"
#include "collide.h"

// I like how it looks at this offset together with the rest of the map
const float base_offset = 183;

void Parallax::Draw() const
{
	Bounds camera = Camera::GetBounds();
	if (!Collide(bounds, camera)) {
		return;
	}

	//ImGui::Begin("asdasd");
	//ImGui::SliderFloat("Mouse: %d", &offseterino, 0, 500);
	//ImGui::End();

	//TODO: Make initial depth (and depthIncrement?) configurable
	float depth = 0.f; //between 0 (moves with camera) and 1 (doesn't move)
	// We increment at the end of the loop, so the last value after adding depthIncrement won't be used.
	// For this reason we do size-1, so we get to depth = 1
	float depthIncrement = 1.f / (layers_count-1);
	for (int i = 0; i < layers_count; i++) {
		GPU_Image* texture = Assets::caveParallaxTextures[i]; //FIXME: layers[i] blows up
		float scale = bounds.height / texture->h; // Stretch so it covers the full height on the Y axis, on the X axis we will tile the texture.
		float moveWithCameraOffset = 0;
		float doNotMoveOffset = (bounds.Left() - camera.Center().x );
		float offset = Mates::Lerp(doNotMoveOffset, moveWithCameraOffset, depth) / scale;

		Window::Draw(texture, bounds.TopLeft())
			.withScale(scale, scale)
			.withRect(base_offset + offset, 0, bounds.width / scale, bounds.height / scale); //Rect could be larger than the texture itself, we assume the texture is set on repeat

		depth += depthIncrement;
	}

}