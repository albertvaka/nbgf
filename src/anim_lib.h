#pragma once

#include "animation_frame.h"

#include "texturepackerexport.h"

namespace AnimLib {

	constexpr const AnimationFrame BUTTON_A_PRESS[] = {
		{ {448 + 0, 0, 8, 8}, 0.1f },
		{ {448 + 8, 0, 8, 8}, 0.25f },
		{ {448 + 0, 0, 8, 8}, 0.4f },
	};

	constexpr const AnimationFrame BUTTON_B_PRESS[] = {
		{ {448 + 0, 8, 8, 8}, 0.1f },
		{ {448 + 8, 8, 8, 8}, 0.25f },
		{ {448 + 0, 8, 8, 8}, 0.4f },
	};

	constexpr const AnimationFrame MARIO_IDLE[] = {
		{ {0,0,16,32}, 1.f },
	};

	constexpr const AnimationFrame MARIO_WALK[] = {
		{ {16,0,16,32}, 0.1f },
		{ {96 + 32,0,16,32}, 0.1f },
		{ {32,0,16,32}, 0.1f },
	};

	constexpr const AnimationFrame MARIO_TURN[] = {
		{ {48,0,16,32}, 1.f },
	};

	constexpr const AnimationFrame MARIO_JUMP[] = {
		{ {64,0,16,32}, 1.f },
	};

	constexpr const AnimationFrame MARIO_CROUCH[] = {
		{ {80,10,16,22}, 1.f },
	};

	constexpr const AnimationFrame MARIO_ONWALL[] = {
		{ {96,0,16,32}, 1.f },
	};

	constexpr const AnimationFrame MARIO_CROUCH_LOOK_UP[] = {
		{ {96 + 16,9,16,22}, 1.f },
	};

	constexpr const AnimationFrame BAT_FLYING[] = {
		{ {0 * 32, 2 * 32, 32, 32}, 0.1f },
		{ {1 * 32, 2 * 32, 32, 32}, 0.1f },
		{ {2 * 32, 2 * 32, 32, 32}, 0.1f },
		{ {1 * 32, 2 * 32, 32, 32}, 0.1f },
	};

	constexpr const AnimationFrame BAT_FLIP[] = {
		{ {3 * 32, 2 * 32, 32, 32}, 0.1f },
	};

	constexpr const AnimationFrame BAT_SIESTA[] = {
		{ {1 * 32, 3 * 32, 32, 32}, 10.f },
		{ {0 * 32, 3 * 32, 32, 32}, 0.7f },
	};

	constexpr const AnimationFrame BAT_AWAKE[] = {
		{ {2 * 32, 3 * 32, 32, 32}, 0.7f },
	};

	constexpr const AnimationFrame FIRESLIME_WALK[] = {
		{ { 13 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 14 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 15 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 16 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 17 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 18 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 12 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
	};

	constexpr const AnimationFrame FIRESLIME_ATTACK[] = {
		{ { 20 * 16.f, 0 * 16.f, 16.f, 16.f }, .2f },
		{ { 21 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 22 * 16.f, 0 * 16.f, 16.f, 16.f }, .4f },
		{ { 25 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 26 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 12 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
	};

	constexpr const AnimationFrame MAGIC_EXPLOSION[] = {
		{ { 128 + 10 * 16.f, 256.f, 16.f, 16.f }, .05f },
		{ { 128 + 11 * 16.f, 256.f, 16.f, 16.f }, .05f },
		{ { 128 + 12 * 16.f, 256.f, 16.f, 16.f }, .05f },
		{ { 128 + 13 * 16.f, 256.f, 16.f, 16.f }, .05f },
		{ { 128 + 14 * 16.f, 256.f, 16.f, 16.f }, .05f },
		{ { 128 + 15 * 16.f, 256.f, 16.f, 16.f }, .05f },
	};

	constexpr const AnimationFrame MANTIS_PREPARE_JUMP[] = {
		{ {2 * 32, 8 * 32, 32, 32}, .17f },
		{ {1 * 32, 8 * 32, 32, 32}, .1f },
		{ {0 * 32, 8 * 32, 32, 32}, .1f },
	};

	constexpr const GPU_Rect MANTIS_AIR = { 1 * 32, 8 * 32, 32, 32 };

	constexpr const AnimationFrame MANTIS_WALK[] = {
		{ {0 * 32, 10 * 32, 32, 32}, .1f },
		{ {1 * 32, 10 * 32, 32, 32}, .1f },
		{ {2 * 32, 10 * 32, 32, 32}, .1f },
		{ {3 * 32, 10 * 32, 32, 32}, .1f },
	};

	constexpr const AnimationFrame GOOMBA[] = {
		{ {14 * 16, 16, 16, 16}, .2f },
		{ {15 * 16, 16, 16, 16}, .2f },
	};

	constexpr const AnimationFrame GOOMBACHARGER[] = {
		{ {16 * 16, 16, 16, 16}, .2f },
		{ {17 * 16, 16, 16, 16}, .2f },
	};
	constexpr const AnimationFrame BIPEDAL_WALKING[] = {
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_000, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_001, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_002, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_003, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_004, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_005, .25f },
	};

	constexpr const AnimationFrame MISSILE[] = {
		{ TexturePackerRects::BOSSWHEELERXMISSILE_DEFAULT_000, 0.1f },
		{ TexturePackerRects::BOSSWHEELERXMISSILE_DEFAULT_001, 0.1f },
	};

	constexpr const AnimationFrame EXPLOSION[] = {
		{ TexturePackerRects::EXPLOSION_01, 0.05f },
		{ TexturePackerRects::EXPLOSION_02, 0.05f },
		{ TexturePackerRects::EXPLOSION_03, 0.05f },
		{ TexturePackerRects::EXPLOSION_04, 0.05f },
		{ TexturePackerRects::EXPLOSION_05, 0.05f },
		{ TexturePackerRects::EXPLOSION_06, 0.05f },
		{ TexturePackerRects::EXPLOSION_07, 0.05f },
		{ TexturePackerRects::EXPLOSION_08, 0.05f },
		{ TexturePackerRects::EXPLOSION_09, 0.05f },
	};

	constexpr const AnimationFrame FLYING_ALIEN[] = {
		{ TexturePackerRects::FLYINGALIEN_01, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_02, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_03, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_04, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_05, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_06, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_07, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_08, 0.05f },
	};

	constexpr const AnimationFrame HIT_SPLASH[] = {
		{ { 7 * 32, 8 * 32, 40, 32 }, 0.05f },
	};

	constexpr const GPU_Rect BIPEDAL_IDLE = TexturePackerRects::BIPEDALUNIT_IDLE_00;

	constexpr const GPU_Rect FIRESHOT = {437, 5, 6, 6};

	constexpr const GPU_Rect HEALTH_FULL = { 19 * 16, 1 * 16, 16, 16 };
	constexpr const GPU_Rect HEALTH_EMPTY = { 20 * 16, 1 * 16, 16, 16 };

	constexpr const GPU_Rect BULLET = { 128 + 9 * 16, 256, 16, 16 };

	constexpr const GPU_Rect POLVITO_PARTICLE = { 50.f, 34.f, 2.f, 2.f };

	constexpr const GPU_Rect ROCKET_LAUCNHER_BASE = { 24 * 16.f, 256.f, 16.f, 16.f };
	constexpr const GPU_Rect ROCKET_LAUCNHER_TURRET = { 25 * 16.f, 256.f, 16.f, 16.f };

	constexpr const AnimationFrame MINOTAUR_IDLE[] = {
		{ { 0 * 96.0, 0 * 96.f, 96.f, 96.f }, .1f },
		{ { 1 * 96.0, 0 * 96.f, 96.f, 96.f }, .1f },
		{ { 2 * 96.0, 0 * 96.f, 96.f, 96.f }, .1f },
		{ { 3 * 96.0, 0 * 96.f, 96.f, 96.f }, .1f },
		{ { 4 * 96.0, 0 * 96.f, 96.f, 96.f }, .1f },
	};

	constexpr const AnimationFrame MINOTAUR_RUN[] = {
		{ { 0 * 96.0, 1 * 96.f, 96.f, 96.f }, .1f },
		{ { 1 * 96.0, 1 * 96.f, 96.f, 96.f }, .1f },
		{ { 2 * 96.0, 1 * 96.f, 96.f, 96.f }, .1f },
		{ { 3 * 96.0, 1 * 96.f, 96.f, 96.f }, .1f },
		{ { 4 * 96.0, 1 * 96.f, 96.f, 96.f }, .1f },
		{ { 5 * 96.0, 1 * 96.f, 96.f, 96.f }, .1f },
		{ { 6 * 96.0, 1 * 96.f, 96.f, 96.f }, .1f },
		{ { 7 * 96.0, 1 * 96.f, 96.f, 96.f }, .1f },
	};

	constexpr const AnimationFrame MINOTAUR_TAUNT[] = {
		{ { 0 * 96.0, 2 * 96.f, 96.f, 96.f }, .1f },
		{ { 1 * 96.0, 2 * 96.f, 96.f, 96.f }, .1f },
		{ { 2 * 96.0, 2 * 96.f, 96.f, 96.f }, .1f },
		{ { 3 * 96.0, 2 * 96.f, 96.f, 96.f }, .1f },
		{ { 4 * 96.0, 2 * 96.f, 96.f, 96.f }, .1f },
	};

	constexpr const AnimationFrame MINOTAUR_ATTACK_BIG[] = {
		{ { 0 * 96.0, 3 * 96.f, 96.f, 96.f }, .8f },
		{ { 1 * 96.0, 3 * 96.f, 96.f, 96.f }, .1f },
		{ { 2 * 96.0, 3 * 96.f, 96.f, 96.f }, .1f },
		{ { 3 * 96.0, 3 * 96.f, 96.f, 96.f }, .1f },
		{ { 4 * 96.0, 3 * 96.f, 96.f, 96.f }, .1f },
		{ { 5 * 96.0, 3 * 96.f, 96.f, 96.f }, .1f },
		{ { 6 * 96.0, 3 * 96.f, 96.f, 96.f }, .1f },
		{ { 7 * 96.0, 3 * 96.f, 96.f, 96.f }, .1f },
		{ { 8 * 96.0, 3 * 96.f, 96.f, 96.f }, .1f },
	};

	constexpr const AnimationFrame MINOTAUR_ATTACK_SMALL[] = {
		{ { 0 * 96.0, 4 * 96.f, 96.f, 96.f }, .1f },
		{ { 1 * 96.0, 4 * 96.f, 96.f, 96.f }, .1f },
		{ { 2 * 96.0, 4 * 96.f, 96.f, 96.f }, .1f },
		{ { 3 * 96.0, 4 * 96.f, 96.f, 96.f }, .1f },
		{ { 4 * 96.0, 4 * 96.f, 96.f, 96.f }, .1f },
	};

	constexpr const AnimationFrame MINOTAUR_ATTACK_SPIN[] = {
		{ { 0 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 1 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 2 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 3 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 4 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 5 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 6 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 7 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 8 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
	};

	constexpr const AnimationFrame MINOTAUR_FLIP[] = {
		{ { 3 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 4 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 5 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 6 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 7 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
		{ { 8 * 96.0, 5 * 96.f, 96.f, 96.f }, .1f },
	};

	constexpr const AnimationFrame MINOTAUR_DAMAGE1[] = {
		{ { 0 * 96.0, 6 * 96.f, 96.f, 96.f }, .1f },
		{ { 1 * 96.0, 6 * 96.f, 96.f, 96.f }, .1f },
		{ { 2 * 96.0, 6 * 96.f, 96.f, 96.f }, .1f },
	};

	constexpr const AnimationFrame MINOTAUR_DAMAGE2[] = {
		{ { 0 * 96.0, 7 * 96.f, 96.f, 96.f }, .1f },
		{ { 1 * 96.0, 7 * 96.f, 96.f, 96.f }, .1f },
		{ { 2 * 96.0, 7 * 96.f, 96.f, 96.f }, .1f },
	};

	constexpr const AnimationFrame MINOTAUR_DIE[] = {
		{ { 0 * 96.0, 8 * 96.f, 96.f, 96.f }, .1f },
		{ { 1 * 96.0, 8 * 96.f, 96.f, 96.f }, .1f },
		{ { 2 * 96.0, 8 * 96.f, 96.f, 96.f }, .1f },
		{ { 3 * 96.0, 8 * 96.f, 96.f, 96.f }, .1f },
		{ { 4 * 96.0, 8 * 96.f, 96.f, 96.f }, .1f },
		{ { 5 * 96.0, 8 * 96.f, 96.f, 96.f }, .1f },
	};

	const constexpr SheetFrameCalculator warriorSheet(70.f, 48.f, 6, .1f);
	constexpr const AnimationFrame WARRIOR_IDLE[] = { warriorSheet.Frame(5, 1.f), warriorSheet.Frame(0, 1.0f), };
	constexpr const std::array<AnimationFrame, 8> WARRIOR_RUN = warriorSheet.Frames<8>(6);
	constexpr const std::array<AnimationFrame, 2> WARRIOR_PULL_SWORD_OUT = warriorSheet.Frames<2>(14);
	constexpr const std::array<AnimationFrame, 3> WARRIOR_ATTACK_UP = warriorSheet.Frames<3>(22);
	//constexpr const std::array<AnimationFrame, 4> WARRIOR_ATTACK = warriorSheet.Frames<4>(18);
	//constexpr const std::array<AnimationFrame, 4> WARRIOR_COMBO = warriorSheet.Frames<4>(22);
	constexpr const std::array<AnimationFrame, 11> WARRIOR_DEATH = warriorSheet.Frames<11>(26);
	constexpr const std::array<AnimationFrame, 3> WARRIOR_HURT = warriorSheet.Frames<3>(38); //skip first 1
	constexpr const std::array<AnimationFrame, 3> WARRIOR_JUMP = warriorSheet.Frames<3>(42);
	constexpr const std::array<AnimationFrame, 2> WARRIOR_JUMP_TO_FALL = warriorSheet.Frames<2>(44);
	constexpr const std::array<AnimationFrame, 3> WARRIOR_FALL = warriorSheet.Frames<3>(46);
	constexpr const std::array<AnimationFrame, 5> WARRIOR_EDGE_GRAB = warriorSheet.Frames<5>(49);
	constexpr const std::array<AnimationFrame, 6> WARRIOR_EDGE_IDLE = warriorSheet.Frames<6>(54);
	constexpr const std::array<AnimationFrame, 3> WARRIOR_WALL_SLIDE = warriorSheet.Frames<3>(60);
	constexpr const std::array<AnimationFrame, 3> WARRIOR_CROUCH = warriorSheet.Frames<3>(63);
	constexpr const std::array<AnimationFrame, 2> WARRIOR_STANDUP = warriorSheet.Frames<2>(67); //skip first 1
	constexpr const std::array<AnimationFrame, 4> WARRIOR_DASH = warriorSheet.Frames<4>(69);
	constexpr const std::array<AnimationFrame, 4> WARRIOR_DASH_STOP = warriorSheet.Frames<4>(73);
	constexpr const std::array<AnimationFrame, 9> WARRIOR_DASH_ATTACK = warriorSheet.Frames<9>(77);
	constexpr const std::array<AnimationFrame, 5> WARRIOR_SLIDE = warriorSheet.Frames<5>(86);
	constexpr const std::array<AnimationFrame, 8> WARRIOR_CLIMB = warriorSheet.Frames<8>(91);
	constexpr const std::array<AnimationFrame, 3> WARRIOR_WALL_SLIDE_ATTACK = warriorSheet.Frames<3>(99);
	constexpr const std::array<AnimationFrame, 1> WARRIOR_ATTACK_DOWN_TRANSITION = warriorSheet.Frames<1>(102);
	constexpr const std::array<AnimationFrame, 3> WARRIOR_ATTACK_DOWN = warriorSheet.Frames<3>(103);
	constexpr const AnimationFrame WARRIOR_TURN[] = { warriorSheet.Frame(12*6+1, 1.f) };
	constexpr const std::array<AnimationFrame, 3> WARRIOR_MOVING_ATTACK = warriorSheet.Frames<3>(78); // skip first 2 and last 5 of DASH_ATTACK

}

