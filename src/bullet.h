#pragma once

#include "entity.h"
#include "anim_lib.h"
#include "selfregister.h"
#include "assets.h"
#include "animation.h"
#include "window.h"
#include "camera.h"
#include "common_tilemapcharacter.h"

extern float mainClock;

const float gasScale = 4.f;
const float minRotationSpeed = 60.f;
const float maxRotationSpeed = 130.f;
const float gasSize = 6.f;
const float elasticGasBounceRatio = 0.3f;

struct Bullet : CircleEntity, SelfRegister<Bullet>
{
	//Animation anim;

	Bullet(int player, int note)
		: CircleEntity(pos, gasSize* gasScale)
	{
		pos.y = Window::GAME_HEIGHT;
		if (player == 0) {
			pos.x = 100;
		} else {
			pos.x = 600;
		}
		while (note--) {
			pos.x += 50;
		}
	}

	void Update(float dt)
	{
		pos.y -= 50 * dt;

		/*
		anim.Update(dt);
		if (anim.IsComplete()) {
			alive = false;
		}
		*/
	}

	void Draw() const
	{
		//Assets::tintShader.Activate();
		//Assets::tintShader.SetUniform("flashColor", 0.8f, 0.8f, 0.f, 0.3f);
		GPU_Rect animRect = { 16 * 16,17 * 16,16, 16 };
		Window::Draw(Assets::spritesheetTexture, pos)
			.withOrigin(vec(animRect.w, animRect.h)/2)
			.withRect(animRect)
			.withScale(gasScale);
		//Shader::Deactivate();
	}
};
