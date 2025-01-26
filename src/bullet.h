#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "assets.h"
#include "animation.h"
#include "oneshotanims.h"
#include "window.h"
#include "camera.h"
#include "common_tilemapcharacter.h"

extern float mainClock;

const float gasScale = 0.11f;
const float minRotationSpeed = 60.f;
const float maxRotationSpeed = 130.f;
const float colliderSize = 10.f;
const float elasticGasBounceRatio = 0.3f;

struct Bullet : CircleEntity, SelfRegister<Bullet>
{
	bool hit[2] = { false ,false };
	int note;
	bool active = false;
	float rotation = Rand::rollf(0,360);
	float rotationSpeed = Rand::rollf(-90, +90);
	GPU_Rect tex;

	Bullet(int player, int note)
		: CircleEntity(pos, colliderSize)
		, tex(AnimLib::BUBBLE1[0].rect)
		, note(note)
	{

		switch (Rand::roll(4)) {
		case 0: tex = AnimLib::BUBBLE2[0].rect; break;
		case 1: tex = AnimLib::BUBBLE3[0].rect; break;
		case 2: tex = AnimLib::BUBBLE4[0].rect; break;
		}

		pos.y = Window::GAME_HEIGHT;
		pos.x = 718;
		while (note--) {
			pos.x += 803-718;
		}
	}

	void Update(float dt)
	{
		pos.y -= 150 * dt;
		rotation += rotationSpeed * dt;
	}

	void Explode() {
		switch (Rand::roll(4)) {
		case 0:new OneShotAnim(Assets::bubbleTexture, pos, AnimLib::BUBBLE1, gasScale); break;
		case 1:new OneShotAnim(Assets::bubbleTexture, pos, AnimLib::BUBBLE2, gasScale); break;
		case 2:new OneShotAnim(Assets::bubbleTexture, pos, AnimLib::BUBBLE3, gasScale); break;
		case 3:new OneShotAnim(Assets::bubbleTexture, pos, AnimLib::BUBBLE4, gasScale); break;
		}
	}

	void Draw() const
	{
		if (active) {
			Window::Draw(Assets::bubbleTexture, pos)
				.withRect(tex)
				.withColor(255, 255, 255)
				.withOrigin(vec(tex.w, tex.h) / 2)
				.withRotationDegs(rotation)
				.withScale(gasScale*1.1);
		}
		Window::Draw(Assets::bubbleTexture, pos)
			.withRect(tex)
			.withOrigin(vec(tex.w, tex.h) / 2)
			.withRotationDegs(rotation)
			.withScale(gasScale);
	}
};
