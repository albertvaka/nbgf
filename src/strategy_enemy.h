#pragma once

#include "entity.h"
#include "selfregister.h"
#include "animation.h"
#include "enemy_bullet.h"
#include "rand.h"
#include "particles.h"
#include "assets.h"

struct StrategyEnemy;
struct Player;

// TODO: Consider taking 'const Player&' as argument here, and having the MainScene pass it to
// the StrategyEnemy::Update.
// This would allow having default strategies defined here that can take Player position.
using ShootingStrategy = std::function<void(StrategyEnemy& self, float dt, float total_time)>;
using MovingStrategy = std::function<void(StrategyEnemy& self, float dt)>;

void EmptyShootingStrategy(StrategyEnemy& self, float dt, float total_time) {}
void StaticMovingStrategy(StrategyEnemy& self, float dt) {}

// StrategyEnemy is an enemy that takes `ShootingStrategy` and `MovingStrategy` behaviors,
// allowing customization of its logic outside the class.
struct StrategyEnemy : CircleEntity, SelfRegister<StrategyEnemy>
{
	vec initialPos;
	MovingStrategy movingStrategy;
	ShootingStrategy shootingStrategy;
	float total_time = 0.0f;
	float rot_rads = 0.0f;
	int hp = 1;
	float flashRedTimer = 0.f;
	char c;

	GPU_Rect animRect;

	StrategyEnemy(char c, const GPU_Rect& animRect, vec pos, const ShootingStrategy& shooting_strategy = EmptyShootingStrategy, const MovingStrategy& moving_strategy = StaticMovingStrategy)
		: CircleEntity(pos, 15.f)
		, initialPos(pos)
		, movingStrategy(moving_strategy)
		, shootingStrategy(shooting_strategy)
		, animRect(animRect)
		, c(c)
	{
	}
	void Hit() {
		hp--;
		flashRedTimer = 0.3f;
		if (hp <= 0) {
			alive = false;
			Assets::enemyExplosionSound.Play();
			Particles::explosion.pos = pos;
			Particles::explosion.AddParticles(10);
		}
	}

	void Update(float dt)
	{
		movingStrategy(*this, dt);
		shootingStrategy(*this, dt, total_time);
		total_time += dt;
		flashRedTimer -= dt;
	}

	void Draw() const
	{
		if (flashRedTimer > 0) {
			Assets::tintShader.Activate();
			Assets::tintShader.SetUniform("flashColor", 1.f, 0.f, 0.f, 0.7f);
		}
		Window::Draw(Assets::spritesTexture, pos)
			.withOrigin(vec(animRect.w, animRect.h)/2)
			.withRect(animRect)
			.withRotationRads(rot_rads)
			.withScale(2.0f);
		Shader::Deactivate();
	}
};
