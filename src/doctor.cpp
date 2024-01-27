
#include "doctor.h"

#include "input.h"
#include "assets.h"
#include "anim_lib.h"
#include "bullet.h"
#include "debug.h"
#include "window.h"
#include "tiled_objects_entities.h"
#include "common_tilemapcharacter.h"

const vec doctorSize = vec(120, 190);
const float doctorAccel = 5500.f;
const float maxVel = 600.f;
const float minVel = 50.f;
const float imageScale = 0.25f;
const float shakeHeight = 20.f;
const float shakeVerticalSpeed = 41.f;
const float shakeHorizontalSpeed = 31.f;
const float shakeHorizontalDegrees = 2.f;
const float gasCooldownTime = 0.01f;
const float gasSpeed = 550.f;
const float gasAngleVariationRads = 0.08*Angles::Tau;
const float gasOriginOffset = 25.f;

extern float mainClock;

Doctor::Doctor(vec pos)
	: BoxEntity(pos, doctorSize)
{
	for (const BoxBounds& room : Tiled::Entities::room) {
		if (room.Contains(pos)) {
			state = LEAVING_ROOM;
		}
	}
}

void Doctor::Update(float dt)
{
	// Move
	if (vel.x < -maxVel) vel.x = -maxVel;
	if (vel.x > maxVel) vel.x = maxVel;
	if (vel.y < -maxVel) vel.y = -maxVel;
	if (vel.y > maxVel) vel.y = maxVel;
	if (vel.x < minVel && vel.x > -minVel) vel.x = 0;
	if (vel.y < minVel && vel.y > -minVel) vel.y = 0;
	MoveResult res = MoveAgainstTileMap(pos, size, vel, dt);
	pos = res.pos;

}

void Doctor::Draw() const
{
	float shakeMagnitude = vel.Normalized().Length();
	Window::Draw(Assets::doctorTexture, pos)
		.withRotationDegs(sin(mainClock* shakeHorizontalSpeed *shakeMagnitude)*shakeHorizontalDegrees)
		.withOrigin(Assets::doctorTexture->base_w/2, Assets::doctorTexture->base_h / 2 + sin(mainClock*shakeVerticalSpeed)*shakeMagnitude* shakeHeight)
		.withScale(lookingLeft ? imageScale : -imageScale, imageScale);

	Bounds().DebugDraw();
}
