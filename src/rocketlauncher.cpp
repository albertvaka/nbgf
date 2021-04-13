#include "rocketlauncher.h"

#include "jumpman.h"
#include "assets.h"
#include "window.h"
#include "tiled_objects_areas.h"
#include "missile.h"
#include "tile.h"
#include "collide.h"
#include "common_tilemapcharacter.h"

const float kTimeBetweenMissiles = 3.f;
const vec kCanonOrigin = vec(0, -4);
const vec kCanonLength = vec(10, 0);
const vec kAwokenLedOffset = vec(-3, -7);

extern float mainClock;

RocketLauncher::RocketLauncher(vec pos)
	: timer(kTimeBetweenMissiles/2.f)
{
	for (const BoxBounds& e : Tiled::Areas::rocket_launcher_awake) {
		if (e.Contains(pos)) {
			awakeArea.push_back(e);
		}
	}
	if (awakeArea.empty()) {
		Debug::out << "RocketLauncher not in any rocket_launcher_awake area";
	}

	this->pos = AlignWithCeiling(pos, Tile::Sizes);
}

void RocketLauncher::Update(float dt)
{
	angle = pos.AngleDegs(JumpMan::instance()->pos);
	Mates::Clamp(angle, 30, 150);

	awoken = false;
	for (const BoxBounds& e : awakeArea) {
		if (Collide(e, JumpMan::instance()->Bounds())) {
			awoken = true;
			break;
		}
	}
	if (!awoken) {
		return;
	}

	timer += dt;
	if (timer >= kTimeBetweenMissiles) {
		vec shotsOrigin = pos + kCanonOrigin + kCanonLength.RotatedAroundOriginDegs(angle);
		new Missile(shotsOrigin, angle);
		timer -= kTimeBetweenMissiles;
	}
}

void RocketLauncher::Draw() const
{
	Window::Draw(Assets::spritesheetTexture, pos)
		.withOrigin(Tile::Sizes / 2)
		.withRect(AnimLib::ROCKET_LAUCNHER_BASE);

	Window::Draw(Assets::spritesheetTexture, pos+kCanonOrigin)
		.withOrigin(Tile::Sizes / 2)
		.withRotationDegs(angle)
		.withRect(AnimLib::ROCKET_LAUCNHER_TURRET);

	const SDL_Color on = { 255, 0, 0, 255 };
	const SDL_Color off = { 255, 255, 128, 255 };
	bool blink = awoken && (int(mainClock * 7) % 2);
	Window::DrawPrimitive::Point(pos + kAwokenLedOffset, 2.f, blink? on : off);

	// Debug-only
	for (const BoxBounds& e : awakeArea) {
		e.DebugDraw();
	}
	pos.DebugDraw();
	vec shotsOrigin = pos + kCanonOrigin + kCanonLength.RotatedAroundOriginDegs(angle);
	shotsOrigin.DebugDraw();
}

