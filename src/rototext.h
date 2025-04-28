#pragma once

#include "vec.h"
#include "camera.h"
#include "input.h"
#include "assets.h"
#include "window.h"
#include "text.h"

extern float mainClock;

class RotoText : public Text
{
	float messageTime;
	float scale;
	float rotation;
public:

	RotoText() : Text(Assets::font_30, Assets::font_30_outline) {
		SetFillColor(255,255,255);
		SetOutlineColor(0,0,0);
		messageTime = -1;
	}

	void ShowMessage(const std::string& msg) {
	
		SetString(msg);
		
		messageTime = 0.01f;
		Update(0);

	}

	float GetElapsedTime() {
		return messageTime;
	}

	void Update(float dt) {
		
		if (messageTime >= 0) {

			messageTime += dt/5;

			float ang = sin(mainClock*2)*10;
			float sc = 1;
			if(messageTime < 0.2)
			{
				sc = 1+(0.2-messageTime)*10;
				ang -= (0.2-messageTime)*600;
			}
			
			rotation = ang;

			if(messageTime < 0.2) {
				sc = messageTime / 0.2;
			}
			scale = sc;

		}

	}

	void Draw() {
		if (messageTime < 0) {
			return;
		}

		Window::Draw(*this, Camera::Center() - vec(0,60))
			.withOrigin(Size() / 2.f)
			.withScale(scale)
			.withRotationDegs(rotation);
	}

};


