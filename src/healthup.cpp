#include "healthup.h"

#include "mates.h"
#include "assets.h"
#include "window.h"
#include "jumpman.h"
#include "collide.h"
#include "tilemap.h"
#include "rand.h"

extern float mainClock;

void HealthUp::Update(float dt) {
	JumpMan* player = JumpMan::instance();
	if (!pickedUp && Collide(bounds(), player->bounds())) {

		//TODO: Show popup or animation or something

		player->health++;
		player->maxHealth++;

		pickedUp = true;
	}
}

void HealthUp::Draw() const
{
	if (pickedUp) {
		return;
	}

	if (TileMap::instance()->getTile(TileMap::toTiles(pos)).isBreakable()) {
		//Don't draw behind breakables
		return;
	}

	float y = 1.5f * -sin(mainClock*2);

	Window::Draw(Assets::marioTexture, pos.x, pos.y - y)
		.withOrigin(8, 8)
		.withRect(6 * 16, 11 * 16, 16, 16);

	if (Debug::Draw) {
		bounds().Draw();
	}
}
