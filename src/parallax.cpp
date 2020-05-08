#include "parallax.h"

#include "input.h"
#include "assets.h"

#include "debug.h"
//#include "imgui.h"
#include "collider.h"

// I like how it looks at this offset together with the rest of the map
const float base_offset = 183;

void Parallax::Draw() const
{
	auto camera = Camera::GetBounds();
	if (!Collide(bounds, camera)) {
		return;
	}

	//ImGui::Begin("asdasd");
	//ImGui::SliderFloat("Mouse: %d", &offseterino, 0, 500);
	//ImGui::End();

	const auto& textures = Assets::forestParallaxTextures;

	float depth = 0.f; //between 0 (moves with camera) and 1 (doesn't move)
	// We increment at the end of the loop, so the last value after adding depthIncrement won't be used.
	// For this reason we will never get to depth = 1 (unless we did size-1 here), but I'm ok with having everything move at least a bit.
	float depthIncrement = 1.f / (textures.size()-1);
	for (auto* texture : textures) {

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