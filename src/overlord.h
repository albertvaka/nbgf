#pragma once

#include "entity.h"
#include "selfregister.h"

enum OverlordState {
	IDLE = -1,

	THROWING_KILL,
	THROWING_WAVE,
};

enum CooldownIndex {
	KILL = 0,
	WAVE
};

const float COOLDOWN_TIME[2] = {
	//Kill
	20,
	//Wave
	20
};

const SDL_Scancode skill_keys[2] = {
	SDL_SCANCODE_1,
	SDL_SCANCODE_2
};

struct Overlord : Entity, SelfRegister<Overlord>
{
	float cooldowns[2] = {
		0,
		0
	};
	OverlordState state = OverlordState::IDLE;
	vec cursorPos;
	Overlord();

	void Update(float dt);
	void Draw() const;
	
};
