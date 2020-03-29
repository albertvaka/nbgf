#pragma once

#include "entity.h"

enum class ExtremityType
{
	BODY,
	RIGHT_LEG,
	LEFT_LEG,
	RIGHT_ARM,
	LEFT_ARM,
	HEAD,
};

enum class ExtremityColor
{
	NONE_COLOR = 0,
	WHITE,
	BLACK,
	YELLOW,
	GREEN,
	BLUE,
	RED,
};

inline ExtremityType RandomExtremityType() {

	return ExtremityType(Random::roll(magic_enum::enum_count<ExtremityType>() - 2) + 1);
};

inline ExtremityColor RandomExtremityColor() {
	return ExtremityColor(Random::roll(magic_enum::enum_count<ExtremityColor>() - 2) + 1);
};

std::map<ExtremityColor, std::map<ExtremityType, sf::IntRect>> extremitySprPos;

struct Extremity : SortedDrawable, Cintable, EntS<Extremity>
{
	ExtremityType type;
	ExtremityColor color;
	bool isCarried = false;
	bool isLet = false;

	vec positionPlz() override { return pos+vec(4,4); }
	vec sizePlz() override { return size; }

	Extremity(vec position, ExtremityType t, ExtremityColor c)
	{
		size = vec(8,8);
		color = c;
		type = t;
		pos = position+vec(1,1);
	}

	void carryExtremity(float x, float y)
	{
		isCarried = true;

		pos.x = x+4;
		pos.y = y+1;
	}


	void Update(int dt) override
	{
		SetVelWithCinta(vel);
		pos += vel * dt;
		vel.Zero();
	}


	void Draw(sf::Sprite& spr, sf::RenderTarget& window) override
	{

		//Bounds(positionPlz(),sizePlz()).Draw(window);
		vec carriedOffset;
		if (isCarried)
		{
			carriedOffset.y = -2;
		}
		spr.setPosition(pos+Offset()+carriedOffset);
		spr.setTextureRect(extremitySprPos[color][type]);



		{
			spr.setColor(sf::Color::Black);
			spr.move(0, 1);
			window.draw(spr);

			spr.move(-1, -1);
			window.draw(spr);

			spr.move(2, 0);
			window.draw(spr);

			spr.move(-1, -1);
			window.draw(spr);

			spr.move(0, 1);
			spr.setColor(sf::Color::White);
		}


		window.draw(spr);


		if (!isLet)
		{
			// Tarrito de mermelada
			spr.setPosition(pos+carriedOffset);
			spr.setTextureRect(sf::IntRect(54, 86, 12, 12));
			window.draw(spr);
		}

		//Bounds(pos.x , pos.y , 16, 16).Draw(window);
		//Bounds(pos.x - 1, pos.y - 1, 2, 2).Draw(window);

	}

	vec Offset()
	{
		if (type == ExtremityType::HEAD)
		{
			return vec(2,3);
		}
		if (type == ExtremityType::LEFT_ARM)
		{
			return vec(4, 1);
		}
		if (type == ExtremityType::LEFT_LEG)
		{
			return vec(3,1);
		}
		if (type == ExtremityType::RIGHT_ARM)
		{
			return vec(0,1);
		}
		if (type == ExtremityType::RIGHT_LEG)
		{
			return vec(1,1);
		}
		return vec(0, 0);
	}

};

void loadExtremityMap()
{
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::RIGHT_LEG] = sf::IntRect(0, 64, 8, 8);
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::LEFT_LEG] =sf::IntRect(8, 64, 8, 8);
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::RIGHT_ARM] = sf::IntRect(16, 64, 8, 8);
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::LEFT_ARM] = sf::IntRect(24, 64, 8, 8);
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::HEAD] = sf::IntRect(32, 64, 8, 8);
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::BODY] = sf::IntRect(40, 64, 8, 8);

	extremitySprPos[ExtremityColor::BLACK][ExtremityType::RIGHT_LEG] = sf::IntRect(0, 72, 8, 8);
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::LEFT_LEG] = sf::IntRect(8, 72, 8, 8);
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::RIGHT_ARM] = sf::IntRect(16, 72, 8, 8);
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::LEFT_ARM] = sf::IntRect(24, 72, 8, 8);
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::HEAD] = sf::IntRect(32, 72, 8, 8);
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::BODY] = sf::IntRect(40, 72, 8, 8);

	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::RIGHT_LEG] = sf::IntRect(0, 80, 8, 8);
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::LEFT_LEG] = sf::IntRect(8, 80, 8, 8);
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::RIGHT_ARM] = sf::IntRect(16, 80, 8, 8);
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::LEFT_ARM] = sf::IntRect(24, 80, 8, 8);
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::HEAD] = sf::IntRect(32, 80, 8, 8);
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::BODY] = sf::IntRect(40, 80, 8, 8);

	extremitySprPos[ExtremityColor::GREEN][ExtremityType::RIGHT_LEG] = sf::IntRect(0, 88, 8, 8);
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::LEFT_LEG] = sf::IntRect(8, 88, 8, 8);
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::RIGHT_ARM] = sf::IntRect(16, 88, 8, 8);
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::LEFT_ARM] = sf::IntRect(24, 88, 8, 8);
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::HEAD] = sf::IntRect(32, 88, 8, 8);
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::BODY] = sf::IntRect(40, 88, 8, 8);

	extremitySprPos[ExtremityColor::BLUE][ExtremityType::RIGHT_LEG] = sf::IntRect(0, 96, 8, 8);
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::LEFT_LEG] = sf::IntRect(8, 96, 8, 8);
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::RIGHT_ARM] = sf::IntRect(16, 96, 8, 8);
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::LEFT_ARM] = sf::IntRect(24, 96, 8, 8);
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::HEAD] = sf::IntRect(32, 96, 8, 8);
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::BODY] = sf::IntRect(40, 96, 8, 8);

	extremitySprPos[ExtremityColor::RED][ExtremityType::RIGHT_LEG] = sf::IntRect(0, 104, 8, 8);
	extremitySprPos[ExtremityColor::RED][ExtremityType::LEFT_LEG] = sf::IntRect(8, 104, 8, 8);
	extremitySprPos[ExtremityColor::RED][ExtremityType::RIGHT_ARM] = sf::IntRect(16, 104, 8, 8);
	extremitySprPos[ExtremityColor::RED][ExtremityType::LEFT_ARM] = sf::IntRect(24, 104, 8, 8);
	extremitySprPos[ExtremityColor::RED][ExtremityType::HEAD] = sf::IntRect(32, 104, 8, 8);
	extremitySprPos[ExtremityColor::RED][ExtremityType::BODY] = sf::IntRect(40, 104, 8, 8);
}
