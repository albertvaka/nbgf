#pragma once

enum class GameKeys
{
	NONE = 0,
	MENU_UP, MENU_DOWN,
	JUMP, CROUCH, LEFT, RIGHT,
	START,
	SHOOT, ATTACK, DASH, ACTION,
};

enum class AnalogInput
{
	NONE = 0,
	MOVE, // normalized
	AIM, // in world coordinates
};
