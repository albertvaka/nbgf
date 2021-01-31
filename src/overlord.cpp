
#include "overlord.h"

#include "raw_input.h"
#include "window.h"
#include "debug.h"
#include "person.h"
#include "wave_skill.h"
#include "anim_lib.h"

const int SKILL_BG_X = 358;
const int SKILL_BG_Y = 112;
const int SKILL_SIZE = 54;

const float BG_SCALE = 2;
const float ICON_SCALE = 2.5;

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
			case OverlordState::THROWING_GATH: {
				Debug::out << "TODO: Throwing gath at" << cursorPos;
				//new WaveSkill(cursorPos);
				state = OverlordState::IDLE;
				cooldowns[CooldownIndex::GATH] = COOLDOWN_TIME[CooldownIndex::GATH];
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

	float w = Camera::Size().x + Camera::TopLeft().x; 
	float h = Camera::Size().y + Camera::TopLeft().y;

	
	float realBgXSize = SKILL_BG_X*BG_SCALE;
	float realBgYSize = SKILL_BG_Y*BG_SCALE;
	float realIconSize = SKILL_SIZE*ICON_SCALE;
	float iconMarginTop = realBgYSize * 0.1;
	float iconLeftMargin = ((realBgXSize)/4)/2;
	vec bottomCenter = vec(w/2, h-realBgYSize/2); 
	vec bottomLeft = vec(w/2-realBgXSize/2, h-realBgYSize/2); 
	Window::Draw(Assets::skillBg, bottomCenter-vec(realBgXSize/2,realBgYSize/2))
		.withScale(BG_SCALE);

	float iconAlphas[4];
	float iconColors[4];
	for (int i = 0; i < std::size(cooldowns); i++) {
		float mc = ((COOLDOWN_TIME[i]-cooldowns[i])/COOLDOWN_TIME[i]) * 255;
		iconColors[i] = 255; 
		if(mc < 255) {
			mc *= 0.7;
			iconColors[i] = 125; 
		} else {
			mc = 255;
		}
		iconAlphas[i] = mc;
	}

	vec pos1 = bottomLeft 
		// First quarter's center 
		+ vec(iconLeftMargin, iconMarginTop)
		// Icon center 
		- vec(realIconSize/2, realIconSize/2);
	Window::Draw(Assets::mortIcon, 	pos1)
		.withScale(ICON_SCALE)
		.withColor(iconColors[0],iconColors[0],iconColors[0],iconAlphas[0]);
	Window::Draw(Assets::freezeIcon,pos1+vec(iconLeftMargin*2, 0))
		.withScale(ICON_SCALE)
		.withColor(iconColors[1],iconColors[1],iconColors[1],iconAlphas[1]);
	Window::Draw(Assets::waveIcon, 	pos1+vec(iconLeftMargin*4, 0))
		.withScale(ICON_SCALE)
		.withColor(iconColors[2],iconColors[2],iconColors[2],iconAlphas[2]);
	Window::Draw(Assets::gathIcon, 	pos1+vec(iconLeftMargin*6, 0))
		.withScale(ICON_SCALE)
		.withColor(iconColors[3],iconColors[3],iconColors[3],iconAlphas[3]);
	/*
	const GPU_Rect& animRect = AnimLib::Overlord;
	Window::Draw(Assets::overlordTexture, pos)
		.withRect(animRect)
		.withOrigin(vec(animRect.w, 0)/2)
	*/
}
