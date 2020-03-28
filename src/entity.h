#pragma once

#include "mates.h"
#include "selfregister.h"
#include "animation.h"

enum class EntityState
{
	IDLE,
	MOVING,
};

enum class EntityDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
};

const float CINTA_SPEED = 8/320.f;

inline vec DirToVec(EntityDirection dir) {
	switch(dir) {
		case EntityDirection::RIGHT: return vec(1,0);
		case EntityDirection::LEFT: return vec(-1,0);
		case EntityDirection::UP: return vec(0,-1);
		case EntityDirection::DOWN: return vec(0,1);
		default: return vec(0,0);
	}
}

struct Entity : EntS<Entity>
{
	EntityState state;
	vec pos;
	vec speed;
	vec size = vec(16, 16);
	int timer = 0;
	Animation anim;
	bool alive = true;
	//Entity* parent = NULL;
	EntityDirection dir = EntityDirection::DOWN;

	virtual void Update(int dt) { }
};

struct Buttonable
{
	virtual void Operate() = 0;
};

struct SortedDrawable : Entity, EntS<SortedDrawable>
{
	virtual void Draw(sf::Sprite& spr, sf::RenderTarget& window) { }
};

struct Cintable : EntS<Cintable>
{
	EntityDirection prevCintaDirection = EntityDirection::NONE;
	EntityDirection currCintaDirection = EntityDirection::NONE;

	virtual vec positionPlz() = 0;
	virtual vec sizePlz() = 0;

	bool SetSpeedWithCinta(vec& speed)
	{
		bool onCinta = false;
		if (currCintaDirection != EntityDirection::NONE)
		{
			speed.x += (EntityDirection::RIGHT == currCintaDirection) * CINTA_SPEED;
			speed.x -= (EntityDirection::LEFT == currCintaDirection) * CINTA_SPEED;
			speed.y -= (EntityDirection::UP == currCintaDirection) * CINTA_SPEED;
			speed.y += (EntityDirection::DOWN == currCintaDirection) * CINTA_SPEED;
			onCinta = true;
		}
		prevCintaDirection = currCintaDirection;
		currCintaDirection = EntityDirection::NONE;
		return onCinta;
	}

};
