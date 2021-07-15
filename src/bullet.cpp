#include "bullet.h"

#include "rand.h"
#include "particles.h"
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
	Particles::bullet.pos = pos;
	for (int i = 0; i < 5; i++) {
		auto& p = Particles::bullet.AddParticle();
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

	Particles::bullet.pos = pos + Rand::VecInRange(-4, -4, 4, 4);
	Particles::bullet.Spawn(dt);
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
