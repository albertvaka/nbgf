#include "powerups.h"

#include "tilemap.h"
#include "mates.h"
#include "assets.h"
#include "window.h"
#include "tilemap.h"
#include "assets.h"

extern float mainClock;


void GunUp::Draw() const
{
	float y = 1.5f * sin(mainClock*2);

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

	float y = 1.5f * -sin(mainClock*2);

	Window::Draw(Assets::marioTexture, pos.x, pos.y - y)
		.withOrigin(8, 8)
		.withRect(6 * 16, 11 * 16, 16, 16);
}


BigItem::BigItem(const vec& p, Skill s)
	: BoxEntity(p + vec(8, -8), vec(32, 16))
	, skill(s)
	, particles(Assets::marioTexture)
{
	particles.AddSprite({ 5, 37, 6, 6 });
	particles.max_vel = vec(0, -10);
	particles.min_vel = vec(0, -30);
	particles.min_ttl = 1.f;
	particles.max_ttl = 1.f;
	particles.min_interval = 0.2f;
	particles.max_interval = 0.3f;
	particles.min_scale = 0.4f;
	particles.max_scale = 0.4f;
	particles.alpha = 0.7f;
	particles.alpha_vel = -0.8f;
}

void BigItem::Draw()
{
	float y = 1.8f * sin(mainClock*2);

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

	auto tilePos = TileMap::alignToTiles(pos);

	particles.pos.y = tilePos.y + 32;
	particles.pos.x = Random::roll(tilePos.x - 13, tilePos.x + 13);
	particles.Spawn(1 / 60.f);
	particles.UpdateParticles(1 / 60.f);
	particles.Draw();

	//particles.DrawImGUI();

	Window::Draw(Assets::marioTexture, tilePos + vec(-16, 32))
		.withRect(5 * 16, 9 * 16, 16, 16);
	Window::Draw(Assets::marioTexture, tilePos + vec(0, 32))
		.withRect(5 * 16, 9 * 16, 16, 16);
	float alpha = abs(sin(mainClock));
	Window::Draw(Assets::marioTexture, tilePos + vec(-16, 32))
		.withRect(6 * 16, 9 * 16, 16, 16)
		.withColor(255, 255, 255, alpha*255);
	Window::Draw(Assets::marioTexture, tilePos + vec(0, 32))
		.withRect(6 * 16, 9 * 16, 16, 16)
		.withColor(255,255,255,alpha*255);
}


