#pragma once

#include "entity.h"
#include "selfregister.h"
#include "assets.h"
#include "person.h"
#include "window.h"
#include "text.h"

struct FreezeSkill : SelfRegister<FreezeSkill>
{
	Text actionText;
	float countdown = 2;
	float actionLength = 1.5;
	bool freezeNow = false;
	bool alive = true;
	FreezeSkill() : actionText(Assets::font_30, Assets::font_30_outline) 
	{
		actionText.SetFillColor(0, 0, 0);
		actionText.SetOutlineColor(255, 255, 0);
	}
	void Update(float dt){
		if (countdown > 0) {
			countdown -= dt;
			actionText.SetString("Freeze in " + Mates::to_string_with_precision(std::max(0.f, countdown), 2));
		} else if (actionLength > 0) {
			actionLength -= dt;
			actionText.SetString("Freeze! " + Mates::to_string_with_precision(std::max(0.f,actionLength), 2));
			freezeNow = true;
		} else {
			alive = false;
		}
	}
	void Draw(){
		Window::Draw(actionText, vec(Camera::Center().x, 60))
			.withOrigin(actionText.Size() / 2)
			.withScale(1.5f);
	}
};