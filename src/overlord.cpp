
#include "overlord.h"

#include "raw_input.h"
#include "window.h"
#include "debug.h"
#include "person.h"
#include "wave_skill.h"
#include "anim_lib.h"

const int SKILL_SIZE = 100;

Overlord::Overlord()
{
}

bool killPersonAt(vec pos) {
	bool hasKilled = false;
	for(Person* p : Person::GetAll()) {
		if(p->ClickBounds().Contains(pos)) {
			p->Kill();
			hasKilled = true;
			break; // Kill only the first person found
		}
	}
	return hasKilled;
}

void Overlord::Update(float dt)
{
	cursorPos = Mouse::GetPositionInWorld();
	for (int i = 0; i < std::size(skill_keys); i++) {
		if (Keyboard::IsKeyJustPressed(skill_keys[i])) {
			if (cooldowns[i] <= 0) {
				state = static_cast<OverlordState>(i);
			} else {
				Debug::out << i << " is still in cooldown " << cooldowns[i];
			}
		}
	}
	if (Keyboard::IsKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
		state = OverlordState::IDLE;
	}
	// Skills that don't need a target
	switch(state) {
		case OverlordState::THROWING_FREEZE: {
			new FreezeSkill();
			state = OverlordState::IDLE;
			cooldowns[CooldownIndex::FREEZE] = COOLDOWN_TIME[CooldownIndex::FREEZE];
			break;
		}
	}
	// Skills that need a target
	if (Mouse::IsJustPressed(Mouse::Button::Left)) {
		switch(state) {
			case OverlordState::THROWING_KILL: {
				Debug::out << "Trying to kill at " << cursorPos;
				if (killPersonAt(cursorPos)) {
					Debug::out << "Successful kill";
					state = OverlordState::IDLE;
					cooldowns[CooldownIndex::KILL] = COOLDOWN_TIME[CooldownIndex::KILL];
				}
				break;
			}
			case OverlordState::THROWING_WAVE: {
				Debug::out << "Throwing wave at" << cursorPos;
				new WaveSkill(cursorPos);
				state = OverlordState::IDLE;
				cooldowns[CooldownIndex::WAVE] = COOLDOWN_TIME[CooldownIndex::WAVE];
				break;
			}
		}
	}

	for (int i = 0; i < std::size(cooldowns); i++) {
		if (cooldowns[i] > 0) {
			cooldowns[i] -= dt;
		}
	}
}

void Overlord::Draw() const
{

	if(Debug::Draw) {
		vec mouseEnd = cursorPos + vec(-1,-1)*20;
		Window::DrawPrimitive::Arrow(mouseEnd, cursorPos, 2, 10, {0, 255, 255, 255});
	}
	
	for (int i =0; i < std::size(cooldowns); i++) {
		int c = ((COOLDOWN_TIME[i]-cooldowns[i])/COOLDOWN_TIME[i]) * 255;
		//Camera::InScreenCoords::Begin();
		Window::DrawPrimitive::Rectangle(0, SKILL_SIZE*i, SKILL_SIZE, SKILL_SIZE*(i+1), 5, c,c,c);
		//Camera::InScreenCoords::End();
	}

	/*
	const GPU_Rect& animRect = AnimLib::Overlord;
	Window::Draw(Assets::overlordTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, 0)/2)
	*/
}
