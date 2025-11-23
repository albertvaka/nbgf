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
	vec vel;
	Animation anim;
	float roationOffset;
	float rotationSpeed;

	Bullet(const vec& position, const vec& velocity)
		: CircleEntity(pos, gasSize* gasScale)
		, anim(AnimLib::GAS, false)
		, roationOffset(Rand::rollf(360.f))
		, rotationSpeed(Rand::rollf(minRotationSpeed, maxRotationSpeed))
	{
		pos = position;
		vel = velocity;
	}

	void Update(float dt)
	{

		MoveResult res = MoveAgainstTileMap(pos, vec(gasSize *gasScale*2, gasSize * gasScale * 2), vel, dt);
		if (res.leftWallCollision != Tile::NONE || res.rightWallCollision != Tile::NONE) {
			float prevVelX = vel.x;
			vel.x *= -elasticGasBounceRatio;
			float sign = vel.y > 0 ? 1.f : -1.f;
			vel.y += abs(prevVelX + vel.x) * sign;
		}
		if (res.ceilingCollision != Tile::NONE || res.groundCollision != Tile::NONE) {
			float prevVelY = vel.y;
			vel.y *= -elasticGasBounceRatio;
			float sign = vel.x > 0? 1.f : -1.f;
			vel.x += abs(prevVelY + vel.y) * sign;
		}
		pos = res.pos;
		anim.Update(dt);
		if (anim.IsComplete()) {
			alive = false;
		}

		//if (!Camera::Bounds().Contains(pos)) {
//			alive = false;
//		}
	}

	void Draw() const
	{
		const GPU_Rect& animRect = anim.CurrentFrameRect();
		Window::Draw(Assets::spritesheetTexture, pos)
			.withOrigin(vec(animRect.w, animRect.h)/2)
			.withRect(animRect)
			.withRotationDegs(roationOffset + mainClock*rotationSpeed)
			.withScale(gasScale);
	}

	static void DrawAll() {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform(Assets::tintShaderColor, 0.8f, 0.8f, 0.f, 0.3f);
		for (const Bullet* b : Bullet::GetAll()) {
			b->Draw();
			b->Bounds().DebugDraw(255, 0, 0);
		}
		Assets::tintShader.SetUniform(Assets::tintShaderColor, 0.f, 0.f, 0.f, 0.f);
		Assets::tintShader.Deactivate();
	}
};
