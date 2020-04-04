#pragma once

#include "entity.h"
#include "animation.h"
#include "selfregister.h"

// Entities specific to the hospital gaem

struct HospitalEntity : BoxEntity, EntS<HospitalEntity>
{
	Animation anim;
	HospitalEntity() : BoxEntity(vec(), vec()) { }
	virtual void Update(int dt) { }
	void UpdateAnim(int dt) {
		if (anim.anim_type != AnimationType::NADA) {
			anim.Update(dt);
		}
	}
};

struct SortedDrawable : HospitalEntity, EntS<SortedDrawable>
{
	virtual void Draw(sf::Sprite& spr, sf::RenderTarget& window) { }
};

struct Buttonable
{
	virtual void Operate() = 0;
};

enum class EntityDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};

const float CINTA_SPEED = 8 / 320.f;

inline vec DirToVec(EntityDirection dir) {
	switch (dir) {
	case EntityDirection::RIGHT: return vec(1, 0);
	case EntityDirection::LEFT: return vec(-1, 0);
	case EntityDirection::UP: return vec(0, -1);
	case EntityDirection::DOWN: return vec(0, 1);
	default: return vec(0, 0);
	}
}
struct Cintable : EntS<Cintable>
{
	EntityDirection prevCintaDirection = EntityDirection::NONE;
	EntityDirection currCintaDirection = EntityDirection::NONE;

	virtual vec positionPlz() = 0;
	virtual vec sizePlz() = 0;

	bool SetVelWithCinta(vec& vel)
	{
		bool onCinta = false;
		if (currCintaDirection != EntityDirection::NONE)
		{
			vel.x += (EntityDirection::RIGHT == currCintaDirection) * CINTA_SPEED;
			vel.x -= (EntityDirection::LEFT == currCintaDirection) * CINTA_SPEED;
			vel.y -= (EntityDirection::UP == currCintaDirection) * CINTA_SPEED;
			vel.y += (EntityDirection::DOWN == currCintaDirection) * CINTA_SPEED;
			onCinta = true;
		}
		prevCintaDirection = currCintaDirection;
		currCintaDirection = EntityDirection::NONE;
		return onCinta;
	}

};
