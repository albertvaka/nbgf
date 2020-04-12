#include "parallax.h"

#include "input.h"
#include "assets.h"

#include "debug.h"
#include "imgui.h"

// I like how it looks at this offset together with the rest of the map
const float offseterino = 183;

void Parallax::Draw(sf::RenderTarget& window) const
{
	auto camera = Camera::GetCameraBounds();
	if (!bounds.intersects(camera)) {
		return;
	}

	//ImGui::Begin("asdasd");
	//ImGui::SliderFloat("Mouse: %d", &offseterino, 0, 500);
	//ImGui::End();

	const auto& textures = Assets::forestParallaxTextures;

	static sf::Sprite spr;

	float depth = 0.f; //between 0 (moves with camera) and 1 (doesn't move)
	// We increment at the end of the loop, so the last value after adding depthIncrement won't be used.
	// For this reason we will never get to depth = 1 (unless we did size-1 here), but I'm ok with having everything move at least a bit.
	float depthIncrement = 1.f / (textures.size()-1);
	for (const sf::Texture& texture : textures) {

		float scale = bounds.height / texture.getSize().y; // Stretch so it covers the full height on the Y axis, on the X axis we will tile the texture.

		float moveWithCameraOffset = 0;
		float doNotMoveOffset = (bounds.Left() - camera.Center().x );
		float offset = Mates::Lerp(doNotMoveOffset, moveWithCameraOffset, depth) / scale;
		int intoffset = floor(offset);

		spr.setTexture(texture);
		spr.setScale(scale, scale);
		spr.setTextureRect(sf::IntRect(offseterino + intoffset, 0, bounds.width / scale, bounds.height / scale)); //Rect could be larger than the texture itself, we assume the texture is set on repeat
		spr.setPosition(bounds.TopLeft() - vec(offset - intoffset, 0)); //superhack because texture coordinates are ints

		window.draw(spr);

		depth += depthIncrement;
	}

}