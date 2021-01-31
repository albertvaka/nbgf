#pragma once

#include "entity.h"
#include "selfregister.h"

enum OverlordState {
	IDLE = -1,
	THROWING_KILL,
	THROWING_FREEZE,
	THROWING_WAVE,
	THROWING_GATH,
};

enum CooldownIndex {
	KILL = 0,
	FREEZE,
	WAVE,
	GATH,
};

const float COOLDOWN_TIME[4] = {
	//Kill
	0.1,
	//Freeze
	10,
	//Wave
	5,
	//Gath
	5
};

const SDL_Scancode skill_keys[4] = {
	SDL_SCANCODE_1,
	SDL_SCANCODE_2,
	SDL_SCANCODE_3,
	SDL_SCANCODE_4,
};

struct Overlord : Entity, SelfRegister<Overlord>
{
	float cooldowns[4] = {
		0,
		0,
		0,
		0,
	};
	OverlordState state = OverlordState::IDLE;
	vec cursorPos;
	Overlord();

	void Update(float dt);
	void Draw() const;
	
};
