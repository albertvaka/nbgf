#pragma once

namespace GameConstants
{
	const int MAX_ENTITIES = 2048;

	const float PLAYER_ACCEL = 1200;
	const float PLAYER_MAX_SPEED = 350;
	const float SOLDIER_MAX_SPEED = 300.f;
	
	const float COOLDOWN_SHOTS = 0.5f;
	const float BULLET_LIFETIME = 1.0f;
	const float HQ_BULLET_LIFETIME = 0.35f;

	const float BULLET_SPEED = 500.f;
	const int BULLET_SPEED_HQ = 1600.0f;

	const float HQ_COOLDOWN_SHOTS = 0.50f;
	const float HQ_ATTACK_RADIUS = 400.f;
	const float SOLDIER_ATTACK_RADIUS = 300.f;

	const float TIME_TO_MINE = 5;
	const int MINERAL_WORTH = 5;
	const int PRICE_WORKER = 5;
	const int PRICE_SOLDIER = 10;

	const int ROTATION_LERP_SPEED = 1500.0f;
	
	const float MINERAL_CARRY_SCALE = 0.25f;

	const float MINERAL_GAIN_ANIM_DURATION = 2.0f;

	const int EXPLOSION_ANIM_FRAMES = 12;
	const float EXPLOSION_ANIM_DURATION = 0.35f;

}
