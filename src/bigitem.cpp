#include "bigitem.h"

#include "mates.h"
#include "assets.h"
#include "window_draw.h"
#include "collide.h"
#include "tile.h"
#include "particles.h"
#include "rand.h"

extern float mainClock;

bool BigItem::HasPedestal() const
{
	return skill != Skill::BREAK;
}

void BigItem::Draw() const
{
	float y = 1.8f * sin(mainClock*2);

	switch(skill) {
	case Skill::BREAK:
		Window::Draw(Assets::spritesheetTexture, pos.x, pos.y - y)
			.withOrigin(16, 8)
			.withRect(8 * 16, 11 * 16, 16, 16);
		break;
	case Skill::GUN:
		Window::Draw(Assets::spritesheetTexture, pos.x, pos.y - y)
			.withOrigin(16, 8)
			.withRect(0 * 16, 3 * 16, 2 * 16, 16);
		break;
	case Skill::ATTACK:
	case Skill::DIVE:
	case Skill::WALLJUMP:
	case Skill::DASH:
	default:
		Window::Draw(Assets::spritesheetTexture, pos.x, pos.y - y)
			.withOrigin(16, 8)
			.withRect(4 * 16, 8 * 16, 2 * 16, 16);
		break;
	}
}

void BigItem::Update(float dt)
{
	if (!HasPedestal()) {
		return;
	}
	vec tilePos = Tile::AlignToTiles(pos);
	Particles::itemSparks.pos.y = tilePos.y + 16;
	Particles::itemSparks.pos.x = Rand::roll(tilePos.x - 13, tilePos.x + 13);
	Particles::itemSparks.SpawnWithExternalTimer(particleTimer, dt);
}

void BigItem::DrawPedestal() const
{
	if (!HasPedestal()) {
		return;
	}

	vec tilePos = Tile::AlignToTiles(pos);
	Window::Draw(Assets::spritesheetTexture, tilePos + vec(-16, 16))
		.withRect(5 * 16, 9 * 16, 16, 16);
	Window::Draw(Assets::spritesheetTexture, tilePos + vec(0, 16))
		.withRect(5 * 16, 9 * 16, 16, 16);
	float alpha = abs(sin(mainClock));
	Window::Draw(Assets::spritesheetTexture, tilePos + vec(-16, 16))
		.withRect(6 * 16, 9 * 16, 16, 16)
		.withColor(255, 255, 255, alpha * 255);
	Window::Draw(Assets::spritesheetTexture, tilePos + vec(0, 16))
		.withRect(6 * 16, 9 * 16, 16, 16)
		.withColor(255, 255, 255, alpha * 255);
}
