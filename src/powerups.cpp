#include "powerups.h"

#include "tilemap.h"
#include "mates.h"
#include "assets.h"
#include "window.h"

extern float mainClock;


void GunUp::Draw() const
{
	float y = 3.f * sin(mainClock);

	Window::Draw(Assets::marioTexture, pos.x, pos.y - y)
		.withOrigin(8, 8)
		.withRect(5 * 16, 11 * 16, 16, 16);

}


void HealthUp::Draw() const
{
	if (TileMap::instance()->getTile(TileMap::toTiles(pos)).isBreakable()) {
		//Don't draw behind breakables
		return;
	}

	float y = 3.f * sin(mainClock);

	Window::Draw(Assets::marioTexture, pos.x, pos.y - y)
		.withOrigin(8, 8)
		.withRect(6 * 16, 11 * 16, 16, 16);
}


void BigItem::Draw() const
{
	float y = 3.f * sin(mainClock);

	switch(skill) {
	case Skill::GUN:
		Window::Draw(Assets::marioTexture, pos.x, pos.y - y)
			.withOrigin(16, 8)
			.withRect(0 * 16, 3 * 16, 2 * 16, 16);

		break;
	case Skill::WALLJUMP:
		Window::Draw(Assets::marioTexture, pos.x, pos.y - y)
			.withOrigin(16, 8)
			.withRect(4 * 16, 8 * 16, 2 * 16, 16);
		break;
	}


}


