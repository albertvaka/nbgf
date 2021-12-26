#include "trigger.h"

#include "jumpman.h"
#include "collide.h"

void Trigger::Update(float dt)
{
	if (active) {
		JumpMan* player = JumpMan::instance();
		if (Collide(Bounds(), player->Bounds())) {
			callback(this);
			if (singleTime) {
				active = false;
			}
		}
	}
}

void Trigger::Draw() const
{
	if (active) {
		// Debug-only
		Bounds().DebugDraw();
	}
}
