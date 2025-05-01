#include "scene_main.h"

#include "imgui.h"
#include "raw_input.h"
#include "assets.h"
#include "game_constants.h"
#include "window.h"
#include "imgui_convert.h"
#include "input.h"
#include "debug.h"
#include "camera.h"
#include "rand.h"

float messageTime = 0;

const float mineralsizesmall = 24.f;

enum EntityType
{
	NONE = 0,

	PLAYER,
	MINERAL,
	HQ,
	SHOT,
	WORKER,
	SOLDIER,

	MINERAL_GAIN,

	EXPLOSION_PLAYER,
	EXPLOSION_MINION,

	ENTITY_TYPE_SIZE
};

enum EntityState
{
	IDLE = 0,
	GOING_MINERAL,
	MINING,
	GOING_BASE,
	WAIT,
};

struct GameData
{
	float gameTime = 0;

	EntityType ent_type[GameConstants::MAX_ENTITIES];

	int ent_player[GameConstants::MAX_ENTITIES];
	float ent_hp[GameConstants::MAX_ENTITIES];
	float ent_x[GameConstants::MAX_ENTITIES];
	float ent_y[GameConstants::MAX_ENTITIES];

	vec ent_size[GameConstants::MAX_ENTITIES];
	vec ent_size_rot[GameConstants::MAX_ENTITIES];

	float ent_spd_x[GameConstants::MAX_ENTITIES];
	float ent_spd_y[GameConstants::MAX_ENTITIES];
	float ent_rot[GameConstants::MAX_ENTITIES];
	float ent_scale[GameConstants::MAX_ENTITIES];

	EntityType ent_parent_type[GameConstants::MAX_ENTITIES];

	EntityType ent_carry[GameConstants::MAX_ENTITIES];

	EntityState ent_state[GameConstants::MAX_ENTITIES];

	vec ent_destination[GameConstants::MAX_ENTITIES];

	int ent_target[GameConstants::MAX_ENTITIES];

	float ent_cooldown_shoot[GameConstants::MAX_ENTITIES];

	float ent_timer_mining[GameConstants::MAX_ENTITIES];

	char  ent_collision_matrix[GameConstants::MAX_ENTITIES][GameConstants::MAX_ENTITIES];

	float ent_timer_last_carry[GameConstants::MAX_ENTITIES];

	char is_moving[GameConstants::MAX_ENTITIES];



	int money[2];

	SDL_Color player_color[2];
	float player_respawn_time[2];

	int tutorial_falso_step[2];

	int id_player[2];

	int id_mineral[2][2];

	int id_hq[2];

	int winner;

	int started;
};

GameData* gd = new GameData();

SDL_Color ColorBlue = { 0,0,255,255 };
SDL_Color ColorGreen = { 0,255,0,255};

float Lerp(float from, float to, float t)
{
	if (t > 1) t = 1;
	if (t < -1) t = -1;
	return (from)+(to - from) * t;
}


GPU_Image* GetSpriteForType(EntityType t, int ent_player, int anim_frame = 0) {
	switch (t)
	{
	case PLAYER:
		return ent_player ? Assets::spr_p2_ship : Assets::spr_p1_ship;
	case MINERAL:
		return Assets::spr_mineral;
	case HQ:
		return ent_player ? Assets::spr_p2_hq : Assets::spr_p1_hq;
	case SHOT:
		return ent_player ? Assets::spr_p2_shot : Assets::spr_p1_shot;
	case WORKER:
		return ent_player ? Assets::spr_p2_worker : Assets::spr_p1_worker;
	case SOLDIER:
		return ent_player ? Assets::spr_p2_soldier : Assets::spr_p1_soldier;
	case EntityType::EXPLOSION_MINION:
		return Assets::spr_sheet_explosion[anim_frame];
	default:
		return Assets::spr_mineral;
	}
}

int GetNextEntityID(GameData* gd)
{
	for (int i = 0; i < GameConstants::MAX_ENTITIES; ++i)
	{
		if (gd->ent_type[i] == EntityType::NONE)
		{
			return i;
		}
	}

	assert(false); //"emosido enganyado"
	return -1;
}

int AddEntity(GameData* gd, EntityType type, float x, float y, int player = -1, float rot = 0, EntityType parent_type = EntityType::NONE)
{
	int id = GetNextEntityID(gd);

	gd->ent_type[id] = type;
	gd->ent_hp[id] = 1000;
	gd->ent_player[id] = player;
	gd->ent_x[id] = x;
	gd->ent_y[id] = y;
	gd->ent_spd_x[id] = 0.f;
	gd->ent_spd_y[id] = 0.f;
	gd->ent_rot[id] = rot;
	gd->ent_scale[id] = 1.f;

	gd->ent_carry[id] = EntityType::NONE;
	gd->ent_timer_mining[id] = 0;

	gd->ent_cooldown_shoot[id] = -1;

	gd->ent_state[id] = EntityState::IDLE;

	GPU_Image* spr = GetSpriteForType(type, player);
	vec size = vec(spr->w, spr->h);

	gd->ent_size[id].x = (float)size.x;
	gd->ent_size[id].y = (float)size.y;

	gd->ent_target[id] = -1;

	gd->is_moving[id] = false;
	gd->ent_parent_type[id] = parent_type;

	switch (type)
	{
	case EntityType::PLAYER:
	case EntityType::SHOT:
	{
		gd->ent_size[id].x = (float)size.x * 0.65f;
		gd->ent_size[id].y = (float)size.y * 0.65f;

	} break;

	case EntityType::HQ:
	{
		gd->ent_size[id].y = (float)size.y * 0.65f;
	} break;

	case EntityType::WORKER:
	{
		gd->ent_size[id].x = (float)size.x * 0.35f;
		gd->ent_size[id].y = (float)size.y * 0.35f;
	} break;

	case EntityType::SOLDIER:
	{
		gd->ent_size[id].x = (float)size.x * 0.35f;
		gd->ent_size[id].y = (float)size.y * 0.35f;
		gd->ent_state[id] = EntityState::GOING_BASE;
	} break;
	}

	if (type == EntityType::SHOT) {
		gd->ent_cooldown_shoot[id] = GameConstants::BULLET_LIFETIME;

		if (Rand::OnceEvery(2)) Assets::sound_laser[0].Play();
		else Assets::sound_laser[1].Play();


	}

	return id;
}

void UpdateShot(GameData* gd, float dt, int id)
{

	gd->ent_cooldown_shoot[id] -= dt;
	if (gd->ent_cooldown_shoot[id] < 0) {
		gd->ent_type[id] = EntityType::NONE;
	}

	float dir_x = cos(Angles::DegsToRads(gd->ent_rot[id] - 90));
	float dir_y = sin(Angles::DegsToRads(gd->ent_rot[id] - 90));

	float speed = GameConstants::BULLET_SPEED;

	if (gd->ent_parent_type[id] == EntityType::HQ)
	{
		speed = GameConstants::BULLET_SPEED_HQ;
	}

	gd->ent_x[id] += speed * dir_x * dt;
	gd->ent_y[id] += speed * dir_y * dt;

	if (gd->ent_x[id] < -100 || gd->ent_x[id] > Window::GAME_WIDTH + 100 ||
		gd->ent_y[id] < -100 || gd->ent_y[id] > Window::GAME_HEIGHT + 100) {
		gd->ent_type[id] = EntityType::NONE;
	}

	for (int i = 0; i < GameConstants::MAX_ENTITIES; i++) {
		bool collides = gd->ent_collision_matrix[id][i];
		if (!collides) {
			continue;
		}
		EntityType type = gd->ent_type[i];
		int player = gd->ent_player[i];
		if (player == gd->ent_player[id]) {
			continue;
		}
		if (type == EntityType::SHOT) {
			if (gd->ent_parent_type[i] != EntityType::HQ) {
				gd->ent_type[i] = EntityType::NONE;
			}
			if (gd->ent_parent_type[id] != EntityType::HQ) {
				gd->ent_type[id] = EntityType::NONE;
			}
		}

		EntityType myParent = gd->ent_parent_type[id];
		if (type == EntityType::HQ || type == EntityType::PLAYER || type == EntityType::WORKER || type == EntityType::SOLDIER) {

			if (type == EntityType::WORKER || type == EntityType::SOLDIER) {
				if (myParent == EntityType::HQ) {
					gd->ent_hp[i] -= 500;
				}
				else {
					gd->ent_hp[i] -= 334;
				}
			}
			else if (type == EntityType::PLAYER) {
				gd->ent_hp[i] -= 100;
			}
			else if (type == EntityType::HQ) {
				gd->ent_hp[i] -= 3;
			}
			gd->ent_type[id] = EntityType::NONE;
			if (type == EntityType::PLAYER || type == EntityType::WORKER || type == EntityType::SOLDIER) {
				const int KNOCKBACK = 10000;
				gd->ent_spd_x[i] += KNOCKBACK * dir_x * dt;
				gd->ent_spd_y[i] += KNOCKBACK * dir_y * dt;

			}

		}
	}
}

void EntityRotateTo(GameData* gd, int id, float dt, float rot)
{
	while (rot > 360.f) {
		rot -= 360.f;
	}

	if (rot < 0.0f) rot += 360.0f;

	if (gd->ent_rot[id] > 270.0f && rot < 90.0f)
	{
		gd->ent_rot[id] -= 360.0f;
	}
	if (gd->ent_rot[id] < 90.0f && rot > 270.0f)
	{
		gd->ent_rot[id] += 360.0f;
	}

	//gd->ent_rot[id] = Lerp(gd->ent_rot[id], rot, 20.0f*dt);
	bool sign = (rot - gd->ent_rot[id] > 0.0f);
	float new_rot = gd->ent_rot[id] + GameConstants::ROTATION_LERP_SPEED * dt * (sign ? 1 : -1);
	if ((sign && new_rot > rot) || (!sign && new_rot < rot))
	{
		new_rot = rot;
	}
	gd->ent_rot[id] = new_rot;
}

void UpdatePlayer(GameData* gd, float dt, int id)
{
	int num_player = gd->ent_player[id];

	vec stick_L = GamePad::AnalogStick::Left.get(num_player);
	vec stick_R = GamePad::AnalogStick::Right.get(num_player);

	float length_L = stick_L.Length();
	float length_R = stick_R.Length();
	if (length_L < 30)
	{
		stick_L = vec(0, 0);
	}
	if (length_R < 30)
	{
		stick_R = vec(0, 0);
	}
	vec stickLNorm = stick_L.Normalized();
	vec stickRNorm = stick_R.Normalized();

	gd->ent_spd_x[id] += stickLNorm.x * GameConstants::PLAYER_ACCEL * dt;
	gd->ent_spd_y[id] += stickLNorm.y * GameConstants::PLAYER_ACCEL * dt;

	// Limit speed
	vec speed(gd->ent_spd_x[id], gd->ent_spd_y[id]);
	vec speed_norm = speed.Normalized();
	float speed_length = speed.Length();

	if (speed_length > GameConstants::PLAYER_MAX_SPEED)
	{
		speed_length = GameConstants::PLAYER_MAX_SPEED;
	}
	else if (speed_length < -GameConstants::PLAYER_MAX_SPEED)
	{
		speed_length = -GameConstants::PLAYER_MAX_SPEED;
	}

	gd->ent_spd_x[id] = speed_norm.x * speed_length;
	gd->ent_spd_y[id] = speed_norm.y * speed_length;

	gd->ent_x[id] = gd->ent_x[id] + gd->ent_spd_x[id] * dt;
	gd->ent_y[id] = gd->ent_y[id] + gd->ent_spd_y[id] * dt;

	gd->is_moving[id] = length_L > 30 && (
		length_R < 30 || stickLNorm.Dot(stickRNorm) > 0.5
		);

	//Out of Screen
	if (gd->ent_x[id] < 0)
	{
		gd->ent_x[id] = 0;
	}
	if (gd->ent_x[id] > Window::GAME_WIDTH)
	{
		gd->ent_x[id] = Window::GAME_WIDTH;
	}

	if (gd->ent_y[id] < 0)
	{
		gd->ent_y[id] = 0;
	}
	if (gd->ent_y[id] > Window::GAME_HEIGHT)
	{
		gd->ent_y[id] = Window::GAME_HEIGHT;
	}


	bool is_moving = false;

	if (std::abs(stick_R.x) > 0 || std::abs(stick_R.y) > 0)
	{
		float rot = (atan2(stick_R.y, stick_R.x) * 180 / 3.1415f) + 90.0f;
		EntityRotateTo(gd, id, dt, rot);
	}
	else if (std::abs(stick_L.x) > 0 || std::abs(stick_L.y) > 0)
	{
		is_moving = true;

		float rot = (atan2(stick_L.y, stick_L.x) * 180 / 3.1415f + 90.0f);
		if (rot < 0.0f) rot += 360.0f;

		EntityRotateTo(gd, id, dt, rot);
	}
	if (!is_moving)
	{
		gd->ent_spd_x[id] = Lerp(gd->ent_spd_x[id], 0, 3.0f * dt);
		gd->ent_spd_y[id] = Lerp(gd->ent_spd_y[id], 0, 3.0f * dt);

		if (std::abs(gd->ent_spd_x[id]) < 10)
		{
			gd->ent_spd_x[id] = 0;
		}
		if (std::abs(gd->ent_spd_y[id]) < 10)
		{
			gd->ent_spd_y[id] = 0;
		}
	}

	gd->ent_timer_last_carry[id] += dt;

	// Mining Mineralsitos
	if ((gd->ent_collision_matrix[id][gd->id_mineral[num_player][0]]
		|| gd->ent_collision_matrix[id][gd->id_mineral[num_player][1]])
		&&
		gd->ent_carry[id] == EntityType::NONE
		)
	{
		gd->ent_timer_mining[id] += dt;
		if (gd->ent_timer_mining[id] > GameConstants::TIME_TO_MINE)
		{
			gd->ent_carry[id] = EntityType::MINERAL;
			gd->ent_timer_mining[id] = 0;
			gd->ent_timer_last_carry[id] = 0;

			if (gd->tutorial_falso_step[num_player] == 0)
			{
				gd->tutorial_falso_step[num_player]++;
			}
		}
	}
	else
	{
		gd->ent_timer_mining[id] = 0;
	}

	// Player on HQ ...
	if (gd->ent_collision_matrix[id][gd->id_hq[gd->ent_player[id]]])
	{
		if (num_player) {
			ImGui::SetNextWindowPosInGameCoords(vec(gd->ent_x[id], gd->ent_y[id]), vec(5, -110));
		}
		else {
			ImGui::SetNextWindowPosInGameCoords(vec(gd->ent_x[id], gd->ent_y[id]), vec(-230, -110));
		}
		std::string name = "hq" + std::to_string(num_player);
		ImGui::Begin(name.c_str(), 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowFontScale(2.f);
		ImGui::Image(im(Assets::buttonA), ImVec2(Assets::buttonA->w, Assets::buttonA->h));
		ImGui::SameLine();
		ImGui::Text("Soldier ");
		ImGui::SameLine();
		ImGui::Image(im(Assets::spr_mineral), ImVec2(mineralsizesmall, mineralsizesmall));
		static std::string soldier_price = std::to_string(GameConstants::PRICE_SOLDIER);
		ImGui::SameLine();
		ImGui::Text(soldier_price.c_str());

		ImGui::Image(im(Assets::buttonB), ImVec2(Assets::buttonB->w, Assets::buttonB->h));
		ImGui::SameLine();
		ImGui::Text("Worker  ");
		ImGui::SameLine();
		ImGui::Image(im(Assets::spr_mineral), ImVec2(mineralsizesmall, mineralsizesmall));
		static std::string worker_price = std::to_string(GameConstants::PRICE_WORKER);
		ImGui::SameLine();
		ImGui::Text(worker_price.c_str());
		ImGui::End();


		//Return minerals
		if (gd->ent_carry[id] == EntityType::MINERAL)
		{
			gd->ent_carry[id] = EntityType::NONE;
			gd->money[gd->ent_player[id]] += 5;

			AddEntity(gd, EntityType::MINERAL_GAIN, gd->ent_x[id], gd->ent_y[id], num_player);

			if (gd->tutorial_falso_step[num_player] == 1)
			{
				gd->tutorial_falso_step[num_player]++;
			}
		}


		//Buy things Purchase
		if (Input::IsJustPressed(num_player, GameKeys::ACCEPT))
		{
			if (gd->money[num_player] >= GameConstants::PRICE_WORKER)
			{
				gd->money[num_player] -= GameConstants::PRICE_WORKER;

				float hq_x = gd->ent_x[gd->id_hq[num_player]];
				float hq_y = gd->ent_y[gd->id_hq[num_player]];

				int id_worker = AddEntity(gd, EntityType::WORKER, hq_x, hq_y, num_player);
			}
		}

		if (Input::IsJustPressed(num_player, GameKeys::CANCEL))
		{
			if (gd->money[num_player] >= GameConstants::PRICE_SOLDIER)
			{
				gd->money[num_player] -= GameConstants::PRICE_SOLDIER;

				float hq_x = gd->ent_x[gd->id_hq[num_player]];
				float hq_y = gd->ent_y[gd->id_hq[num_player]];

				hq_x += Rand::rollf(30.0f);
				hq_y += Rand::rollf(30.0f);

				float hq_other_x = gd->ent_x[gd->id_hq[(num_player + 1) % 2]];
				float hq_other_y = gd->ent_y[gd->id_hq[(num_player + 1) % 2]];

				if (num_player == 1)
				{
					hq_other_x += Rand::rollf(80.0f, 280.0f);
					hq_other_y += Rand::rollf(-120.0f, 120.0f);
				}
				else
				{
					hq_other_x -= Rand::rollf(80.0f, 280.0f);
					hq_other_y -= Rand::rollf(-120.0f, 120.0f);
				}

				int id_soldier = AddEntity(gd, EntityType::SOLDIER, hq_x, hq_y, num_player);

				float rot = -60 + Rand::rollf(120) + 90;
				if (rot < 0) rot += 360;

				//gd->ent_rot[id_soldier] = rot;

				gd->ent_destination[id_soldier].x = hq_other_x;
				gd->ent_destination[id_soldier].y = hq_other_y;

				gd->ent_spd_x[id_soldier] = 150 * cos(Angles::DegsToRads(rot - 90));
				gd->ent_spd_y[id_soldier] = 150 * sin(Angles::DegsToRads(rot - 90));
			}
		}
	}


	if (gd->ent_cooldown_shoot[id] < 0.f) {

		if (GamePad::Trigger::Right.IsPressed(num_player))
		{
			gd->ent_cooldown_shoot[id] = GameConstants::COOLDOWN_SHOTS;

			float dir_x = cos(Angles::DegsToRads(gd->ent_rot[id]));
			float dir_y = sin(Angles::DegsToRads(gd->ent_rot[id]));

			const int DISTANSITA_CANYONES = 22;
			const float ANGULITO_JORDI_PUJOL = 0.33f;

			AddEntity(gd, EntityType::SHOT, gd->ent_x[id] + DISTANSITA_CANYONES * dir_x, gd->ent_y[id] + DISTANSITA_CANYONES * dir_y, num_player, gd->ent_rot[id] - ANGULITO_JORDI_PUJOL, EntityType::PLAYER);
			AddEntity(gd, EntityType::SHOT, gd->ent_x[id] - DISTANSITA_CANYONES * dir_x, gd->ent_y[id] - DISTANSITA_CANYONES * dir_y, num_player, gd->ent_rot[id] + ANGULITO_JORDI_PUJOL, EntityType::PLAYER);
		}

	}
	else {
		gd->ent_cooldown_shoot[id] -= dt;
	}
}


void UpdateWorker(GameData* gd, float dt, int id)
{

	int num_player = gd->ent_player[id];

	gd->ent_timer_last_carry[id] += dt;

	switch (gd->ent_state[id])
	{
	case IDLE:
	{
		//Search new Mineral
		int id_mineral = gd->id_mineral[num_player][Rand::OnceEvery(2)];

		float rnd_x = Rand::rollf(gd->ent_size[id_mineral].x);
		float rnd_y = Rand::rollf(gd->ent_size[id_mineral].y);

		gd->ent_destination[id].x = gd->ent_x[id_mineral] + rnd_x - gd->ent_size[id_mineral].x * 0.5f;
		gd->ent_destination[id].y = gd->ent_y[id_mineral] + rnd_y - gd->ent_size[id_mineral].y * 0.5f;

		gd->ent_target[id] = id_mineral;

		gd->ent_state[id] = EntityState::GOING_MINERAL;
	} break;

	case GOING_MINERAL:
	{
		float dir_x = (gd->ent_destination[id].x - gd->ent_x[id]);
		float dir_y = (gd->ent_destination[id].y - gd->ent_y[id]);
		vec dir(dir_x, dir_y);
		vec dst(gd->ent_destination[id].x, gd->ent_destination[id].y);
		vec pos(gd->ent_x[id], gd->ent_y[id]);
		vec norm_dir = dir.Normalized();

		gd->ent_x[id] += norm_dir.x * dt * 50.0f;
		gd->ent_y[id] += norm_dir.y * dt * 50.0f;

		gd->ent_rot[id] = Angles::RadsToDegs(atan2(norm_dir.y, norm_dir.x)) + 90.0f;

		if (Distance(pos, dst) < 5.0f)
		{
			gd->ent_state[id] = EntityState::MINING;
			gd->ent_timer_mining[id] = 0;
		}
	} break;

	case MINING:
	{
		gd->ent_timer_mining[id] += dt;
		if (gd->ent_timer_mining[id] > GameConstants::TIME_TO_MINE)
		{
			gd->ent_carry[id] = EntityType::MINERAL;
			gd->ent_timer_mining[id] = 0;
			gd->ent_state[id] = EntityState::GOING_BASE;
			int id_hq = gd->id_hq[num_player];
			//gd->ent_destination[id] = vec(gd->ent_x[id_hq], gd->ent_y[id_hq]) ;

			float rnd_x = Rand::rollf(gd->ent_size[id_hq].x);
			float rnd_y = Rand::rollf(gd->ent_size[id_hq].y);

			gd->ent_destination[id].x = gd->ent_x[id_hq] + rnd_x - gd->ent_size[id_hq].x * 0.5f;
			gd->ent_destination[id].y = gd->ent_y[id_hq] + rnd_y - gd->ent_size[id_hq].y * 0.5f;

			gd->ent_timer_last_carry[id] = 0.0f;
		}

		float dir_x = (gd->ent_x[gd->ent_target[id]] - gd->ent_x[id]);
		float dir_y = (gd->ent_y[gd->ent_target[id]] - gd->ent_y[id]);
		vec dir(dir_x, dir_y);
		vec norm_dir = dir.Normalized();
		gd->ent_rot[id] = Angles::RadsToDegs(atan2(norm_dir.y, norm_dir.x)) + 90.0f;

	} break;

	case GOING_BASE:
	{
		float dir_x = (gd->ent_destination[id].x - gd->ent_x[id]);
		float dir_y = (gd->ent_destination[id].y - gd->ent_y[id]);

		vec dir(dir_x, dir_y);
		vec dst(gd->ent_destination[id].x, gd->ent_destination[id].y);

		vec pos(gd->ent_x[id], gd->ent_y[id]);

		vec norm_dir = dir.Normalized();

		gd->ent_x[id] += norm_dir.x * dt * 50.0f;
		gd->ent_y[id] += norm_dir.y * dt * 50.0f;

		gd->ent_rot[id] = Angles::RadsToDegs(atan2(norm_dir.y, norm_dir.x)) + 90.0f;

		if (Distance(pos, dst) < 5.0f)
		{
			gd->ent_state[id] = EntityState::IDLE;
			gd->ent_carry[id] = EntityType::NONE;
			gd->money[num_player] += GameConstants::MINERAL_WORTH;
			gd->ent_timer_mining[id] = 0;
			gd->ent_target[id] = -1;

			AddEntity(gd, EntityType::MINERAL_GAIN, gd->ent_x[id], gd->ent_y[id], num_player);
		}
	} break;
	}
}

void UpdateSoldier(GameData* gd, float dt, int id)
{

	int num_player = gd->ent_player[id];
	vec pos(gd->ent_x[id], gd->ent_y[id]);

	int id_target = gd->ent_target[id];
	if (id_target > -1) {

		EntityType target_type = gd->ent_type[id_target];
		if (target_type == EntityType::HQ || target_type == EntityType::SOLDIER || target_type == EntityType::PLAYER) {

		}
		else {
			gd->ent_target[id] = -1;
		}

		vec dir_target = vec(gd->ent_x[id_target], gd->ent_y[id_target]) - vec(gd->ent_x[id], gd->ent_y[id]);
		if (dir_target.Length() <= GameConstants::SOLDIER_ATTACK_RADIUS)
		{
			int id_target = gd->ent_target[id];
			float rot_target = Angles::RadsToDegs(atan2(gd->ent_y[id_target] - pos.y, gd->ent_x[id_target] - pos.x)) + 90;
			EntityRotateTo(gd, id, dt, rot_target);
		}
		else {
			id_target = -1;
		}
	}

	if (id_target == -1) {

		vec dir_hq = gd->ent_destination[id] - vec(gd->ent_x[id], gd->ent_y[id]);

		float rot = Angles::RadsToDegs(atan2(dir_hq.y, dir_hq.x)) + 90.0f;

		gd->ent_spd_x[id] += dir_hq.Normalized().x * GameConstants::SOLDIER_MAX_SPEED / 10.f;
		gd->ent_spd_y[id] += dir_hq.Normalized().y * GameConstants::SOLDIER_MAX_SPEED / 10.f;

		if (gd->ent_spd_x[id] > GameConstants::SOLDIER_MAX_SPEED)
		{
			gd->ent_spd_x[id] = GameConstants::SOLDIER_MAX_SPEED;
		}
		if (gd->ent_spd_x[id] < -GameConstants::SOLDIER_MAX_SPEED)
		{
			gd->ent_spd_x[id] = -GameConstants::SOLDIER_MAX_SPEED;
		}
		if (gd->ent_spd_y[id] > GameConstants::SOLDIER_MAX_SPEED)
		{
			gd->ent_spd_y[id] = GameConstants::SOLDIER_MAX_SPEED;
		}
		if (gd->ent_spd_y[id] < -GameConstants::SOLDIER_MAX_SPEED)
		{
			gd->ent_spd_y[id] = -GameConstants::SOLDIER_MAX_SPEED;
		}

		float new_x = gd->ent_x[id] + gd->ent_spd_x[id] * dt;
		float new_y = gd->ent_y[id] + gd->ent_spd_y[id] * dt;

		gd->ent_x[id] = new_x;
		gd->ent_y[id] = new_y;

		EntityRotateTo(gd, id, dt, rot);
	}

	int id_nearest = -1;
	float dist_nearest = 10000000.0f;
	for (int i = 0; i < GameConstants::MAX_ENTITIES; ++i)
	{
		if (gd->ent_type[i] == EntityType::HQ || gd->ent_type[i] == EntityType::SOLDIER || gd->ent_type[i] == EntityType::PLAYER)
		{
			if (gd->ent_player[i] != gd->ent_player[id])
			{
				float dist = Distance(vec(gd->ent_x[i], gd->ent_y[i]), pos);
				if (dist < dist_nearest)
				{
					dist_nearest = dist;
					id_nearest = i;
				}
			}
		}
	}

	if (gd->ent_cooldown_shoot[id] < 0.0f)
	{

		if (id_nearest != -1 && dist_nearest <= GameConstants::SOLDIER_ATTACK_RADIUS + 10)
		{
			float rot_shoot = Angles::RadsToDegs(atan2(gd->ent_y[id_nearest] - pos.y, gd->ent_x[id_nearest] - pos.x)) + 90;
			int shotecito = AddEntity(gd, EntityType::SHOT, pos.x, pos.y, num_player, rot_shoot, EntityType::SOLDIER);
			gd->ent_cooldown_shoot[id] = 1.0f;
			EntityRotateTo(gd, shotecito, dt, rot_shoot);
			//gd->ent_rot[id] = rot_shoot;
			gd->ent_target[id] = id_nearest;
		}
	}
	else {
		gd->ent_cooldown_shoot[id] -= dt;
	}
}



bool IsEntityCollision(GameData* gd, int e1, int e2)
{
	if ((gd->ent_x[e1] + gd->ent_size_rot[e1].x / 2) < (gd->ent_x[e2] - gd->ent_size_rot[e2].x / 2)) return false; // "other" is on the far right
	if ((gd->ent_x[e2] + gd->ent_size_rot[e2].x / 2) < (gd->ent_x[e1] - gd->ent_size_rot[e1].x / 2)) return false; //"other" is on the far left

	if ((gd->ent_y[e1] + gd->ent_size_rot[e1].y / 2) < (gd->ent_y[e2] - gd->ent_size_rot[e2].y / 2)) return false; // "other" is up
	if ((gd->ent_y[e2] + gd->ent_size_rot[e2].y / 2) < (gd->ent_y[e1] - gd->ent_size_rot[e1].y / 2)) return false; // "other" is down

	return true;
}

void UpdateHQ(GameData* gd, float dt, int id)
{

	if (gd->ent_cooldown_shoot[id] > 0.f) {
		gd->ent_cooldown_shoot[id] -= dt;
	}
	else {

		int player = gd->ent_player[id];
		int other_player = 1 - player;
		int other_player_ent = gd->id_player[other_player];

		vec pos(gd->ent_x[id], gd->ent_y[id]);

		int target = -1;

		float min_dist = 99999999.f;
		for (int i = 0; i < GameConstants::MAX_ENTITIES; i++) {
			if (gd->ent_type[i] == EntityType::SOLDIER && gd->ent_player[i] == other_player) {
				vec otherPos(vec(gd->ent_x[i], gd->ent_y[i]));
				float length = (pos - otherPos).Length();
				if (length < GameConstants::HQ_ATTACK_RADIUS) {
					if (length < min_dist) {
						target = i;
						min_dist = length;
					}
				}
			}
		}
		if (target < 0) {
			vec otherPlayerPos(vec(gd->ent_x[other_player_ent], gd->ent_y[other_player_ent]));
			if (gd->player_respawn_time[other_player] <= 0.f) { //The player is not dead
				if (player == 0 && otherPlayerPos.x < pos.x + GameConstants::HQ_ATTACK_RADIUS
					||
					player == 1 && otherPlayerPos.x > pos.x - GameConstants::HQ_ATTACK_RADIUS
					) {
					target = other_player_ent;
				}
			}
		}

		if (target > -1) {
			gd->ent_cooldown_shoot[id] = GameConstants::HQ_COOLDOWN_SHOTS;
			int shotecito = AddEntity(gd, EntityType::SHOT, pos.x, pos.y, player, Angles::RadsToDegs(atan2(gd->ent_y[target] - pos.y, gd->ent_x[target] - pos.x)) + 90, EntityType::HQ);
			gd->ent_cooldown_shoot[shotecito] = GameConstants::HQ_BULLET_LIFETIME;
			gd->ent_scale[shotecito] = 1.9f;
		}
	}
}

bool HasSteeringBehaviourXD(EntityType type)
{
	return
		type == EntityType::PLAYER ||
		type == EntityType::SOLDIER ||
		type == EntityType::WORKER;
}

void UpdateEntities(GameData* gd, float dt)
{
	for (int i = 0; i < GameConstants::MAX_ENTITIES; ++i)
	{
		if (gd->ent_type[i] == EntityType::NONE) continue;

		float x = gd->ent_size[i].x;
		float y = gd->ent_size[i].y;
		float rot = gd->ent_rot[i];

		gd->ent_size_rot[i].x = abs(x * cos(Angles::DegsToRads(rot))) + abs(y * sin(Angles::DegsToRads(rot)));
		gd->ent_size_rot[i].y = abs(x * sin(Angles::DegsToRads(rot))) + abs(y * cos(Angles::DegsToRads(rot)));

	}


	for (int i = 0; i < GameConstants::MAX_ENTITIES; ++i)
	{
		int t1 = gd->ent_type[i];

		//Collisions
		gd->ent_collision_matrix[i][i] = false;
		for (int j = i + 1; j < GameConstants::MAX_ENTITIES; ++j)
		{
			int t2 = gd->ent_type[j];
			if (t1 > t2)
			{
				int aux = t1;
				t1 = t2;
				t2 = aux;
			}

			gd->ent_collision_matrix[i][j] = false;
			gd->ent_collision_matrix[j][i] = false;

			if (gd->ent_type[j] == EntityType::NONE) continue;
			if (IsEntityCollision(gd, i, j))
			{
				gd->ent_collision_matrix[i][j] = true;
				gd->ent_collision_matrix[j][i] = true;

				if (HasSteeringBehaviourXD(gd->ent_type[i])
					&& HasSteeringBehaviourXD(gd->ent_type[j]))
				{
					vec pos_i(gd->ent_x[i], gd->ent_y[i]);
					vec pos_j(gd->ent_x[j], gd->ent_y[j]);

					vec force_i_to_j = (pos_i - pos_j).Normalized();
					vec force_j_to_i = (pos_j - pos_i).Normalized();

					float dist = 1.f / sqrt(Distance(pos_i, pos_j));

					gd->ent_x[i] += force_i_to_j.x * dist * dt;
					gd->ent_y[i] += force_i_to_j.y * dist * dt;

					gd->ent_x[j] += force_j_to_i.x * dist * dt;
					gd->ent_y[j] += force_j_to_i.y * dist * dt;
				}

			}
		}


		switch (gd->ent_type[i])
		{
		case EntityType::PLAYER:
		{
			UpdatePlayer(gd, dt, i);
		} break;

		case EntityType::SHOT:
		{
			UpdateShot(gd, dt, i);
		} break;

		case EntityType::WORKER:
		{
			UpdateWorker(gd, dt, i);
		} break;

		case EntityType::SOLDIER:
		{
			UpdateSoldier(gd, dt, i);
		} break;

		case EntityType::HQ:
		{
			UpdateHQ(gd, dt, i);
		} break;

		case EntityType::MINERAL_GAIN:
		{
			gd->ent_y[i] -= 50 * (dt);
			gd->ent_x[i] += sin(gd->gameTime * 10.0f) * 0.75f;
			gd->ent_timer_mining[i] += dt;
			if (gd->ent_timer_mining[i] > GameConstants::MINERAL_GAIN_ANIM_DURATION)
			{
				gd->ent_type[i] = EntityType::NONE;
			}
		} break;

		case EntityType::EXPLOSION_MINION:
		{
			gd->ent_timer_mining[i] += dt;
			if (gd->ent_timer_mining[i] > GameConstants::EXPLOSION_ANIM_DURATION)
			{
				gd->ent_type[i] = EntityType::NONE;
			}
		} break;
		}


		if (gd->ent_hp[i] <= 0 && gd->ent_type[i] != EntityType::NONE)
		{

			if (gd->ent_type[i] == EntityType::PLAYER)
			{
				gd->player_respawn_time[gd->ent_player[i]] = sqrt(0.25f * gd->gameTime);
			}

			if (gd->ent_type[i] == EntityType::HQ)
			{
				gd->winner = 1 - gd->ent_player[i];
			}

			if (gd->ent_type[i] == EntityType::SOLDIER
				|| gd->ent_type[i] == EntityType::WORKER)
			{
				AddEntity(gd, EntityType::EXPLOSION_MINION, gd->ent_x[i], gd->ent_y[i]);
			}

			gd->ent_type[i] = EntityType::NONE;

			if (Rand::OnceEvery(2)) Assets::sound_explosion[0].Play();
			else Assets::sound_explosion[1].Play();

		}
	}

	for (int p = 0; p < 2; p++) {
		if (gd->player_respawn_time[p] > 0.f) {
			gd->player_respawn_time[p] -= dt;
			if (gd->player_respawn_time[p] <= 0.f) {
				float x = 400;
				float r = 90;
				if (p == 1) {
					x = Window::GAME_WIDTH - x;
					r += 180;
				}
				gd->id_player[p] = AddEntity(gd, EntityType::PLAYER, x, Window::GAME_HEIGHT / 2, p, r);
			}
		}
	}

}



void InitGame(GameData* gd)
{
	messageTime = 0;

	for (int i = 0; i < GameConstants::MAX_ENTITIES; ++i)
	{
		gd->ent_type[i] = EntityType::NONE;
	}
	gd->money[0] = 0;
	gd->money[1] = 0;

	gd->id_mineral[0][0] = AddEntity(gd, EntityType::MINERAL, 140, 250, 0);
	gd->id_mineral[0][1] = AddEntity(gd, EntityType::MINERAL, 140, 850, 0);

	gd->id_mineral[1][0] = AddEntity(gd, EntityType::MINERAL, Window::GAME_WIDTH - 140, 250, 1);
	gd->id_mineral[1][1] = AddEntity(gd, EntityType::MINERAL, Window::GAME_WIDTH - 140, 850, 1);

	gd->id_hq[0] = AddEntity(gd, EntityType::HQ, 250, 530, 0);
	gd->id_hq[1] = AddEntity(gd, EntityType::HQ, Window::GAME_WIDTH - 250, 530, 1);

	gd->id_player[0] = AddEntity(gd, EntityType::PLAYER, 400, Window::GAME_HEIGHT / 2, 0, 90);
	gd->id_player[1] = AddEntity(gd, EntityType::PLAYER, Window::GAME_WIDTH - 400, Window::GAME_HEIGHT / 2, 1, 270);

	gd->winner = -1;
	gd->started = false;

	gd->player_color[0] = ColorBlue;
	gd->player_color[1] = ColorGreen;

	gd->player_respawn_time[0] = -1;
	gd->player_respawn_time[1] = -1;

	gd->tutorial_falso_step[0] = 0;
	gd->tutorial_falso_step[1] = 0;
}

//DrawEntities
void RenderEntities() {

	for (int p = 0; p < 2; p++) {
		if (gd->player_respawn_time[p] > 0.f) {

			Text txt_money(Assets::font_80, Assets::font_80_outline_2);
			txt_money.SetString(std::to_string((int)ceil(gd->player_respawn_time[p])));
			txt_money.SetFillColor(p ? ColorGreen : ColorBlue);
			txt_money.SetOutlineColor(255,255,255);

			float x = 400;
			if (p == 1) {
				x = Window::GAME_WIDTH - x;
			}
			vec textPos(x, Window::GAME_HEIGHT / 2);
			vec textSize = txt_money.Size();
			float scale = 0.6 + 2 * (ceil(gd->player_respawn_time[p]) - gd->player_respawn_time[p]);

			Window::Draw(txt_money, textPos)
				.withOrigin(textSize / 2.f)
				.withScale(scale);
		}
	}


	for (int i = 0; i < GameConstants::MAX_ENTITIES; ++i)
	{
		vec pos;
		pos.x = gd->ent_x[i] - gd->ent_size[i].x * 0.25f;
		pos.y = gd->ent_y[i];

		if (gd->ent_type[i] == EntityType::MINERAL_GAIN)
		{
			float alpha = 255 - (gd->ent_timer_mining[i] / GameConstants::MINERAL_GAIN_ANIM_DURATION) * 255;

			vec spr_mineral_carry_pos = vec(gd->ent_x[i], gd->ent_y[i]);

			Text txt(Assets::font_30, Assets::font_30_outline_2);
			txt.SetString("+5");
			SDL_Color color = (gd->ent_player[i] == 0) ? ColorBlue : ColorGreen;
			txt.SetFillColor(color);
			txt.SetOutlineColor(255, 255, 255);

			Window::Draw(Assets::spr_mineral_carry, spr_mineral_carry_pos)
				.withScale(GameConstants::MINERAL_CARRY_SCALE)
				.withOriginAtSpriteCenter()
				.withColor(255, 255, 255, alpha);

			Window::Draw(txt, spr_mineral_carry_pos + vec(10, -20))
				.withOrigin(0, -txt.Size().y/2)
				.withScale(0.8f)
				.withColor(255, 255, 255, alpha);
		}
		else if (gd->ent_type[i] != EntityType::NONE)
		{
			int num_player = gd->ent_player[i];

			if (gd->ent_type[i] == EntityType::MINERAL && gd->tutorial_falso_step[num_player] == 0)
			{
				vec spr_arrow_down_pos(gd->ent_x[i], gd->ent_y[i] - gd->ent_size[i].y * 1.2f + sin(gd->gameTime * 4.0f) * 10.0f);
				Window::Draw(Assets::spr_arrow_down, spr_arrow_down_pos)
					.withOriginAtSpriteCenter()
					.withColor(255,255,255,192)
					.withScale(2);
			}

			if (gd->ent_type[i] == EntityType::HQ && gd->tutorial_falso_step[num_player] == 1)
			{
				vec spr_arrow_down_pos(gd->ent_x[i], gd->ent_y[i] - gd->ent_size[i].y * 1.2f + sin(gd->gameTime * 4.0f) * 10.0f);
				Window::Draw(Assets::spr_arrow_down, spr_arrow_down_pos)
					.withOriginAtSpriteCenter()
					.withColor(255, 255, 255, 192)
					.withScale(2);
			}

			if (gd->ent_type[i] == EntityType::PLAYER)
			{
				if (gd->is_moving[i])
				{
					vec spr_ship_fire_pos = vec(gd->ent_x[i], gd->ent_y[i]);
					Window::Draw(Assets::spr_ship_fire, spr_ship_fire_pos)
						.withRotationDegs(gd->ent_rot[i])
						.withOriginAtSpriteCenter();
				}
			}

			GPU_Image* sprite = GetSpriteForType(gd->ent_type[i], gd->ent_player[i]);
			if (gd->ent_type[i] == EntityType::EXPLOSION_MINION)
			{
				int frame = std::min(GameConstants::EXPLOSION_ANIM_FRAMES - 1, static_cast<int>((gd->ent_timer_mining[i] / GameConstants::EXPLOSION_ANIM_DURATION) * GameConstants::EXPLOSION_ANIM_FRAMES));
				sprite = GetSpriteForType(gd->ent_type[i], gd->ent_player[i], frame);
			}
			vec spritePos = vec(gd->ent_x[i], gd->ent_y[i]);

			float scale = 1.f;
			switch(gd->ent_type[i]) {
				case EntityType::SHOT:
					scale = gd->ent_scale[i];
					break;
				case EntityType::SOLDIER:
				case EntityType::WORKER:
					scale = 0.3f;
			}
			Window::Draw(sprite, spritePos)
				.withOriginAtSpriteCenter()
				.withRotationDegs(gd->ent_rot[i])
				.withScale(scale);

			if (gd->ent_hp[i] < 1000)
			{
				BoxBounds b(gd->ent_x[i] - gd->ent_size[i].x * 0.5f, gd->ent_y[i] - gd->ent_size[i].y - 5, (gd->ent_hp[i] / 1000.0f) * gd->ent_size[i].x * 1.2f, 3.0f);
				Window::DrawPrimitive::Rectangle(b, 1, { 255,255,255,255 });
				Window::DrawPrimitive::Rectangle(b, -1, { 255,0,0,255 });
			}

			if (gd->ent_type[i] == EntityType::PLAYER && gd->ent_timer_mining[i] > 0)
			{
				float angle = 360 * (gd->ent_timer_mining[i] / GameConstants::TIME_TO_MINE);
				if (angle > 3) {
					vec pos = vec(gd->ent_x[i], gd->ent_y[i]);
					Window::DrawPrimitive::Arc(pos, 46, 360 - 90, angle - 90, 13, { 255,255,255,255 });
					Window::DrawPrimitive::Arc(pos, 46, 360 - 90, angle - 90, 11, { 255,0,0,255 });
				}
			}

			if (gd->ent_carry[i] == EntityType::MINERAL)
			{
				float scale = GameConstants::MINERAL_CARRY_SCALE;
				float a = 1.0f;
				if (gd->ent_timer_last_carry[i] < 0.4f)
				{
					float t = gd->ent_timer_last_carry[i] / 0.4f;
					scale = GameConstants::MINERAL_CARRY_SCALE * 2.5f - t * GameConstants::MINERAL_CARRY_SCALE * 1.5f;
					a = gd->ent_timer_last_carry[i] / 0.2f;
					if (a > 1.0f) a = 1.0f;
				}
				vec spr_mineral_carry_pos = vec(
					gd->ent_x[i] + gd->ent_size[i].x * 0.5f * cos(Angles::DegsToRads(gd->ent_rot[i] - 90.0f)),
					gd->ent_y[i] + gd->ent_size[i].y * 0.5f * sin(Angles::DegsToRads(gd->ent_rot[i] - 90.0f))
				);

				//if ((gd->gameTime* 1000)%400 < 200)
				{
					Window::Draw(Assets::spr_mineral_carry, spr_mineral_carry_pos)
						.withColor(255, 255, 255, 255 * a)
						.withOriginAtSpriteCenter()
						.withScale(scale);
				}
			}

		}
	}

	if (gd->winner != -1)
	{
		float ang = sin(messageTime * 2) * 10;
		float sc = 1;
		if (messageTime < 0.2)
		{
			sc = 1 + (0.2 - messageTime) * 10;
			ang -= (0.2 - messageTime) * 600;
		}

		Text txt_money(Assets::font_80, Assets::font_80_outline_2);

		txt_money.SetString("PLAYER " + std::to_string(gd->winner + 1) + " WINS");
		
		txt_money.SetOutlineColor(255,255,255);
		txt_money.SetFillColor(gd->winner ? ColorGreen : ColorBlue);
		
		vec pos = vec(vec(Window::GAME_WIDTH / 2.f, Window::GAME_HEIGHT / 2.f));

		if (messageTime < 0.2)
		{
			sc = messageTime / 0.2;
		}

		Window::Draw(txt_money, pos)
			.withOrigin(txt_money.Size() / 2)
			.withRotationDegs(ang)
			.withScale(2 * sc);
	}
}

SceneMain::SceneMain()
{
	ImGui::GetStyle().WindowRounding = 7.0;
}

void SceneMain::EnterScene() 
{
	InitGame(gd);
}

void SceneMain::ExitScene()
{
}

void SceneMain::Update(float dt)
{
	gd->gameTime += dt;

	if (!gd->started)
	{
		if (Input::IsJustPressedAnyPlayer(GameKeys::START)) {
			gd->started = true;
			gd->gameTime = 0;
		}
		return;
	}

	if (gd->winner == -1)
	{
		UpdateEntities(gd, dt);
	} 
	else 
	{
		messageTime += dt;

		if (Input::IsJustPressedAnyPlayer(GameKeys::START)) {
			InitGame(gd);
		}
	}

#ifdef _DEBUG
	for (int i = 0; i < 2; i++) {
		if (GamePad::IsButtonJustPressed(i, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_BACK)) {
			gd->money[0] += 100;
			gd->money[1] += 100;

		}
	}
	{
		ImGui::Begin("Cosas");

		std::string dbg_ship_pos = "[P1] Pos (" + std::to_string(gd->ent_x[gd->id_player[0]]) + ", " + std::to_string(gd->ent_y[gd->id_player[0]]) + ")";
		ImGui::Text(dbg_ship_pos.c_str());

		std::string dbg_ship_spd = "[P1] Speed (" + std::to_string(gd->ent_spd_x[gd->id_player[0]]) + ", " + std::to_string(gd->ent_spd_y[gd->id_player[0]]) + ")";
		ImGui::Text(dbg_ship_spd.c_str());

		std::string dbg_rot = "[P1] Rotation " + std::to_string(gd->ent_rot[gd->id_player[0]]);
		ImGui::Text(dbg_rot.c_str());

		std::string dbg_ship_pos2 = "[P2] Pos (" + std::to_string(gd->ent_x[gd->id_player[1]]) + ", " + std::to_string(gd->ent_y[gd->id_player[1]]) + ")";
		ImGui::Text(dbg_ship_pos2.c_str());

		std::string dbg_ship_spd2 = "[P2] Speed (" + std::to_string(gd->ent_spd_x[gd->id_player[1]]) + ", " + std::to_string(gd->ent_spd_y[gd->id_player[1]]) + ")";
		ImGui::Text(dbg_ship_spd2.c_str());

		std::string dbg_rot2 = "[P2] Rotation " + std::to_string(gd->ent_rot[gd->id_player[1]]);
		ImGui::Text(dbg_rot2.c_str());

		ImGui::End();
	}


	if (Debug::Draw)
	{
		for (int i = 0; i < GameConstants::MAX_ENTITIES; ++i)
		{
			if (gd->ent_type[i] == EntityType::NONE) continue;
			vec pos = vec(gd->ent_x[i] - gd->ent_size_rot[i].x / 2,
				gd->ent_y[i] - gd->ent_size_rot[i].y / 2);
			BoxBounds rs(pos, gd->ent_size_rot[i]);
			rs.DebugDraw();
		}
	}
#endif

}

void SceneMain::Draw()
{
	Window::Clear(0,0,0);

	Window::Draw(Assets::spr_bg, Camera::Bounds());

	if (!gd->started)
	{
		Window::Draw(Assets::spr_splash, Camera::Bounds());

		if (int(gd->gameTime * 2) % 2 < 1)
		{
			Text txt_start(Assets::font_160, Assets::font_160_outline_4);
			txt_start.SetString("PRESS START");
			txt_start.SetFillColor(236, 223, 202);
			txt_start.SetOutlineColor(0, 0, 0);

			vec textPos(Window::GAME_WIDTH / 2, Window::GAME_HEIGHT / 2 + 125);
			vec textSize = txt_start.Size();

			Window::Draw(txt_start, textPos)
				.withOrigin(textSize / 2.0f)
				.withScale(0.65f, 0.65f);
		}
	} else {

		RenderEntities();

		{
			int id_hq0 = gd->id_hq[0];
			vec size = vec(gd->ent_size[id_hq0].x + 1, 42);
			ImGui::SetNextWindowPosInGameCoords(vec(gd->ent_x[id_hq0] - gd->ent_size[id_hq0].x * 0.65, gd->ent_y[id_hq0]), vec(-size.x, -16));
			ImGui::Begin("hud_money_p1", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowFontScale(2.f);
			ImGui::Image(im(Assets::spr_mineral), ImVec2(mineralsizesmall, mineralsizesmall));
			ImGui::SameLine();
			std::string txt_money = std::to_string(gd->money[0]);
			ImGui::Text(txt_money.c_str());
			ImGui::SetWindowSize(im(size));
			ImGui::End();
		}

		{
			int id_hq1 = gd->id_hq[1];
			vec size = vec(gd->ent_size[id_hq1].x + 1, 42);
			ImGui::SetNextWindowPosInGameCoords(vec(gd->ent_x[id_hq1] + gd->ent_size[id_hq1].x * 0.65, gd->ent_y[id_hq1]), vec(0, -16));
			ImGui::Begin("hud_money_p2", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowFontScale(2.f);
			ImGui::Image(im(Assets::spr_mineral), ImVec2(mineralsizesmall, mineralsizesmall));
			ImGui::SameLine();
			std::string txt_money = std::to_string(gd->money[1]);
			ImGui::Text(txt_money.c_str());
			ImGui::SetWindowSize(im(size));
			ImGui::End();
		}

	}


}
