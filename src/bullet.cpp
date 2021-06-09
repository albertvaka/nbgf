#include "bullet.h"

#include "rand.h"
#include "partsys.h"
#include "camera.h"
#include "assets.h"
#include "anim_lib.h"
#include "lava.h"
#include "explosive.h"
#include "skilltree.h"
#include "common_bullet.h"
#include "bat.h"

void Bullet::Explode() {
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
	BoxBounds cam = Camera::Bounds();
	cam.Grow(Tile::Size, Tile::Size);
	if (!cam.Contains(pos)) {
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

	for (Explosive* e : Explosive::GetAll()) {
		if (e->CheckBulletCollision(Bounds())) {
			Explode();
			return;
		}
	}

	if (BigBulletTilemapCollision(this, Tile::BreakResistance::HARD_OR_LOWER)) {
		Explode();
		return;
	}

	pos += vel * dt;

	Bullet::particles.pos = pos + Rand::VecInRange(-4, -4, 4, 4);
	Bullet::particles.Spawn(dt);
}

void Bullet::Draw() const
{
	float rotation = Rand::roll(0, 360);
	vec drawPos = pos + Rand::VecInRange(-1, -1, 1, 1);
	Window::Draw(Assets::spritesheetTexture, drawPos)
		.withScale(kSpriteScale)
		.withOrigin(8, 8)
		.withRect(AnimLib::BULLET)
		.withRotationDegs(rotation);

	// Debug-only
	Bounds().DebugDraw();
}

void Bullet::InitParticles() {
	if (particles.texture != nullptr) {
		return; // It's already been init
	}
	particles.SetTexture(Assets::spritesheetTexture);
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
