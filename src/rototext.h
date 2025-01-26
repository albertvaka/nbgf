#pragma once

#include "window_draw.h"
#include "camera.h"
#include "selfregister.h"
#include "text.h"

struct RotoText : Text, SelfRegister<RotoText>
{
	bool alive = true;
	float timer = -1;

	float secretScale = 1;
	float DURATION = 0.6f;
	const float INTRO_DURATION = 0.25;
	vec pos;

	RotoText(vec pos, TTF_Font* font, TTF_Font* font_outline = nullptr) : Text(font, font_outline), pos(pos)
	{
		SetFillColor(255,255,255);
		SetOutlineColor(0,0,0);
		SetMultilineAlignment(MultilineAlignment::CENTER);
	}

	void ShowMessage(const std::string& msg)
	{
		SetString(msg);
		timer = 0.01f;
	}

	void Update(float dt) {
		if (timer >= 0) {
			timer += dt;
		}
		else {
			alive = false;
		}
	}

	void Draw(float scale = 1.f) {
		if (timer < 0) {
			return;
		}

		float messageTime = timer;
		if (messageTime > DURATION) {
			messageTime = (DURATION + INTRO_DURATION) - messageTime;
			if (messageTime < 0) {
				timer = -1.f;
				return;
			}
		}

		float ang = sin(messageTime * 2) * 10;
		float sc = 1;
		if (messageTime < INTRO_DURATION)
		{
			sc = 1 + (INTRO_DURATION - messageTime) * 10;
			ang -= (INTRO_DURATION - messageTime) * 600;
		}

		if (messageTime < INTRO_DURATION) {
			sc = messageTime / INTRO_DURATION;
		}

		Window::Draw(*this, pos)
			.withOrigin(Size() / 2.f)
			.withScale(sc*scale* secretScale)
			.withRotationDegs(ang);
	}
};


