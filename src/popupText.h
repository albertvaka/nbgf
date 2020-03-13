#pragma once

#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "vector.h"

struct TextMolest : Entity, EntS<TextMolest>
{
	enum Type
	{
		GOOD,
		BAD
	};

	int timer = 0;
	Type m_type = BAD;

	TextMolest(vec position, Type type)
	{
		pos = vec(16*13.5f, 16*14);
		m_type = type;

		if (type == BAD)
		{
			pos.x = 16 * 11.5f;
		}

		speed.x = 0;// 16 * 2.5f;
		speed.y = -16* 1.8f;
	}

	bool rotatzione = false;
	int descansito_rot = 0;

	void Update(int dt)
	{
		timer += dt;

		pos += (dt/1000.0f) * speed;

		if (timer > 22000)
		{
			alive = false;
		}
		else if (timer > 9000)
		{
			speed.y = 16 * 3.0f;
			descansito_rot += 2 * dt;
		} else if (timer > 5800)
		{
			speed = vec(0, 0);
			rotatzione = true;
			descansito_rot += dt;
		}
	}

	void Draw(sf::Sprite& spr, sf::RenderTarget& window)
	{
		float scx = 4 + cos(timer/200.0f)*2;
		float scy = 4 + sin(timer/200.0f)*2;

		float rot = sin(timer / 100.0f) * 5;

		if (m_type == GOOD)
		{
			spr.setOrigin(16 * 2.5f, 8);
		} 
		else if (m_type == BAD)
		{
			spr.setOrigin(16 * 1.5f, 8);
		}


		spr.setScale(scx, scy);


		if (rotatzione)
		{
			rot = sin(5800 / 100.0f) * 5 + (360.0f * descansito_rot/3000.0f);
		}

		spr.setRotation(rot);


		switch (m_type)
		{
			case Type::GOOD:
			{
				spr.setTextureRect(sf::IntRect(128, 192, 16*4, 16));

				spr.setPosition(pos);
				window.draw(spr);
			} break;

			case Type::BAD:
			{
				spr.setTextureRect(sf::IntRect(128, 192+16, 16 * 3, 16));

				//spr.setTextureRect(anim.CurrentFrame());
				spr.setPosition(pos);
				window.draw(spr);
			} break;
		}

		spr.setScale(1, 1);
		spr.setOrigin(8, 8);
		spr.setRotation(0);
	}

};
