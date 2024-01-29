#pragma once

#include "bounds.h"

struct AnimationData
{
	int frames;
	GPU_Rect rect[16];
	int timer[16];
};

enum AnimationType
{
	PLAYER_WALKING_DOWN,
	PLAYER_WALKING_UP,
	PLAYER_WALKING_RIGHT,
	PLAYER_WALKING_LEFT,
	PLAYER_IDLE_DOWN,
	PLAYER_IDLE_UP,
	PLAYER_IDLE_RIGHT,
	PLAYER_IDLE_LEFT,
	DOCTOR_WALKING_DOWN,
	DOCTOR_WALKING_UP,
	DOCTOR_WALKING_RIGHT,
	DOCTOR_WALKING_LEFT,
	DOCTOR_IDLE_DOWN,
	DOCTOR_IDLE_UP,
	DOCTOR_IDLE_RIGHT,
	DOCTOR_IDLE_LEFT,
	BELT_RIGHT,
	BUTTON_A_PRESS,
	NADA,
	POKEMON,
	PLAGUEDOC_IDLE_DOWN,
	PLAGUEDOC_IDLE_UP,
	PLAGUEDOC_IDLE_LEFT,
	PLAGUEDOC_IDLE_RIGHT,
	PLAGUEDOC_WALKING_LEFT,
	PLAGUEDOC_WALKING_RIGHT,
	PLAGUEDOC_WALKING_UP,
	PLAGUE_DOC_WALKING_DOWN,
	COOLGUY_IDLE_DOWN,
	COOLGUY_IDLE_UP,
	COOLGUY_IDLE_LEFT,
	COOLGUY_IDLE_RIGHT,
	COOLGUY_WALKING_DOWN,
	COOLGUY_WALKING_UP,
	COOLGUY_WALKING_LEFT,
	COOLGUY_WALKING_RIGHT,
	ROOMBA_DOWN,
	ROOMBA_UP,
	ROOMBA_LEFT,
	ROOMBA_RIGHT,
	ROOMBA_DOOR_OPEN,
	ROOMBA_DOOR_CLOSE,
	CAPSULE_OPENING,
	CAPSULE_OPENED,
	CAPSULE_CLOSED,
	CAPSULE_CLOSING,
	SPAWN,
};

static AnimationData anim_lib[] =
{
	//PLAYER_WALKING_DOWN
	{
		4,
		{
			{3,16,10,16},
			{3,0,10,16},
			{3,32,10,16},
			{3,0,10,16},
		},
		{
			100,100,100,100,
		},
	},
	//PLAYER_WALKING_UP
	{
		4,
		{
			{19,16,10,16},
			{19,0,9,16},
			{19,32,10,16},
			{19,0,9,16},
		},
		{
			100,100,100,100,
		},
	},
	//PLAYER_WALKING_RIGHT
	{
		3,
		{
			{36,16,9,16},
			{36,32,10,16},
			{36,0,9,16},
		},
		{
			100,100,100,
		},
	},
	//PLAYER_WALKING_LEFT
	{
		3,
		{
			{52,16,9,16},
			{51,32,10,16},
			{52,0,9,16},
		},
		{
			100,100,100,
		},
	},
	//PLAYER_IDLE_DOWN
	{
		1,
		{
			{3,0,10,16},
		},
		{
			100,
		},
	},
	//PLAYER_IDLE_UP
	{
		1,
		{
			{20,0,8,16},
		},
		{
			100,
		},
	},
	//PLAYER_IDLE_RIGHT
	{
		1,
		{
			{36,0,9,16},
		},
		{
			100,
		},
	},
	//PLAYER_IDLE_LEFT
	{
		1,
		{
			{52,0,9,16},
		},
		{
			100,
		},
	},
	//DOCTOR_WALKING_DOWN
	{
		4,
		{
			{67,16,10,16},
			{67,0,10,16},
			{68,32,9,16},
			{67,0,10,16},
		},
		{
			100,100,100,100,
		},
	},
	//DOCTOR_WALKING_UP
	{
		4,
		{
			{83,16,9,16},
			{83,0,10,16},
			{84,32,9,16},
			{83,0,10,16},
		},
		{
			100,100,100,100,
		},
	},
	//DOCTOR_WALKING_RIGHT
	{
		3,
		{
			{100,16,9,16},
			{100,32,10,16},
			{100,0,9,16},
		},
		{
			100,100,100,
		},
	},
	//DOCTOR_WALKING_LEFT
	{
		3,
		{
			{116,16,9,16},
			{115,32,10,16},
			{116,0,9,16},
		},
		{
			100,100,100,
		},
	},
	//DOCTOR_IDLE_DOWN
	{
		1,
		{
			{67,0,10,16},
		},
		{
			100,
		},
	},
	//DOCTOR_IDLE_UP
	{
		1,
		{
			{83,0,10,16},
		},
		{
			1,
		},
	},
	//DOCTOR_IDLE_RIGHT
	{
		1,
		{
			{100,0,9,16},
		},
		{
			100,
		},
	},
	//DOCTOR_IDLE_LEFT
	{
		1,
		{
			{116,0,9,16},
		},
		{
			100,
		},
	},
	//BELT_RIGHT
	{
		8,
		{
			{104,240,16,16},
			{103,240,16,16},
			{102,240,16,16},
			{101,240,16,16},
			{100,240,16,16},
			{99,240,16,16},
			{98,240,16,16},
			{97,240,16,16},
		},
		{
			40,40,40,40,40,40,40,40,
		},
	},
	//BUTTON_A_PRESS
	{
		3,
		{
			{0,112,8,8},
			{8,112,8,8},
			{0,112,8,8},
		},
		{
			100,250,400,
		},
	},
	//NADA
	{
		1,
		{
			{1,1,1,1},
		},
		{
			1,
		},
	},
	//POKEMON
	{
		12,
		{
			{96,48,30,32},
			{126,48,30,32},
			{156,48,30,32},
			{186,48,30,32},
			{96,80,30,32},
			{126,80,30,32},
			{156,80,30,32},
			{186,80,30,32},
			{96,112,30,32},
			{126,112,30,32},
			{156,112,30,32},
			{186,112,30,32},
		},
		{
			100,100,100,100,100,100,100,100,100,100,100,100,
		},
	},
	//PLAGUEDOC_IDLE_DOWN
	{
		1,
		{
			{131,0,10,16},
		},
		{
			100,
		},
	},
	//PLAGUEDOC_IDLE_UP
	{
		1,
		{
			{147,0,10,16},
		},
		{
			100,
		},
	},
	//PLAGUEDOC_IDLE_LEFT
	{
		1,
		{
			{179,0,11,16},
		},
		{
			100,
		},
	},
	//PLAGUEDOC_IDLE_RIGHT
	{
		1,
		{
			{163,0,11,16},
		},
		{
			100,
		},
	},
	//PLAGUEDOC_WALKING_LEFT
	{
		3,
		{
			{179,0,11,16},
			{179,16,11,16},
			{178,32,11,16},
		},
		{
			100,100,100,
		},
	},
	//PLAGUEDOC_WALKING_RIGHT
	{
		3,
		{
			{163,0,11,16},
			{163,16,11,16},
			{164,32,11,16},
		},
		{
			100,100,100,
		},
	},
	//PLAGUEDOC_WALKING_UP
	{
		4,
		{
			{147,16,10,16},
			{147,0,10,16},
			{147,32,10,16},
			{147,0,10,16},
		},
		{
			100,100,100,100,
		},
	},
	//PLAGUE_DOC_WALKING_DOWN
	{
		4,
		{
			{131,16,10,16},
			{131,0,10,16},
			{131,32,10,16},
			{131,0,10,16},
		},
		{
			100,100,100,100,
		},
	},
	//COOLGUY_IDLE_DOWN
	{
		1,
		{
			{195,0,10,16},
		},
		{
			100,
		},
	},
	//COOLGUY_IDLE_UP
	{
		1,
		{
			{211,0,10,16},
		},
		{
			100,
		},
	},
	//COOLGUY_IDLE_LEFT
	{
		1,
		{
			{243,0,9,16},
		},
		{
			100,
		},
	},
	//COOLGUY_IDLE_RIGHT
	{
		1,
		{
			{228,0,9,16},
		},
		{
			100,
		},
	},
	//COOLGUY_WALKING_DOWN
	{
		4,
		{
			{195,16,9,16},
			{195,0,10,16},
			{196,32,9,16},
			{195,0,10,16},
		},
		{
			100,100,100,0,
		},
	},
	//COOLGUY_WALKING_UP
	{
		4,
		{
			{211,16,9,16},
			{211,0,10,16},
			{212,32,9,16},
			{211,0,10,16},
		},
		{
			100,100,100,100,
		},
	},
	//COOLGUY_WALKING_LEFT
	{
		3,
		{
			{243,16,9,16},
			{242,32,10,16},
			{243,0,9,16},
		},
		{
			100,100,100,
		},
	},
	//COOLGUY_WALKING_RIGHT
	{
		3,
		{
			{228,16,9,16},
			{228,32,10,16},
			{228,0,9,16},
		},
		{
			100,100,100,
		},
	},
	//ROOMBA_DOWN
	{
		4,
		{
			{1,129,14,14},
			{1,145,14,14},
			{1,129,14,14},
			{1,145,14,14},
		},
		{
			800,175,125,175,
		},
	},
	//ROOMBA_UP
	{
		4,
		{
			{17,129,14,14},
			{17,145,14,14},
			{17,129,14,14},
			{17,145,14,14},
		},
		{
			800,175,125,175,
		},
	},
	//ROOMBA_LEFT
	{
		4,
		{
			{33,129,14,14},
			{33,145,14,14},
			{33,129,14,14},
			{33,145,14,14},
		},
		{
			800,175,125,175,
		},
	},
	//ROOMBA_RIGHT
	{
		4,
		{
			{49,129,14,14},
			{49,145,14,14},
			{49,129,14,14},
			{49,145,14,14},
		},
		{
			800,175,125,175,
		},
	},
	//ROOMBA_DOOR_OPEN
	{
		5,
		{
			{0,192,16,16},
			{16,192,16,16},
			{32,192,16,16},
			{48,192,16,16},
			{64,192,16,16},
		},
		{
			200,200,200,200,900,
		},
	},
	//ROOMBA_DOOR_CLOSE
	{
		5,
		{
			{64,192,16,16},
			{48,192,16,16},
			{32,192,16,16},
			{16,192,16,16},
			{1,192,16,16},
		},
		{
			200,200,200,200,500,
		},
	},
	//CAPSULE_OPENING
	{
		6,
		{
			{96,224,16,16},
			{112,224,16,16},
			{128,224,16,16},
			{144,224,16,16},
			{160,224,16,16},
			{176,224,16,16},
		},
		{
			100,100,100,100,100,500,
		},
	},
	//CAPSULE_OPENED
	{
		1,
		{
			{176,224,16,16},
		},
		{
			500,
		},
	},
	//CAPSULE_CLOSED
	{
		1,
		{
			{96,224,16,16},
		},
		{
			500,
		},
	},
	//CAPSULE_CLOSING
	{
		6,
		{
			{176,224,16,16},
			{160,224,16,16},
			{144,224,16,16},
			{128,224,16,16},
			{112,224,16,16},
			{96,224,16,16},
		},
		{
			100,100,100,100,100,500,
		},
	},
	//SPAWN
	{
		7,
		{
			{144,160,16,16},
			{160,160,16,16},
			{176,160,16,16},
			{192,160,16,16},
			{208,160,16,16},
			{224,160,16,16},
			{0,0,0,0},
		},
		{
			100,100,100,75,75,25,500,
		},
	},
};