#include "healthup.h"

#include "mates.h"
#include "assets.h"
#include "window.h"
#include "jumpman.h"
#include "collide.h"
#include "gaemtilemap.h"
#include "rand.h"

extern float mainClock;

HealthUp::HealthUp(int id, vec p)
	: id(id)
	, pickedUp(false)
	, BoxEntity(Tile::AlignToTiles(p) + vec(Tile::Size/2, Tile::Size/2), vec(Tile::Size-1.f, Tile::Size -1.f))
{
}

bool HealthUp::IsBehindBreakable() const
{
	return (GaemTileMap::instance()->GetTile(Tile::ToTiles(pos)).isBreakable(Tile::BreakResistance::ANY));
}

void HealthUp::Update(float dt)
{
	if (IsBehindBreakable()) {
		return;
	}

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

	// Debug-only
	Bounds().DebugDraw();
	
	if (IsBehindBreakable()) {
		return;
	}

	float y = 1.5f * -sin(mainClock*2);

	Window::Draw(Assets::spritesheetTexture, pos.x, pos.y - y)
		.withOrigin(8, 8)
		.withRect(6 * 16, 11 * 16, 16, 16);

}
