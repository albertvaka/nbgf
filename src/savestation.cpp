#include "savestation.h"

#include "tilemap.h"
#include "mates.h"
#include "assets.h"
#include "camera.h"
#include "collider.h"

extern float mainClock;

SaveStation::SaveStation(const vec& p) : BoxEntity(p, vec(32, 32)) { }

void SaveStation::Update(float dt) {
	bool prevInScene = inScene;
	inScene = Collide(Camera::GetBounds(),bounds());
	if (inScene && !prevInScene) {
		enabled = true;
	}
}

void SaveStation::Activate() {
	if (enabled) {
		enabled = false;
	}
}

void SaveStation::Draw() const
{
	Window::Draw(Assets::marioTexture, pos)
		.withRect(4 * 16, 12 * 16, 32, 32)
		.withOrigin(16, 16);

	if (enabled) {
		Window::Draw(Assets::marioTexture, pos)
			.withRect(6 * 16, 12 * 16, 32, 32)
			.withOrigin(16, 16)
			.withColor(255, 255, 255, 128 + 128 * sin(mainClock*2));
	}
}

