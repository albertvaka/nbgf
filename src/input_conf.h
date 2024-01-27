#pragma once

enum class GameKeys
{
	NONE = 0,
	UP, DOWN, LEFT, RIGHT,
	START,
	SHOOT,
};

enum class AnalogInput
{
	NONE = 0,
	MOVE, // between 0-1
	AIM, // in world coordinates
};