#pragma once

#include "rand.h"
#include "animation.h"
#include "gaem_constants.h"
#include "vec.h"

enum struct MinionShape
{
	CIRCLE = 0,
	BOY,
	SPERM,
	CAPDETRUNYO,
	FADA,
	APPLE,
	MEGAJAM,
	EGGMAN,
	SHAPE_COUNT
};

enum struct MinionState
{
	EGG,
	ALIVE,
	EATING,
	MINING,
	RIP,
};

SDL_Color Genetics_RandomColor(int key)
{
	static bool inited = false;
	static SDL_Color colors[6];
	if (!inited)
	{
		colors[0] = SDL_Color(192, 0, 192, 255);
		colors[1] = SDL_Color(0, 128, 255, 255);
		colors[2] = SDL_Color(255, 128, 0, 255);
		colors[3] = SDL_Color(0, 255, 192, 255);
		colors[4] = SDL_Color(255, 0, 0, 255);
		colors[5] = SDL_Color(255, 230, 0, 255);

		inited = true;
	}

	return colors[key];
}

SDL_Color Genetics_RandomColor()
{
	return Genetics_RandomColor(Rand::roll(0, 5));
}

struct Genetics
{
	SDL_Color color;
	MinionShape shape;

	int walk_speed_delay;
	float mining_multiplier;


	//no vale
	int color_key;
};


AnimationType Minion_AnimForShape(AnimationType type, MinionShape shape)
{
	if (type != AnimationType::IDLE && type != AnimationType::LOVE_MAKING) return type;

	switch (shape)
	{
	case MinionShape::CIRCLE:
		return AnimationType::IDLE_CIRCLE;
		break;
	case MinionShape::BOY:
		return AnimationType::IDLE_BOY;
		break;
	case MinionShape::SPERM:
		return AnimationType::IDLE_SPERM;
		break;
	case MinionShape::CAPDETRUNYO:
		return AnimationType::IDLE_CAPDETRUNYO;
		break;
	case MinionShape::FADA:
		return AnimationType::IDLE_FADA;
		break;
	case MinionShape::APPLE:
		return AnimationType::IDLE_APPLE;
		break;
	case MinionShape::MEGAJAM:
		return AnimationType::IDLE_MEGAJAM;
		break;
	case MinionShape::EGGMAN:
		return AnimationType::IDLE_EGGMAN;
		break;
	default:
		return type;
		break;
	}
}

struct Minion
{
	MinionState state = MinionState::EGG;
	AnimationType anim = AnimationType::EGG_SHELL;
	int lifetime = 0;
	int satiation = 10000;
	int eggtime = EGG_HATCH_TIME + Rand::roll(EGG_HATCH_TIME_RAND);
	int wait_delay = 0;
	int timer = 0;

	veci pos;
	veci pos_target;

	Genetics gens;

	Minion(veci p)
		: pos(p)
	{
		gens.color_key = Rand::roll(6);
		gens.color = Genetics_RandomColor(gens.color_key);
		gens.shape = (MinionShape)Rand::roll((int)MinionShape::SHAPE_COUNT - 1);
		gens.walk_speed_delay = Rand::roll(MIN_WALK_SPEED_DELAY, MAX_WALK_SPEED_DELAY);
		gens.mining_multiplier = Rand::rollf(MIN_MINING_MULTIPLIER, MAX_MINING_MULTIPLIER);
	}

	void generateGenetics(Minion* parent1, Minion* parent2)
	{
		bool flip = Rand::OnceEvery(2);
		if (flip)
		{
			gens.color = parent1->gens.color;
			gens.shape = parent2->gens.shape;
		}
		else
		{
			gens.color = parent2->gens.color;
			gens.shape = parent1->gens.shape;
		}


		bool flip2 = Rand::OnceEvery(2);
		bool flip3 = Rand::OnceEvery(2);

		if (flip2 && flip3)
		{
			gens.color = Genetics_RandomColor();
		}

		gens.walk_speed_delay = Rand::roll(
			std::min(parent1->gens.walk_speed_delay, parent2->gens.walk_speed_delay),
			std::max(parent1->gens.walk_speed_delay, parent2->gens.walk_speed_delay)
		);
		gens.mining_multiplier = Rand::rollf(
			std::min(parent1->gens.mining_multiplier, parent2->gens.mining_multiplier),
			std::max(parent1->gens.mining_multiplier, parent2->gens.mining_multiplier)
		);
	}

};
