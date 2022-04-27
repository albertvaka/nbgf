#include "drain.h"

#include "jumpman.h"
#include "assets.h"
#include "rand.h"
#include "screen.h"
#include "anim_lib.h"
#include "window_draw.h"
#include "ooytear.h"

const float kTimeWithoutDrops = 2.5f;
const float kTimeBetweenDrops = 0.05f;
const int kNumDrops = 7;
const float kTotalCycleTime = kTimeWithoutDrops + kNumDrops * kTimeBetweenDrops;

constexpr const float kDropVel = 230;

Drain::Drain(vec pos)
	: pos(pos)
	, screen(ScreenManager::FindScreenContaining(pos))
	, timer(Rand::rollf(kTotalCycleTime))
{
}

void Drain::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		return;
	}

	float oldTimer = timer;
	timer += dt;
	if (timer >= kTimeWithoutDrops) {
		int oldDropCount = (oldTimer - kTimeWithoutDrops) / kTimeBetweenDrops;
		int dropCount = (timer - kTimeWithoutDrops) / kTimeBetweenDrops;
		if (oldDropCount != dropCount) {
			new OoyTear(pos, vec(0, kDropVel));
			if (dropCount >= kNumDrops) {
				timer -= kTotalCycleTime;
			}
		}
	}
}

void Drain::Draw() const
{
	Window::Draw(Assets::spritesheetTexture, pos)
		.withRectWithOriginCentered(AnimLib::DRAIN);

	pos.DebugDraw();
}
