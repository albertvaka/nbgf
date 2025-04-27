#pragma once

inline bool movementEnabled = true;

enum class GameKeys
{
	NONE = 0,
	UP, DOWN, LEFT, RIGHT,
	START,
	DRIFT,
};

enum class AnalogInput
{
	NONE = 0,
	MOVE, // between 0-1
};
