#include "particles.h"
#include "assets.h"
#include "anim_lib.h"

namespace Particles {

void InitDust()
{
	dust.SetTexture(Assets::spritesheetTexture);
	dust.AddSprite(AnimLib::POLVITO_PARTICLE);

	dust.min_interval = 0.01f;
	dust.max_interval = 0.05f;

	dust.min_ttl = 0.1f;
	dust.max_ttl = 0.3f;

	dust.min_vel.x = 3;
	dust.max_vel.x = 18;

	dust.min_vel.y = -50.f;
	dust.max_vel.y = -20.f;

	dust.acc.y = 120.f;

	dust.min_scale = 1.0f;
	dust.max_scale = 1.8f;
	dust.scale_vel = -3.5f;

	dust.min_rotation = 0.f;
	dust.max_rotation = 360.f;
	dust.rotation_vel = 160.f;

	dust.alpha_vel = -1.8f;
}

void InitBullet()
{
	bullet.SetTexture(Assets::spritesheetTexture);
	bullet.AddSprite({ 5, 37, 6, 6 });

	float vel = 15;
	bullet.max_vel = vec(vel, vel);
	bullet.min_vel = vec(-vel, -vel);
	bullet.min_ttl = 0.5f;
	bullet.max_ttl = 1.f;
	bullet.min_interval = 0.03f;
	bullet.max_interval = 0.06f;
	bullet.min_scale = 0.5f;
	bullet.max_scale = 0.9f;
	bullet.scale_vel = -2.5f;
	bullet.min_rotation = 0.f;
	bullet.max_rotation = 360.f;
	bullet.rotation_vel = 180.f;
	bullet.alpha = 0.75f;
}

void InitHealth()
{
	health.SetTexture(Assets::spritesheetTexture);
	health.AddSprite({ 353, 18, 4, 4 });

	health.max_vel = vec(12, -13);
	health.min_vel = vec(-12, -16);
	health.min_ttl = 0.5f;
	health.max_ttl = 0.6f;
	health.min_interval = 0.1f;
	health.max_interval = 0.1f;
	health.min_scale = 0.5f;
	health.max_scale = 0.7f;
	health.scale_vel = -0.5f;
	health.min_rotation = 0.f;
	health.max_rotation = 360.f;
	health.rotation_vel = 180.f;
	health.rotation_vel = 180.f;
	health.alpha = 0.7f;
}

void InitMissile()
{
	missile.SetTexture(Assets::spritesheetTexture);
	missile.AddSprite(AnimLib::MISSILE_SMOKE_1);
	missile.AddSprite(AnimLib::MISSILE_SMOKE_2);

	missile.max_vel = vec(10, 10);
	missile.min_vel = vec(-10, -10);
	missile.min_ttl = 0.6f;
	missile.max_ttl = 1.f;
	missile.min_scale = 0.3f;
	missile.max_scale = 0.6f;
	missile.alpha = 0.5f;
	missile.alpha_vel = -1.0f;
}

void Init()
{
	if (missile.texture == nullptr)
	{
		InitDust();
		InitBullet();
		InitHealth();
		InitMissile();
	}
}

void ClearAll()
{
	dust.Clear();
	bullet.Clear();
	health.Clear();
	missile.Clear();
}

void DoDustJump(vec pos)
{
	// Both ways
	dust.pos = pos + vec(-1.5f, -1.5f);
	if (dust.min_vel.x > 0) {
		dust.FlipX();
	}
	dust.AddParticles(2);
	dust.pos = pos + vec(1.5f, -1.5f);
	dust.FlipX();
	dust.AddParticles(2);
}

void DoDustWallJump(vec pos, bool toTheLeft)
{
	if (toTheLeft) {
		dust.pos = pos + vec(7.f, -16.f);
	}
	else {
		dust.pos = pos + vec(-7.f, -16.f);
	}

	if ((toTheLeft && dust.min_vel.x > 0) || (!toTheLeft && dust.min_vel.x < 0)) {
		dust.FlipX();
	}
	dust.AddParticles(5);
}

void DoDustLand(vec pos)
{
	// Both ways
	dust.pos = pos + vec(-8.f, -0.3f);
	if (dust.min_vel.x > 0) {
		dust.FlipX();
	}
	dust.AddParticles(3);
	dust.pos = pos + vec(8.f, -0.3f);
	dust.FlipX();
	dust.AddParticles(3);
}

void DoDustRun(vec pos, float dt, bool toTheLeft, bool moarDust)
{
	if (toTheLeft) {
		dust.pos = pos + vec(4.f, -0.5f);
		if (dust.min_vel.x < 0) {
			dust.FlipX();
		}
	}
	else {
		dust.pos = pos + vec(-4.f, -0.5f);
		if (dust.min_vel.x > 0) {
			dust.FlipX();
		}
	}
	if (moarDust) {
		dust.AddParticles(2);
	}
	dust.Spawn(dt);
}

}