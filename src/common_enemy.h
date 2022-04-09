#pragma once

#include "jumpman.h"
#include "bullet.h"
#include "collide.h"
#include "health.h"
#include "rand.h"
#include "oneshotanim.h"
#include "anim_lib.h"
#include "fx.h"
#include "bounds.h"
#include "assets.h"

// returns the position of the attack if damaged or null otherwise. don't store the returned pointer for longer than this frame
template<typename B>
const vec* ReceiveDamageFromPlayer(const B& bounds, bool enemyInvulnerable) {
	for (Bullet* b : Bullet::GetAll()) {
		if (!b->alive) continue;
		if (Collide(b->Bounds(), bounds)) {
			b->Explode();
			if (enemyInvulnerable) return nullptr;
			return &(b->pos);
		}
	}

	JumpMan* player = JumpMan::instance();
	if (player->playerAttack.alive) {
		if (Collide(player->playerAttack.Bounds(), bounds)) {
			player->DealDamage(bounds.Center());
			if (enemyInvulnerable) return nullptr;
			return &(player->playerAttack.pos);
		}
	}
	return nullptr;
}

template<typename B>
bool DamagePlayerOnCollision(const B& bounds) { // returns true if collided
	JumpMan* player = JumpMan::instance();
	if (Collide(player->HitBoxBounds(), bounds)) {
		if (!player->isInvencible()) {
			player->TakeDamage(bounds.Center());
		}
		return true;
	}
	return false;
}

inline void RandomlySpawnHealth(vec pos, int percentChance = 16) {
	if (Rand::PercentChance(percentChance)) {
		new Health(pos + Rand::VecInRange(-6, -6, 6, 6));
	}
}

inline void DieScreenShake() {
	Fx::FreezeImage::Freeze(0.07f, true);
	Fx::Screenshake::Start(0.4f, vec(4.f, 4.f), vec(18.f, 20.f), 0.85f);
}

inline void DieWithSmallExplosion(Entity* e) {
	e->alive = false;
	DieScreenShake();
	new BackgroundOneShotAnim(Assets::spritesheetTexture, e->pos, AnimLib::MAGIC_EXPLOSION, 1.3f);
	RandomlySpawnHealth(e->pos);
}

template<typename BoundsIterable>
inline int FindIndexOfSmallestBoundsContaining(vec pos, const BoundsIterable& bounds_array) {
	int smallest_i = -1;
	float smallest_area = Mates::MaxFloat;
	int i = 0;
	for (const auto& bounds : bounds_array) {
		if (bounds.Contains(pos)) {
			float area = bounds.Area();
			if (area < smallest_area) {
				smallest_i = i;
				smallest_area = area;
			}
		}
		i++;
	}
	return smallest_i;
}

inline void DrawBossHealth(int current, int max, int offset)
{
	// TODO: Flash/animation/particles when health goes down

	const vec pos(6, 28 + 19 * offset);
	const int size = 8 * max;
	const float scale = 1.5f;

	// Background
	Window::Draw(Assets::spritesheetTexture, pos + vec(2 * scale, 0))
		.withScale(size * scale, scale)
		.withRect(480, 260, 1, 9);

	//Health
	Window::Draw(Assets::spritesheetTexture, pos + vec(2 * scale, 0))
		.withScale((float(current) / max) * size * scale, scale)
		.withRect(478, 260, 1, 9);

	// Bar begin
	Window::Draw(Assets::spritesheetTexture, pos + vec())
		.withScale(scale)
		.withRect(463, 260, 4, 9);

	// Bar
	Window::Draw(Assets::spritesheetTexture, pos + vec(2 * scale, 0))
		.withScale(size * scale, scale)
		.withRect(466, 260, 1, 9);

	// Bar end
	Window::Draw(Assets::spritesheetTexture, pos + vec(size * scale, 0))
		.withScale(scale)
		.withRect(473, 260, 4, 9);

}
