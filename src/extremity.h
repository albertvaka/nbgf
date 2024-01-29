#pragma once

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include <map>

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

	return ExtremityType(Rand::roll(magic_enum::enum_count<ExtremityType>() - 1) + 1);
};

inline ExtremityColor RandomExtremityColor() {
	return ExtremityColor(Rand::roll(magic_enum::enum_count<ExtremityColor>() - 1) + 1);
};

static std::map<ExtremityColor, std::map<ExtremityType, GPU_Rect>> extremitySprPos;

struct Extremity : SortedDrawable, Cintable, SelfRegister<Extremity>
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
		SetSpeedWithCinta(speed);
		pos += speed * dt;
		speed = vec::Zero;
	}


	void Draw() override
	{

		//BoxBounds(positionPlz(),sizePlz()).Draw(window);
		vec carriedOffset;
		if (isCarried)
		{
			carriedOffset.y = -2;
		}

		vec drawPos = pos+Offset()+carriedOffset;
		GPU_Rect rect = extremitySprPos[color][type];

		//Window::Draw(Assets::sprite, drawPos+vec(0,0.95)).withRect(rect).withColor(0,0,0).withOrigin(8,8);
		//Window::Draw(Assets::sprite, drawPos+vec(0.95,0)).withRect(rect).withColor(0,0,0).withOrigin(8,8);
		//Window::Draw(Assets::sprite, drawPos+vec(0,-0.95)).withRect(rect).withColor(0,0,0).withOrigin(8,8);
		//Window::Draw(Assets::sprite, drawPos+vec(-0.95,0)).withRect(rect).withColor(0,0,0).withOrigin(8,8);
		Window::Draw(Assets::sprite, drawPos).withRect(rect).withOrigin(8,8);

		if (!isLet)
		{
			// Tarrito de mermelada
			Window::Draw(Assets::sprite, pos+carriedOffset).withRect(54, 86, 12, 12).withOrigin(8,8);
		}

		//BoxBounds(pos.x , pos.y , 16, 16).Draw(window);
		//BoxBounds(pos.x - 1, pos.y - 1, 2, 2).Draw(window);

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

inline void loadExtremityMap()
{
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::RIGHT_LEG] = GPU_Rect{ 0, 64, 8, 8};
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::LEFT_LEG] = GPU_Rect{ 8, 64, 8, 8};
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::RIGHT_ARM] = GPU_Rect{ 16, 64, 8, 8};
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::LEFT_ARM] = GPU_Rect{ 24, 64, 8, 8};
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::HEAD] = GPU_Rect{ 32, 64, 8, 8};
	extremitySprPos[ExtremityColor::WHITE][ExtremityType::BODY] = GPU_Rect{ 40, 64, 8, 8};

	extremitySprPos[ExtremityColor::BLACK][ExtremityType::RIGHT_LEG] = GPU_Rect{ 0, 72, 8, 8};
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::LEFT_LEG] = GPU_Rect{ 8, 72, 8, 8};
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::RIGHT_ARM] = GPU_Rect{ 16, 72, 8, 8};
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::LEFT_ARM] = GPU_Rect{ 24, 72, 8, 8};
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::HEAD] = GPU_Rect{ 32, 72, 8, 8};
	extremitySprPos[ExtremityColor::BLACK][ExtremityType::BODY] = GPU_Rect{ 40, 72, 8, 8};

	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::RIGHT_LEG] = GPU_Rect{ 0, 80, 8, 8};
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::LEFT_LEG] = GPU_Rect{ 8, 80, 8, 8};
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::RIGHT_ARM] = GPU_Rect{ 16, 80, 8, 8};
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::LEFT_ARM] = GPU_Rect{ 24, 80, 8, 8};
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::HEAD] = GPU_Rect{ 32, 80, 8, 8};
	extremitySprPos[ExtremityColor::YELLOW][ExtremityType::BODY] = GPU_Rect{ 40, 80, 8, 8};

	extremitySprPos[ExtremityColor::GREEN][ExtremityType::RIGHT_LEG] = GPU_Rect{ 0, 88, 8, 8};
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::LEFT_LEG] = GPU_Rect{ 8, 88, 8, 8};
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::RIGHT_ARM] = GPU_Rect{ 16, 88, 8, 8};
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::LEFT_ARM] = GPU_Rect{ 24, 88, 8, 8};
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::HEAD] = GPU_Rect{ 32, 88, 8, 8};
	extremitySprPos[ExtremityColor::GREEN][ExtremityType::BODY] = GPU_Rect{ 40, 88, 8, 8};

	extremitySprPos[ExtremityColor::BLUE][ExtremityType::RIGHT_LEG] = GPU_Rect{ 0, 96, 8, 8};
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::LEFT_LEG] = GPU_Rect{ 8, 96, 8, 8};
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::RIGHT_ARM] = GPU_Rect{ 16, 96, 8, 8};
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::LEFT_ARM] = GPU_Rect{ 24, 96, 8, 8};
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::HEAD] = GPU_Rect{ 32, 96, 8, 8};
	extremitySprPos[ExtremityColor::BLUE][ExtremityType::BODY] = GPU_Rect{ 40, 96, 8, 8};

	extremitySprPos[ExtremityColor::RED][ExtremityType::RIGHT_LEG] = GPU_Rect{ 0, 104, 8, 8};
	extremitySprPos[ExtremityColor::RED][ExtremityType::LEFT_LEG] = GPU_Rect{ 8, 104, 8, 8};
	extremitySprPos[ExtremityColor::RED][ExtremityType::RIGHT_ARM] = GPU_Rect{ 16, 104, 8, 8};
	extremitySprPos[ExtremityColor::RED][ExtremityType::LEFT_ARM] = GPU_Rect{ 24, 104, 8, 8};
	extremitySprPos[ExtremityColor::RED][ExtremityType::HEAD] = GPU_Rect{ 32, 104, 8, 8};
	extremitySprPos[ExtremityColor::RED][ExtremityType::BODY] = GPU_Rect{ 40, 104, 8, 8};
}
