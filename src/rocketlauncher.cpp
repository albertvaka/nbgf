#include "rocketlauncher.h"

#include "jumpman.h"
#include "screen.h"
#include "assets.h"
#include "window.h"
#include "rand.h"
#include "debug.h"
#include "tiled_objects_areas.h"
#include "missile.h"
#include "fx.h"
#include "camera.h"
#include "tile.h"
#include "bullet.h"
#include "common_enemy.h"

const float kTimeBetweenMissiles = 3.f;
const vec missilesOriginOffset = vec(0, 0);

RocketLauncher::RocketLauncher(vec pos)
	: pos(pos)
{
	for (const BoxBounds& e : Tiled::Areas::rocket_launcher_awake) {
		if (e.Contains(pos)) {
			awakeArea.push_back(e);
		}
	}

	pos = Tile::AlignToTiles(pos);
}

void RocketLauncher::Update(float dt)
{
	bool awoken = false;
	for (const BoxBounds& e : awakeArea) {
		if (Collide(e, JumpMan::instance()->Bounds())) {
			awoken = true;
			break;
		}
	}
	if (!awoken) {
		return;
	}

	float oldTimer = timer;
	float timeBetweenMissiles = 0.4f;
	vec shotsOrigin = pos + missilesOriginOffset;
	timer += dt;
	if (oldTimer < kTimeBetweenMissiles && timer >= kTimeBetweenMissiles) {
		new Missile(shotsOrigin, 180);
		timer = 0;
	}
	/*
	else if (oldTimer < 2 * timeBetweenMissiles && timer >= 2 * timeBetweenMissiles) {
		new Missile(shotsOrigin, -90);
	}
	else if (oldTimer < 3 * timeBetweenMissiles && timer >= 3 * timeBetweenMissiles) {
		new Missile(shotsOrigin, -45);
	}*/
}

void RocketLauncher::Draw() const
{
	
	Window::Draw(Assets::scifiTexture, pos)
		.withRect(AnimLib::ROCKET_LAUCNHER_BASE);

	Window::Draw(Assets::scifiTexture, pos)
		.withRotationDegs(angle)
		.withRect(AnimLib::ROCKET_LAUCNHER_TURRET);


	for (const BoxBounds& e : awakeArea) {
		e.DebugDraw();
	}
}

