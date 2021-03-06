#include "healthup.h"

#include "mates.h"
#include "assets.h"
#include "window.h"
#include "jumpman.h"
#include "collide.h"
#include "gaemtilemap.h"
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

	pos.DebugDraw();
	
	if (GaemTileMap::instance()->GetTile(Tile::ToTiles(pos)).isBreakable(Tile::BreakResistance::ANY)) {
		//Don't draw behind breakables
		return;
	}

	float y = 1.5f * -sin(mainClock*2);

	Window::Draw(Assets::spritesheetTexture, pos.x, pos.y - y)
		.withOrigin(8, 8)
		.withRect(6 * 16, 11 * 16, 16, 16);

	// Debug-only
	Bounds().DebugDraw();
}
