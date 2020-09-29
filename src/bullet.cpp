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

const float kExplosionTime = 0.3f;

void Bullet::Update(float dt)
{
	if (explode) {
		vel = vec(0,0);
		timer_explosion += dt;
		if (timer_explosion > kExplosionTime) {
			alive = false;
		}
		return;
	}

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

	TileMap* map = TileMap::instance();
	vec toTheOutside = vel.Perp().Normalized()* radius * 0.85f;
	//(pos + toTheOutside).Debuggerino(sf::Color::White);
	//(pos - toTheOutside).Debuggerino(sf::Color::White);
	veci t = map->toTiles(pos+ toTheOutside);
	Tile tile = map->getTile(t);
	if (!tile.isFullSolid()) {
		t = map->toTiles(pos - toTheOutside);
		tile = map->getTile(t);
	}
	if (tile.isFullSolid()) {
		if (tile.isBreakable() && SkillTree::instance()->IsEnabled(Skill::BREAK)) {
			DestroyedTiles::instance()->Destroy(t.x, t.y);
		}
		Bat::AwakeNearbyBats(pos);
		Bullet::particles.pos = pos;
		for (int i = 0; i < 5; i++) {
			auto& p = Bullet::particles.AddParticle();
			p.scale = 1.7f;
			p.vel *= 1.5f;
		}
		alive = false;
		return;
	}

	Bullet::particles.pos = pos + Rand::vecInRange(-4, -4, 4, 4);
	Bullet::particles.Spawn(dt);
}

void Bullet::Draw() const
{
	vec drawPos = pos;
	float rotation = 0.f;
	GPU_Rect rect;

	if (!explode) {
		rect = { 8 * 16, 10 * 16, 16, 16 };
		rotation = Rand::roll(0, 360);
		drawPos += Rand::vecInRange(-1, -1, 1, 1);
	}
	else {
		int frame = (7 * timer_explosion / kExplosionTime);
		rect = { (9 + frame) * 16.f, 10 * 16.f, 16.f, 16.f };
	}
	Window::Draw(Assets::hospitalTexture, drawPos)
		.withScale(scale)
		.withOrigin(8, 8)
		.withRect(rect)
		.withRotationDegs(rotation);

}

void Bullet::InitParticles() {
	if (particles.texture != nullptr) {
		return;
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
