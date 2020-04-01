#pragma once

#include "hospital_entity.h"
#include "animation.h"
#include "rand.h"

extern std::vector< std::vector<bool> > passableCleaner;

struct Cleaner : SortedDrawable, EntS<Cleaner>
{
	enum class EntityState
	{
		IDLE,
		MOVING,
	};

	EntityState state;
	EntityDirection dir = EntityDirection::DOWN;
	bool naixement = true;

	float pos_y_spawn;
	SortedDrawable* parent;
	vec oldPos;

	AnimationType animForCleaner(AnimationType anim)
	{
		int animint = int(anim);
		return AnimationType(animint);
	}

	float decisionCounter = 1000.f;
	Cleaner(vec position, SortedDrawable* _parent)
	{
		anim.Ensure(animForCleaner(AnimationType::ROOMBA_DOWN));

		state = EntityState::MOVING;
		oldPos = position;
		pos = position;

		vel.x = 0;
		vel.y = 0;

		parent = _parent;
		pos_y_spawn = pos.y;
		size = vec(13.f,13.f);

		if (_parent == nullptr) {
			timer_naixement = 100000;
		}
	}
	void DecideNextAction() {
		decisionCounter = 1000.f;
		int newdir = Random::roll(2);
		switch (newdir)
		{
		case 0:
			vel.x = 0.05;
			break;
		case 1:
			vel.x = -0.05;
			break;
		}
		newdir = Random::roll(2);
		switch (newdir)
		{
		case 0:
			vel.y = 0.05;
			break;
		case 1:
			vel.y = -0.05;
			break;
		}
	}
	void Move(int dt)
	{

		decisionCounter -= dt;
		if (decisionCounter < 0) {
			DecideNextAction();


		}
		oldPos = pos;



		bool moved = tryMove(dt/4.f) && tryMove(dt / 4.f) && tryMove(dt / 4.f) && tryMove(dt / 4.f);

		if (pos.x > oldPos.x)
		{
			state = EntityState::MOVING;
			dir = EntityDirection::RIGHT;
		}
		else if (pos.x < oldPos.x)
		{
			state = EntityState::MOVING;
			dir = EntityDirection::LEFT;

		}
		else if (pos.y > oldPos.y)
		{
			state = EntityState::MOVING;
			dir = EntityDirection::DOWN;
		}
		else if (pos.y < oldPos.y)
		{
			state = EntityState::MOVING;
			dir = EntityDirection::UP;
		}
		else
		{
			state = EntityState::IDLE;
		}

		if (moved)
		{
			for (Cleaner* p : EntS<Cleaner>::getAll())
			{
				if (p == this) continue;
				if (Collide(p->bounds(),this->bounds()))
				{
					pos = oldPos;
					DecideNextAction();
					break;
				}
			}
		}
	}

	bool tryMove(float dt)
	{
		bool moved = false;

		vec newPos = pos + vel * dt;

		float dd = size.x/2;

		Mates::xy TL_x = PosToTile(vec(newPos.x, pos.y) + vec(-dd, -dd));
		Mates::xy TR_x = PosToTile(vec(newPos.x, pos.y) + vec(dd, -dd));
		Mates::xy BL_x = PosToTile(vec(newPos.x, pos.y) + vec(-dd, dd));
		Mates::xy BR_x = PosToTile(vec(newPos.x, pos.y) + vec(dd, dd));

		Mates::xy TL_y = PosToTile(vec(pos.x, newPos.y) + vec(-dd, -dd));
		Mates::xy TR_y = PosToTile(vec(pos.x, newPos.y) + vec(dd, -dd));
		Mates::xy BL_y = PosToTile(vec(pos.x, newPos.y) + vec(-dd, dd));
		Mates::xy BR_y = PosToTile(vec(pos.x, newPos.y) + vec(dd, dd));

		//Right
		if (vel.x > 0)
		{
			if (passableCleaner[TR_x.x][TR_x.y] && passableCleaner[BR_x.x][BR_x.y])
			{
				pos.x = newPos.x;
				moved = true;
			}
		}

		//Left
		if (vel.x < 0)
		{
			if (passableCleaner[TL_x.x][TL_x.y] && passableCleaner[BL_x.x][BL_x.y])
			{
				pos.x = newPos.x;
				moved = true;
			}
		}

		//Down
		if (vel.y > 0)
		{
			if (passableCleaner[BL_y.x][BL_y.y] && passableCleaner[BR_y.x][BR_y.y])
			{
				pos.y = newPos.y;
				moved = true;
			}
		}

		//Up
		if (vel.y < 0)
		{
			if (passableCleaner[TL_y.x][TL_y.y] && passableCleaner[TR_y.x][TR_y.y])
			{
				pos.y = newPos.y;
				moved = true;
			}
		}


		return moved;

	}

	int timer_naixement = 0;
	bool stuck = false;
	bool ya_va = false;


	int just_absorbed_timer = -1;
	int JUST_ABSORBED_TIMER_MAX = 200;

	void Update(int dt)
	{
		if (just_absorbed_timer > 0)
		{
			just_absorbed_timer -= dt;
		}

		oldPos = pos;
		if (stuck) {
			stuck = false;
			return;
		}
		int oldTimer = timer_naixement;
		timer_naixement += dt;

		if (timer_naixement < 800)
		{
			return;
		}
		else if (timer_naixement < 2400)
		{

			pos.y += dt*0.01f;
			for (Cleaner* p : EntS<Cleaner>::getAll())
			{
				if (p == this) continue;
				if (Collide(p->bounds(), this->bounds()))
				{
					pos = oldPos;
					timer_naixement = oldTimer;
					break;
				}
			}
			return;
		}


		Move(dt);

		switch (state)
		{
			case EntityState::IDLE:
			{

				if (dir == EntityDirection::UP)
				{
					anim.EnsureNoReset(animForCleaner(AnimationType::ROOMBA_UP));
				}
				else if (dir == EntityDirection::DOWN)
				{
					anim.EnsureNoReset(animForCleaner(AnimationType::ROOMBA_DOWN));
				}
				else if (dir == EntityDirection::LEFT)
				{
					anim.EnsureNoReset(animForCleaner(AnimationType::ROOMBA_LEFT));
				}
				else if (dir == EntityDirection::RIGHT)
				{
					anim.EnsureNoReset(animForCleaner(AnimationType::ROOMBA_RIGHT));
				}

			} break;

			case EntityState::MOVING:
			{
				if (dir == EntityDirection::UP)
				{
					anim.EnsureNoReset(animForCleaner(AnimationType::ROOMBA_UP));
				}
				else if (dir == EntityDirection::DOWN)
				{
					anim.EnsureNoReset(animForCleaner(AnimationType::ROOMBA_DOWN));
				}
				else if (dir == EntityDirection::LEFT)
				{
					anim.EnsureNoReset(animForCleaner(AnimationType::ROOMBA_LEFT));
				}
				else if (dir == EntityDirection::RIGHT)
				{
					anim.EnsureNoReset(animForCleaner(AnimationType::ROOMBA_RIGHT));
				}
			} break;
		}
	}

	Bounds bounds()
	 {
		return Bounds(pos, size, true);
	}

	void Draw(sf::Sprite& spr, sf::RenderTarget& window)
	{
		//bounds().Draw(window);

		spr.setPosition(pos.x+1, pos.y+1);

		spr.setTextureRect(anim.CurrentFrame());

		if (just_absorbed_timer > 0)
		{
			float sc = 1.0f + 0.1f*((just_absorbed_timer)/float(JUST_ABSORBED_TIMER_MAX));
			spr.setScale(sc, sc);
		}
		window.draw(spr);
		spr.setScale(1, 1);

		if (timer_naixement < 2400)
		{
			parent->Draw(spr, window);
		}

	}

	void Absorbed()
	{
		if (just_absorbed_timer <= 0)
		{
			just_absorbed_timer = JUST_ABSORBED_TIMER_MAX;
		}
	}

};



struct CleanerSpawner : SortedDrawable, EntS<CleanerSpawner>
{
	enum CleanerSpawnerState
	{
		NORMAL,
		ABRIENDOSE,
		TANCANT
	};

	CleanerSpawnerState state = CleanerSpawnerState::NORMAL;

	CleanerSpawner(vec _pos)
	{
		pos = _pos;

		anim.Ensure(AnimationType::ROOMBA_DOOR_OPEN);


	}
	Bounds bounds() {

		return Bounds(pos.x + (16 - 16) / 2, pos.y + (16 - 16) / 2, 16, 16);
	}
	int timer = 0;

	void Update(int dt)
	{

		switch (state)
		{
		case CleanerSpawnerState::NORMAL:
		{
			//Nothing to do here
		} break;

		case CleanerSpawnerState::ABRIENDOSE:
		{
			anim.Ensure(AnimationType::ROOMBA_DOOR_OPEN);
			anim.loopable = false;

			timer += dt;
			bool haSortit = true;
			for (Cleaner* p : EntS<Cleaner>::getAll())
			{
				if (Collide(p->bounds(), this->bounds()))
				{
					haSortit = false;
					break;
				}
			}
			if (timer > 2600 && haSortit)
			{
				state = CleanerSpawnerState::TANCANT;
				timer = 0;
			}

		} break;

		case CleanerSpawnerState::TANCANT:
		{
			anim.Ensure(AnimationType::ROOMBA_DOOR_CLOSE);
			timer += dt;
			if (timer > 800)
			{
				state = CleanerSpawnerState::NORMAL;
			}

		} break;

		}

	}

	void Draw(sf::Sprite& spr, sf::RenderTarget& window)
	{

		switch (state)
		{
		case CleanerSpawnerState::NORMAL:
		{
			spr.setTextureRect(Animation::AnimFrame(AnimationType::ROOMBA_DOOR_OPEN, 0));
			spr.setPosition(pos);
			window.draw(spr);
		} break;

		default:
		{
			spr.setTextureRect(anim.CurrentFrame());
			spr.setPosition(pos);
			window.draw(spr);
		} break;
		}

	}

	void TreuElGos()
	{
		state = CleanerSpawnerState::ABRIENDOSE;
		anim.Ensure(AnimationType::ROOMBA_DOOR_OPEN);
		anim.Reset();
		new Cleaner(pos,this);
	}

};
