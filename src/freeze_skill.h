#pragma once

#include "entity.h"
#include "selfregister.h"
#include "assets.h"
#include "person.h"
#include "window.h"
#include "text.h"

template <typename T>
inline std::string to_string_with_precision(const T a_value, const int n = 2)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}

struct FreezeSkill : SelfRegister<FreezeSkill>
{
	Text actionText;
	float countdown = 1;
	float actionLength = 0.5;
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
			actionText.SetString("Freeze in" + to_string_with_precision(countdown, 2));
		} else if (actionLength > 0) {
			actionLength -= dt;
			actionText.SetString("Freeze!" + to_string_with_precision(actionLength, 2));
			freezeNow = true;
		} else {
			alive = false;
		}
	}
	void Draw(){
		Window::Draw(actionText, vec(Camera::Center().x, 60))
			.withOrigin(actionText.Size() / 2);
	}
};