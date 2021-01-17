#pragma once

#include "assets.h"
#include "window.h"
#include "camera.h"
#include "singleinstance.h"
#include "text.h"

struct RotoText : Text, SingleInstance<RotoText>
{
	float timer = -1;

	const float DURATION = 2.2f;
	const float INTRO_DURATION = 0.3f;

	RotoText() : Text(Assets::font_30, Assets::font_30_outline)
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
	}

	void Draw(vec pos, float scale = 1.f) {
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

		Window::Draw(*this, Camera::GetCenter() + pos)
			.withOrigin(GetSize() / 2.f)
			.withScale(sc)
			.withRotationDegs(ang);
	}
};


