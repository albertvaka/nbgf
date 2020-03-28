#pragma once

#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "cleaner.h"

#include "input.h"

#include "cadaver.h"
#include "mesa.h"
#include "bullet.h"

extern std::vector< std::vector<bool> > passable;

struct Player : SortedDrawable, EntS<Player>
{
	enum class EntityState
	{
		IDLE,
		MOVING,
	};

	EntityState state;
	EntityDirection dir = EntityDirection::DOWN;

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
		sf::Vector2f anal = vec(GamePad::AnalogStick::Left.get(player, deadZone));

		//Player 0 can move with keyboard
		if (player == 3)
		{
			if (Keyboard::IsKeyPressed(GameKeys::UP))
			{
				anal.y = -100;
			}
			else if (Keyboard::IsKeyPressed(GameKeys::DOWN))
			{
				anal.y = 100;
			}
			if (Keyboard::IsKeyPressed(GameKeys::RIGHT))
			{
				anal.x = 100;
			}
			else if (Keyboard::IsKeyPressed(GameKeys::LEFT))
			{
				anal.x = -100;
			}
		}


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

		Mates::xy TL_x = PosToTile(vec(newPos.x, pos.y) + vec(-dd, -dd));
		Mates::xy TR_x = PosToTile(vec(newPos.x, pos.y) + vec(dd, -dd));
		Mates::xy BL_x = PosToTile(vec(newPos.x, pos.y) + vec(-dd, dd));
		Mates::xy BR_x = PosToTile(vec(newPos.x, pos.y) + vec(dd, dd));

		Mates::xy TL_y = PosToTile(vec(pos.x, newPos.y) + vec(-dd, -dd));
		Mates::xy TR_y = PosToTile(vec(pos.x, newPos.y) + vec(dd, -dd));
		Mates::xy BL_y = PosToTile(vec(pos.x, newPos.y) + vec(-dd, dd));
		Mates::xy BR_y = PosToTile(vec(pos.x, newPos.y) + vec(dd, dd));

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
			for (Player* p : EntS<Player>::getAll())
			{
				if (p == this) continue;
				if (Collide(p->bounds(), this->bounds()))
				{
					pos = oldPos;
					break;
				}
			}
			for (Cleaner* p : EntS<Cleaner>::getAll())
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

		if ((Keyboard::IsKeyJustPressed(GameKeys::ACTION) && player == 3) || GamePad::IsButtonJustPressed(player, GamePad::Button::A))
		{
			ActionButtonJustPressed();
		}

		if ((Keyboard::IsKeyPressed(GameKeys::ACTION) && player == 3) || GamePad::IsButtonPressed(player, GamePad::Button::A))
		{
			ActionButtonHold();
		}

		if (Keyboard::IsKeyJustPressed(GameKeys::SHOOT)) {
			new Bullet(pos, DirToVec(dir)*100.f);
		}
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

	Bounds bounds() {
		return Bounds(pos, sizeToCollideWithTilemap, true);
	}

	void Draw(sf::Sprite& spr, sf::RenderTarget& window)
	{
		//bounds().Draw(window, sf::Color::Magenta);
		//Bounds(pos, size, true).Draw(window);

		auto a = spr.getScale();
		spr.setScale(1.25, 1.25);
		spr.setPosition(pos.x + 5.f, pos.y - 5.f);

		auto frame = anim.CurrentFrame();
		frame.left += 4 * 16 * player; //offset
		spr.setTextureRect(frame);

		window.draw(spr);
		spr.setScale(a);

		if (((cadaver || extremity) && !isCarrying) || (lever && lever->is_connected) || (isCarrying && cadaver && mesa && !mesa->cadaver) || (collector && collector->extremity) || (isCarrying && extremity && collector && !collector->extremity))
		{
			// Action button
			spr.setTextureRect(actionButton.CurrentFrame());
			spr.setPosition(pos.x, pos.y - 15);
			window.draw(spr);
		}
		else
		{
			actionButton.Reset();
		}

	}


	void Draw(sf::VertexArray &vertexArray)
	{
		float x = pos.x / 100.0f;
		float y = pos.y / 100.0f;
		x = x - 8;
		y = y - 18;
		sf::IntRect rect = anim.CurrentFrame();
		sf::Vector2u posStartInSpritesheet(rect.left,rect.top);
		sf::Vector2u sizeSprite(rect.width,rect.height);
		sf::Vector2u scale(5, 5);



		vertexArray.append(sf::Vertex(sf::Vector2f(x, y), sf::Vector2f(posStartInSpritesheet.x, posStartInSpritesheet.y)));
		vertexArray.append(sf::Vertex(sf::Vector2f(x + sizeSprite.x*scale.x, y), sf::Vector2f(posStartInSpritesheet.x + sizeSprite.x, posStartInSpritesheet.y)));
		vertexArray.append(sf::Vertex(sf::Vector2f(x + sizeSprite.x*scale.x, y + sizeSprite.y*scale.y), sf::Vector2f(posStartInSpritesheet.x + sizeSprite.x, posStartInSpritesheet.y + sizeSprite.y)));
		vertexArray.append(sf::Vertex(sf::Vector2f(x, y + sizeSprite.y*scale.y), sf::Vector2f(posStartInSpritesheet.x, posStartInSpritesheet.y + sizeSprite.y)));



		//window.draw(spr);
	}

};

