#pragma once

enum class GameKeys
{
	NONE = 0,
	UP, DOWN, LEFT, RIGHT,
	START,
	SHOOT,
	NOTE_1,
	NOTE_2,
	NOTE_3,
	NOTE_4,
};

enum class AnalogInput
{
	NONE = 0,
	MOVE, // between 0-1
	AIM, // in world coordinates
};