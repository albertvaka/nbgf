#include "healthup.h"

#include "mates.h"
#include "assets.h"
#include "window_draw.h"
#include "player.h"
#include "player.h"
#include "collide.h"
#include "gaemtilemap.h"
#include "rand.h"

extern float mainClock;

namespace AnimLib {
	constexpr GPU_Rect ROCK_SHARD = { 29 * 16, 13 * 16, 16, 16 };
	constexpr GPU_Rect HEALTH_UP = { 6 * 16, 11 * 16, 16, 16 };
}

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

	Player* player = Player::instance();
	if (!pickedUp && Collide(Bounds(), player->CollisionBounds())) {
		player->HealthUp(5);
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
		.withRect(AnimLib::HEALTH_UP);

}


Shard::Shard(int id, vec p)
	: id(id)
	, pickedUp(false)
	, BoxEntity(Tile::AlignToTiles(p) + vec(Tile::Size / 2, Tile::Size / 2), vec(Tile::Size - 1.f, Tile::Size - 1.f))
{
}

bool Shard::IsBehindBreakable() const
{
	return (GaemTileMap::instance()->GetTile(Tile::ToTiles(pos)).isBreakable(Tile::BreakResistance::ANY));
}

void Shard::Update(float dt)
{
	if (IsBehindBreakable()) {
		return;
	}

	Player* player = Player::instance();
	if (!pickedUp && Collide(Bounds(), player->CollisionBounds())) {

		//TODO: Show popup or animation or something
		//TODO: Add to some kind of inventory or something

		pickedUp = true;
	}
}

void Shard::Draw() const
{
	if (pickedUp) {
		return;
	}

	// Debug-only
	Bounds().DebugDraw();

	if (IsBehindBreakable()) {
		return;
	}

	float y = 2.f * -sin(mainClock * 3);

	Window::Draw(Assets::spritesheetTexture, pos.x, pos.y - y)
		.withRectWithOriginCentered(AnimLib::ROCK_SHARD);

}
