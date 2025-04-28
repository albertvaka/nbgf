#pragma once

#include "animation_frame.h"

constexpr AnimationFrame BUTTON_A_PRESS[] =
{
	{{0,112,8,8}, 100.f },
	{{8,112,8,8}, 250.f },
	{{0,112,8,8}, 400.f },
};

constexpr AnimationFrame MARIO_IDLE[] = {
	{{0,0,16,32}, 1000}
};

constexpr AnimationFrame MARIO_WALK[] = {
	{ {16,0,16,32}, 100 },
	{ {96 + 32,0,16,32}, 100 },
	{ {32,0,16,32}, 100 },
};


constexpr AnimationFrame MARIO_TURN[] = {
	{ {48,0,16,32}, 100 }
};

constexpr AnimationFrame MARIO_JUMP[] = {
	{ {64,0,16,32}, 100 }
};

constexpr AnimationFrame MARIO_CROUCH[] = {
	{ {80,10,16,22}, 1000 }
};

constexpr AnimationFrame MARIO_ONWALL[] = {
	{ {96,0,16,32}, 1000 }
};

constexpr AnimationFrame MARIO_CROUCH_LOOK_UP[] = {
	{ {96 + 16,9,16,22}, 1000 }
};

constexpr AnimationFrame FOUNTAIN[] = {
	{ {6 * 16, 2 * 16, 32, 3 * 16}, 100},
	{ {8 * 16, 2 * 16, 32, 3 * 16}, 100},
	{ {10 * 16, 2 * 16, 32, 3 * 16}, 100},
};

constexpr AnimationFrame NPC_IDLE[] = {
	{{0 * 30, 0, 30, 50},220},
	{{1 * 30, 0, 30, 50},150},
	{{2 * 30, 0, 30, 50},150},
	{{3 * 30, 0, 30, 50},150},
	{{4 * 30, 0, 30, 50},150},
	{{5 * 30, 0, 30, 50},150},
	{{6 * 30, 0, 30, 50},150},
	{{0 * 30, 0, 30, 50},150},
	{{0 * 30, 0, 30, 50},220},
	{{1 * 30, 0, 30, 50},150},
	{{2 * 30, 0, 30, 50},150},
	{{3 * 30, 0, 30, 50},150},
	{{4 * 30, 0, 30, 50},150},
	{{5 * 30, 0, 30, 50},150},
	{{6 * 30, 0, 30, 50},150},
	{{7 * 30, 0, 30, 50},150},
};

constexpr AnimationFrame NPC_WALK[] = {
	{{0 * 30, 50, 30, 51},100},
	{{1 * 30, 50, 30, 51},100},
	{{2 * 30, 50, 30, 51},100},
	{{3 * 30, 50, 30, 51},100},
	{{4 * 30, 50, 30, 51},100},
	{{5 * 30, 50, 30, 51},100},
	{{6 * 30, 50, 30, 51},100},
	{{7 * 30, 50, 30, 51},100},
};

constexpr AnimationFrame DOGGO_IDLE[] = {
	{{0, 0, 28, 28}, 250},
	{{1*28, 0, 28, 28}, 250},
};

constexpr AnimationFrame DOGGO_LEFT[] = {
	{{0 ,  1 * 28, 28, 28}, 100},
	{{1 * 28, 1 * 28, 28, 28}, 100},
	{{2 * 28, 1 * 28, 28, 28}, 100},
};

constexpr AnimationFrame DOGGO_RIGHT[] = {
	{{0 ,  2 * 28, 28, 28},100},
	{{1 * 28, 2 * 28, 28, 28}, 100},
	{{2 * 28, 2*28, 28, 28}, 100},
};

constexpr AnimationFrame DOGGO_EAT[] = {
	{{0, 3 * 28, 28, 28},150},
	{{28, 3 * 28, 28, 28},150},
};

