#pragma once

#include "window_draw.h"
#include "camera.h"
#include "text.h"

struct RotoText : Text
{
	float timer = -1;

	float kDuration;
	const float kIntroDuration = 0.3f;

	RotoText(TTF_Font* font, TTF_Font* font_outline = nullptr, float duration = -1.f)
		: Text(font, font_outline)
		, kDuration(duration)
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
			float messageTime = timer;
			if (kDuration > 0.f && messageTime > kDuration) {
				messageTime = (kDuration + kIntroDuration) - messageTime;
				if (messageTime < 0) {
					timer = -1.f;
				}
			}
		}
	}

	void Draw(vec pos, float scale = 1.f) const {
		if (timer < 0) {
			return;
		}
		
		float messageTime = timer;
		if (kDuration > 0.f && messageTime > kDuration) {
			messageTime = (kDuration + kIntroDuration) - messageTime;
		}

		float ang = sin(messageTime * 2) * 10;
		float sc = 1;
		if (messageTime < kIntroDuration)
		{
			sc = 1 + (kIntroDuration - messageTime) * 10;
			ang -= (kIntroDuration - messageTime) * 600;
		}

		if (messageTime < kIntroDuration) {
			sc = messageTime / kIntroDuration;
		}

		Window::Draw(*this, pos)
			.withOrigin(Size() / 2.f)
			.withScale(scale * sc)
			.withRotationDegs(ang);
	}
};


