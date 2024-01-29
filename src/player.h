#pragma once

#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "cleaner.h"

#include "input.h"

#include "cadaver.h"
#include "mesa.h"

extern std::vector< std::vector<bool> > passable;

struct Bullet : Entity, SelfRegister<Bullet>
{
	bool explode = false;
	int timer_explosion = 0;

	Bullet(vec position, vec velocity) {
		pos = position;
		speed = velocity;
		size = vec(1,1);
	}

	void Update(int dt)
	{

		auto tile = PosToTile(pos);
		if (!passableCleaner[tile.x][tile.y]) {
			alive = false;
		}

		if (explode) {
			speed = vec(0,0);
			timer_explosion += dt;
			if (timer_explosion > 1000) {
				alive = false;
			}
			return;
		}

		pos += speed * dt * 0.1f;
		if (IsOutOfScreen(pos)) {
			alive = false;
		}
	}

	void Draw()
	{
		int frame = 0;
		if (timer_explosion > 0) {
			frame += (timer_explosion*6)/1000;
		}
		Window::Draw(Assets::sprite, pos).withRect((9+frame) * 16, 10 * 16, 16, 16).withOrigin(8,8);
	}
};

struct Player : SortedDrawable, SelfRegister<Player>
{
	Animation actionButton;

	int player;
	bool isCarrying;

	bool isPullingLever = false;

	Extremity* extremity = nullptr;
	Cadaver* cadaver = nullptr;
	Mesa* mesa = nullptr;
	Lever* lever = nullptr;
	Collector* collector = nullptr;
	vec sizeToCollideWithTilemap;
	Player(int id, vec position)
	{
		player = id;

		isCarrying = false;

		anim.Ensure((AnimationType::PLAYER_IDLE_DOWN));
		actionButton.Ensure(AnimationType::BUTTON_A_PRESS);

		state = EntityState::MOVING;

		pos = position;
		speed.x = 0;
		speed.y = 0;
		sizeToCollideWithTilemap = vec(10.f, 10.f);
		size = vec(16.f,16.f); //Bigger than the previous one so we can interact with objects further away
	}
	void SetSpeedWithPlayerInput()
	{
		float deadZone = 20;
		vec anal = Input::GetAnalog(player, AnalogInput::MOVE);

		speed = anal * 0.0008f;

		if (anal.x > deadZone)
		{
			state = EntityState::MOVING;
			dir = EntityDirection::RIGHT;
		}
		else if (anal.x < -deadZone)
		{
			state = EntityState::MOVING;
			dir = EntityDirection::LEFT;

		}
		else if (anal.y > deadZone)
		{
			state = EntityState::MOVING;
			dir = EntityDirection::DOWN;
		}
		else if (anal.y < -deadZone)
		{
			state = EntityState::MOVING;
			dir = EntityDirection::UP;
		}
		else
		{
			state = EntityState::IDLE;
		}
	}

	void ActionButtonJustPressed()
	{

		// Poner cadaver en mesa
		if (isCarrying && cadaver && mesa && !mesa->cadaver)
		{
			isCarrying = false;
			mesa->cadaver = cadaver;
			cadaver->putCadaverOnTable(mesa->pos);
			cadaver = NULL;
			return;
		}

		// Sacar cadaver de mesa
		if (!isCarrying && mesa && mesa->cadaver)
		{
			isCarrying = true;
			cadaver = mesa->cadaver;
			mesa->cadaver = NULL;
			cadaver->isLet = false;
			cadaver->carryCadaver(pos.x, pos.y);
			extremity = NULL;
			return;
		}

		// Poner extremity en collector
		if (isCarrying && extremity && collector && !collector->extremity)
		{
			extremity->isLet = true;
			extremity->pos = collector->pos + vec(2,2);
			collector->extremity = extremity;
			extremity->isCarried = false;
			extremity = nullptr;
			isCarrying = false;
			return;
		}
		// Sacar extremity de collector
		if (!isCarrying && collector && collector->extremity)
		{
			extremity = collector->extremity;
			extremity->isLet = false;
			collector->extremity = nullptr;
			extremity->isCarried = true;
			isCarrying = true;
			extremity->carryExtremity(pos.x, pos.y);
			cadaver = NULL;
			return;
		}


		// Coger extremity del suelo
		if (!isCarrying && extremity)
		{
			isCarrying = true;
			extremity->isCarried = true;
			extremity->pos.x = pos.x;
			extremity->pos.y = pos.y;
			cadaver = NULL;
			return;
		}

		// Coger cadaver del suelo
		if (!isCarrying && cadaver)
		{
			isCarrying = true;
			cadaver->carryCadaver(pos.x, pos.y);
			extremity = NULL;
			return;
		}

		// Dejar extremity en suelo
		if (isCarrying && extremity)
		{
			isCarrying = false;
			extremity->isCarried = false;
			extremity = NULL;
			return;
		}

		// Dejar cadaver en suelo
		 if (isCarrying && cadaver != NULL)
		 {
			isCarrying = false;
			cadaver->isCarried = false;
			cadaver->pos.y -= 3;
			cadaver->pos.x -= 8;
			cadaver->pos.x = round(cadaver->pos.x / 16.f )*16 + 8;
			cadaver = NULL;
			return;
		 }

		 // Boton
		 if (!isCarrying && lever)
		 {
			 lever->Push();
			 return;
		 }
	}

	void ActionButtonHold()
	{
		// Boton
		if (!isCarrying && lever)
		{
			lever->Hold();
			return;
		}
	}

	bool tryMove(float dt)
	{
		bool moved = false;

		vec newPos = pos + speed * dt;

		float dd = sizeToCollideWithTilemap.x/2;

		veci TL_x = PosToTile(vec(newPos.x, pos.y) + vec(-dd, -dd));
		veci TR_x = PosToTile(vec(newPos.x, pos.y) + vec(dd, -dd));
		veci BL_x = PosToTile(vec(newPos.x, pos.y) + vec(-dd, dd));
		veci BR_x = PosToTile(vec(newPos.x, pos.y) + vec(dd, dd));

		veci TL_y = PosToTile(vec(pos.x, newPos.y) + vec(-dd, -dd));
		veci TR_y = PosToTile(vec(pos.x, newPos.y) + vec(dd, -dd));
		veci BL_y = PosToTile(vec(pos.x, newPos.y) + vec(-dd, dd));
		veci BR_y = PosToTile(vec(pos.x, newPos.y) + vec(dd, dd));

		//Right
		if (speed.x > 0)
		{
			if (passable[TR_x.x][TR_x.y] && passable[BR_x.x][BR_x.y])
			{
				pos.x = newPos.x;
				moved = true;
			}
		}

		//Left
		if (speed.x < 0)
		{
			if (passable[TL_x.x][TL_x.y] && passable[BL_x.x][BL_x.y])
			{
				pos.x = newPos.x;
				moved = true;
			}
		}

		//Down
		if (speed.y > 0)
		{
			if (passable[BL_y.x][BL_y.y] && passable[BR_y.x][BR_y.y])
			{
				pos.y = newPos.y;
				moved = true;
			}
		}

		//Up
		if (speed.y < 0)
		{
			if (passable[TL_y.x][TL_y.y] && passable[TR_y.x][TR_y.y])
			{
				pos.y = newPos.y;
				moved = true;
			}
		}


		return moved;

	}

	void Move(int dt) {

		auto oldPos = pos;

		bool moved = tryMove(dt / 4.f) && tryMove(dt / 4.f) && tryMove(dt / 4.f) && tryMove(dt / 4.f);

		if (moved)
		{
			for (Player* p : SelfRegister<Player>::GetAll())
			{
				if (p == this) continue;
				if (Collide(p->bounds(), this->bounds()))
				{
					pos = oldPos;
					break;
				}
			}
			for (Cleaner* p : SelfRegister<Cleaner>::GetAll())
			{
				if (Collide(p->bounds(), this->bounds()))
				{
					pos = oldPos;
					break;
				}
			}
		}

		switch (state)
		{
		case EntityState::IDLE:
		{

			if (dir == EntityDirection::UP)
			{
				anim.Ensure((AnimationType::PLAYER_IDLE_UP));
			}
			if (dir == EntityDirection::DOWN)
			{
				anim.Ensure((AnimationType::PLAYER_IDLE_DOWN));
			}
			if (dir == EntityDirection::LEFT)
			{
				anim.Ensure((AnimationType::PLAYER_IDLE_LEFT));
			}
			if (dir == EntityDirection::RIGHT)
			{
				anim.Ensure((AnimationType::PLAYER_IDLE_RIGHT));
			}

		} break;

		case EntityState::MOVING:
		{
			if (dir == EntityDirection::UP)
			{
				anim.Ensure((AnimationType::PLAYER_WALKING_UP));
			}
			if (dir == EntityDirection::DOWN)
			{
				anim.Ensure((AnimationType::PLAYER_WALKING_DOWN));
			}
			if (dir == EntityDirection::LEFT)
			{
				anim.Ensure((AnimationType::PLAYER_WALKING_LEFT));
			}
			if (dir == EntityDirection::RIGHT)
			{
				anim.Ensure((AnimationType::PLAYER_WALKING_RIGHT));
			}
		} break;
		}
	}

	void Update(int dt)
	{
		SetSpeedWithPlayerInput();
		//SetSpeedWithCinta(speed);

		if (Input::IsJustPressed(player, GameKeys::ACTION))
		{
			ActionButtonJustPressed();
		}

		if (Input::IsPressed(player, GameKeys::ACTION))
		{
			ActionButtonHold();
		}

		//if (Input::IsJustPressed(player, GameKeys::SHOOT)) {
		//	new Bullet(pos, DirToVec(dir));
		//}

		Move(dt);

		if (isCarrying) {
			if (extremity != NULL)
			{
				extremity->carryExtremity(pos.x, pos.y);
			}
			if (cadaver != NULL)
			{
				cadaver->carryCadaver(pos.x, pos.y);
			}
		}

		actionButton.Update(dt);
	}

	BoxBounds bounds() {
		return BoxBounds::FromCenter(pos, sizeToCollideWithTilemap);
	}

	void Draw()
	{
		//bounds().Draw(window, SDL_Color::Magenta);
		//BoxBounds(pos, size, true).Draw(window);

		auto frame = anim.CurrentFrame();
		frame.x += 4 * 16 * player; //offset

		Window::Draw(Assets::sprite, vec(pos.x + 5.f, pos.y - 5.f)).withRect(frame).withScale(1.25f).withOrigin(8,8);

		if (((cadaver || extremity) && !isCarrying) || (lever && lever->is_connected) || (isCarrying && cadaver && mesa && !mesa->cadaver) || (collector && collector->extremity) || (isCarrying && extremity && collector && !collector->extremity))
		{
			// Action button
			Window::Draw(Assets::sprite, vec(pos.x, pos.y - 15.f)).withRect(actionButton.CurrentFrame()).withOrigin(8,8);
		}
		else
		{
			actionButton.Reset();
		}

	}

};

