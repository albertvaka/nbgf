#pragma once

#include "entity.h"
#include "selfregister.h"

enum OverlordState {
	IDLE = -1,
	THROWING_KILL,
	THROWING_FREEZE,
	THROWING_WAVE,
};

enum CooldownIndex {
	KILL = 0,
	FREEZE,
	WAVE,
};

const float COOLDOWN_TIME[3] = {
	//Kill
	5.f,
	//Freeze
	8.0f,
	//Wave
	4.0f,
};

const SDL_Scancode skill_keys[3] = {
	SDL_SCANCODE_1,
	SDL_SCANCODE_2,
	SDL_SCANCODE_3,
};

struct Overlord : Entity, SelfRegister<Overlord>
{
	float cooldowns[3] = {
		0,
		0,
		0,
	};
	vec buttonPos[3];
	vec bottomPos;
	OverlordState state = OverlordState::IDLE;
	vec cursorPos;
	Overlord();

	void Update(float dt);
	void Draw() const;
	
};
