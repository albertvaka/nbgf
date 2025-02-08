#include "particles.h"
#include "assets.h"
#include "anim_lib.h"

namespace Particles {

void InitDust(PartSys& ps)
{
	ps.SetTexture(Assets::spritesheetTexture);
	ps.AddSprite(AnimLib::POLVITO_PARTICLE);

	ps.min_interval = 0.01f;
	ps.max_interval = 0.05f;

	ps.min_ttl = 0.1f;
	ps.max_ttl = 0.3f;

	ps.min_vel.x = 3;
	ps.max_vel.x = 18;

	ps.min_vel.y = -50.f;
	ps.max_vel.y = -20.f;

	ps.acc.y = 120.f;

	ps.min_scale = 1.0f;
	ps.max_scale = 1.8f;
	ps.scale_vel = -3.5f;

	ps.min_rotation = 0.f;
	ps.max_rotation = 360.f;
	ps.min_rotation_vel = 160.f;
	ps.max_rotation_vel = 160.f;

	ps.alpha_vel = -1.8f;
}

void InitLeafs(PartSys& ps)
{
	ps.SetTexture(Assets::spritesheetTexture);
	ps.AddSprite({ 496, 288     , 16, 16 });
	ps.AddSprite({ 496, 288 + 16, 16, 16 });
	ps.AddSprite({ 496, 288 + 32, 16, 16 });
	ps.AddSprite({ 496, 288 + 48, 16, 16 });
	ps.AddSprite({ 496, 288 + 64, 16, 16 });
	ps.AddSprite({ 496, 288 + 80, 16, 16 });

	ps.min_ttl = 2.f;
	ps.max_ttl = 2.f;

	ps.min_vel.x = -1.f;
	ps.max_vel.x = 55.f;

	ps.min_vel.y = 180.f;
	ps.max_vel.y = 350.f;

	ps.alpha = 1.f;
	ps.alpha_vel = -0.35f;

	ps.min_rotation = 0.f;
	ps.max_rotation = 360.f;

	ps.min_rotation_vel = -80.f;
	ps.max_rotation_vel = -200.f;

	ps.acc.y = -90.f;

	ps.min_scale = 0.5f;
	ps.max_scale = 0.8f;
	ps.scale_vel = -0.14f;

}

void InitBullet(PartSys& ps)
{
	ps.SetTexture(Assets::spritesheetTexture);
	ps.AddSprite({ 5, 37, 6, 6 });

	float vel = 15;
	ps.max_vel = vec(vel, vel);
	ps.min_vel = vec(-vel, -vel);
	ps.min_ttl = 0.5f;
	ps.max_ttl = 1.f;
	ps.min_interval = 0.03f;
	ps.max_interval = 0.06f;
	ps.min_scale = 0.5f;
	ps.max_scale = 0.9f;
	ps.scale_vel = -2.5f;
	ps.min_rotation = 0.f;
	ps.max_rotation = 360.f;
	ps.min_rotation_vel = 180.f;
	ps.max_rotation_vel = 180.f;
	ps.alpha = 0.75f;
}

void InitHealth(PartSys& ps)
{
	ps.SetTexture(Assets::spritesheetTexture);
	ps.AddSprite({ 353, 18, 4, 4 });

	ps.max_vel = vec(12, -13);
	ps.min_vel = vec(-12, -16);
	ps.min_ttl = 0.5f;
	ps.max_ttl = 0.6f;
	ps.min_interval = 0.1f;
	ps.max_interval = 0.1f;
	ps.min_scale = 0.6f;
	ps.max_scale = 0.8f;
	ps.scale_vel = -0.5f;
	ps.min_rotation = 0.f;
	ps.max_rotation = 360.f;
	ps.min_rotation_vel = 180.f;
	ps.max_rotation_vel = 180.f;
	ps.alpha = 0.7f;
}

void InitMissile(PartSys& ps)
{
	ps.SetTexture(Assets::spritesheetTexture);
	ps.AddSprite(AnimLib::MISSILE_SMOKE_1);
	ps.AddSprite(AnimLib::MISSILE_SMOKE_2);

	ps.max_vel = vec(10, 10);
	ps.min_vel = vec(-10, -10);
	ps.min_ttl = 0.6f;
	ps.max_ttl = 1.f;
	ps.min_scale = 0.3f;
	ps.max_scale = 0.6f;
	ps.alpha = 0.5f;
	ps.alpha_vel = -1.0f;
}

void InitItemSparks(PartSys& ps)
{
	ps.SetTexture(Assets::spritesheetTexture);
	ps.AddSprite({ 5, 37, 6, 6 });
	ps.max_vel = vec(0, -12);
	ps.min_vel = vec(0, -35);
	ps.min_ttl = 1.f;
	ps.max_ttl = 1.f;
	ps.min_interval = 0.15f;
	ps.max_interval = 0.25f;
	ps.min_scale = 0.6f;
	ps.max_scale = 0.4f;
	ps.alpha = 0.7f;
	ps.alpha_vel = -0.8f;
}

void InitOoyTearTrails(PartSys& ps)
{
	ps.SetTexture(Assets::spritesheetTexture);
	for (int i = 0; i < AnimLib::OOY_TEAR.size(); i++) {
		ps.AddSprite(AnimLib::OOY_TEAR[i].rect);
	}
	ps.min_interval = 0.15f;
	ps.max_interval = 0.25f;
	ps.max_vel = vec(10, 10);
	ps.min_vel = vec(-10, -10);
	ps.min_ttl = 0.6f;
	ps.max_ttl = 1.f;
	ps.min_scale = 0.4f;
	ps.max_scale = 0.65f;
	ps.alpha = 0.5f;
	ps.alpha_vel = -1.0f;
}

void Init()
{
	if (missile.texture == nullptr)
	{
		InitLeafs(leafs);
		InitDust(dust);
		InitBullet(bullet);
		InitHealth(health);
		InitMissile(missile);
		InitItemSparks(itemSparks);
		InitOoyTearTrails(ooyTearTrail);
	}
}

void UpdateAll(float dt)
{
	leafs.UpdateParticles(dt);
	dust.UpdateParticles(dt);
	bullet.UpdateParticles(dt);
	health.UpdateParticles(dt);
	missile.UpdateParticles(dt);
	itemSparks.UpdateParticles(dt);
	ooyTearTrail.UpdateParticles(dt);
}

void ClearAll()
{
	leafs.Clear();
	dust.Clear();
	bullet.Clear();
	health.Clear();
	missile.Clear();
	itemSparks.Clear();
	ooyTearTrail.Clear();
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
