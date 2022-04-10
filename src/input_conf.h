#pragma once

enum class GameKeys
{
	NONE = 0,

	LEFT,
	RIGHT, // FIXME: Still used in some places, might be good to replace with AnalogInput::MOVE
	CROUCH,
	JUMP,
	SHOOT,
	ATTACK,
	DASH,
	ACTION,

	PAUSE,

	MENU_UP,
	MENU_DOWN,
	MENU_ACCEPT,
	MENU_CANCEL,
};

namespace GameKeyAliases {
	static const constexpr GameKeys DIALOG_NEXT = GameKeys::MENU_ACCEPT;
	static const constexpr GameKeys DIALOG_SKIP = GameKeys::MENU_CANCEL;
};

enum class AnalogInput
{
	NONE = 0,
	MOVE, // normalized
	AIM, // in world coordinates
};
