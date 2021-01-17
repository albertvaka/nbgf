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
	if (!pickedUp && Collide(Bounds(), player->Bounds())) {

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

	if (TileMap::instance()->GetTile(TileMap::ToTiles(pos)).isBreakable()) {
		//Don't draw behind breakables
		return;
	}

	float y = 1.5f * -sin(mainClock*2);

	Window::Draw(Assets::marioTexture, pos.x, pos.y - y)
		.withOrigin(8, 8)
		.withRect(6 * 16, 11 * 16, 16, 16);

	// Debug-only
	Bounds().DebugDraw();
}
