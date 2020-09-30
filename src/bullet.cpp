#include "bullet.h"

#include "rand.h"
#include "partsys.h"
#include "camera.h"
#include "selfregister.h"
#include "assets.h"
#include "lava.h"
#include "skilltree.h"
#include "destroyedtiles.h"
#include "bat.h"

void Bullet::explode() {
	alive = false;
	Bat::AwakeNearbyBats(pos);
	Bullet::particles.pos = pos;
	for (int i = 0; i < 5; i++) {
		auto& p = Bullet::particles.AddParticle();
		p.scale = 1.7f;
		p.vel *= 1.5f;
	}
}

void Bullet::Update(float dt)
{
	pos += vel * dt;
	if (!Camera::GetBounds().Contains(pos)) {
		alive = false;
		return;
	}

	for (Lava* l : Lava::GetAll()) {
		if (l->IsInside(pos)) {
			Bat::AwakeNearbyBats(pos);
			l->Plof(pos.x);
			alive = false;
			return;
		}
	}

	if (BulletTilemapCollision(this)) {
		explode();
		return;
	}

	Bullet::particles.pos = pos + Rand::vecInRange(-4, -4, 4, 4);
	Bullet::particles.Spawn(dt);
}

void Bullet::Draw() const
{
	GPU_Rect rect = { 8 * 16, 10 * 16, 16, 16 };
	float rotation = Rand::roll(0, 360);
	vec drawPos = pos + Rand::vecInRange(-1, -1, 1, 1);
	Window::Draw(Assets::hospitalTexture, drawPos)
		.withScale(scale)
		.withOrigin(8, 8)
		.withRect(rect)
		.withRotationDegs(rotation);

	if (Debug::Draw) {
		DrawBounds();
	}
}

void Bullet::InitParticles() {
	if (particles.texture != nullptr) {
		return; // It's already been init
	}
	particles.SetTexture(Assets::marioTexture);
	particles.AddSprite({ 5, 37, 6, 6 });

	float vel = 15;
	particles.max_vel = vec(vel, vel);
	particles.min_vel = vec(-vel, -vel);
	particles.min_ttl = 0.5f;
	particles.max_ttl = 1.f;
	particles.min_interval = 0.03f;
	particles.max_interval = 0.06f;
	particles.min_scale = 0.5f;
	particles.max_scale = 0.9f;
	particles.scale_vel = -2.5f;
	particles.min_rotation = 0.f;
	particles.max_rotation = 360.f;
	particles.rotation_vel = 180.f;
	particles.alpha = 0.75f;
}
