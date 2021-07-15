#include "particles.h"
#include "assets.h"
#include "anim_lib.h"

namespace Particles {

void InitPolvito() {
	polvito.SetTexture(Assets::spritesheetTexture);
	polvito.AddSprite(AnimLib::POLVITO_PARTICLE);

	polvito.min_interval = 0.01f;
	polvito.max_interval = 0.05f;

	polvito.min_ttl = 0.1f;
	polvito.max_ttl = 0.3f;

	polvito.min_vel.x = 3;
	polvito.max_vel.x = 18;

	polvito.min_vel.y = -50.f;
	polvito.max_vel.y = -20.f;

	polvito.acc.y = 120.f;

	polvito.min_scale = 1.0f;
	polvito.max_scale = 1.8f;
	polvito.scale_vel = -3.5f;

	polvito.min_rotation = 0.f;
	polvito.max_rotation = 360.f;
	polvito.rotation_vel = 160.f;

	polvito.alpha_vel = -1.8f;
}

void InitBullet() {
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


void InitHealth() {
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

void InitMissile() {
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

void Init() {
	if (missile.texture == nullptr) {
		InitPolvito();
		InitBullet();
		InitHealth();
		InitMissile();
	}
}

void ClearAll() {
	Particles::polvito.Clear();
	Particles::bullet.Clear();
	Particles::health.Clear();
	Particles::missile.Clear();
}

}