#pragma once

#include "jumpman.h"
#include "bullet.h"

template<typename T>
struct BaseEnemy
{
	void TakeDamageFromBullets() {
		T* self = (T*)this;
		for (Bullet* b : Bullet::GetAll()) {
			if (b->explode) continue;
			if (Collide(b->bounds(), self->bounds())) {
				b->boom(self->pos);
				self->takeDamage();
				return;
			}
		}
	}

	void DamagePlayer() {
		T* self = (T*)this;
		JumpMan* player = JumpMan::instance();
		if (!player->isInvencible()) {
			if (Collide(player->bounds(), self->bounds())) {
				player->takeDamage(self->pos);
			}
		}
	}
};

