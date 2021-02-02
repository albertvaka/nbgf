#pragma once

#include "entity.h"
#include "collide.h"
#include "input.h"
#include "anim_lib.h"
#include "building.h"
#include "waypoint.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "animation.h"
#include "camera.h"
#include "freeze_skill.h"
#include "wave_skill.h"

static const float JUMP_TIME = 0.6f;

static const float scale = 0.36f;
static const float speed = 100.f;

static const float panic_multiplier = 2.3f;

struct Person : BoxEntity, SelfRegister<Person>
{
	vec next_point;
	Waypoint* next_waypoint;
	Waypoint* old_waypoint;
	bool goingLeft;
	Animation anim;
	int player_id;
	float jump = -1;
	bool alive;
	float timerdead = 0;

	bool in_panic;
	vec panic_dir;
	float panic_left;

	float speed_multiplier;

	BoxBounds ClickBounds() const
	{
		return BoxBounds(pos, vec(150, 300)*scale, vec(150/2,300-80) * scale)*1.1f;
	}
		
	Person(const vec& position, int player_id)
		: BoxEntity(pos, vec(150, 150)*scale)
		, anim(AnimLib::NPC_1_DOWN)
		, player_id(player_id)
	{
		alive = true;
		pos = position;
		SetNearestWaypoint();
		old_waypoint = next_waypoint;
		next_waypoint = next_waypoint->GetRandomNext();
		next_point = next_waypoint->GetRandomPoint();
		in_panic = false;
		panic_left = 0.0f;
		panic_dir = vec::Zero;
		speed_multiplier = 1.0f;
	}

	void SetNearestWaypoint() {
		next_waypoint = Waypoint::GetAll()[0];
		for (auto w : Waypoint::GetAll()) {
			float w_distance = this->pos.Distance(w->pos);
			float current_distance = this->pos.Distance(next_waypoint->pos);
			if (w_distance < current_distance) {
				next_waypoint = w;
			}
		}
	}

	void UpdatePlayer(float dt) {
		if (!alive) { 
			timerdead += dt;
			anim.Update(dt);
			return; 
		}
		vec dir = Input::GetAnalog(player_id, AnalogInput::MOVE).Normalized();
		if (Input::IsJustPressed(player_id, GameKeys::ACTION)) {
			if (jump < 0) {
				jump = JUMP_TIME;
				anim.Set(AnimLib::NPC_1_JUMP, false);
			}
		}
		if (Input::IsPressed(player_id, GameKeys::RUN)) {
			speed_multiplier = panic_multiplier;
		}
		else {
			speed_multiplier = 1.0f;
		}

		if (jump > 0) {
			jump -= dt;
			dir = vec::Zero;
			anim.Update(dt);
		}

		if (dir != vec::Zero) {
			dir.Normalize();
			vel = dir * speed * speed_multiplier;

			UpdateAnim(dir);

			if (vel != vec::Zero) {
				vec oldPos = pos;
				pos.x += vel.x * dt;
				for (const Building* b : Building::GetAll()) {
					if (Collide(this, b)) {
						pos.x = oldPos.x;
						vel.x = 0;
					}
				}
				pos.y += vel.y * dt;
				for (const Building* b : Building::GetAll()) {
					if (Collide(this, b)) {
						pos.y = oldPos.y;
						vel.y = 0;
					}
				}
				if (pos != oldPos) {
					anim.Update(dt);
				}
			}
		}
	}

	void Kill() {
		alive = false;
		anim.Ensure(AnimLib::NPC_1_DIE, false);
		for (auto p : Person::GetAll()) {
			if (p->pos.Distance(pos) < 600) {
				p->Panic(p->pos - pos);
			}
		}
	}

	void Panic(vec dir) {
		in_panic = true;
		panic_dir = dir;
		panic_left = 2.5f;
		speed_multiplier = panic_multiplier;
	}

	bool isAlive() {
		return alive || timerdead < 1.f;
	}

	void UpdateNpc(float dt)
	{
		if (!alive) {
			timerdead += dt;
			anim.Update(dt);
			return;
		}

		vec dir;
		if (in_panic) {
			dir = panic_dir;
		}
		else {
			dir = next_point - pos;
		}

		dir.Normalize();

		for (auto f : FreezeSkill::GetAll()) {
			if (f->freezeNow) {
				dir = vec::Zero;
			}
		}
		for (WaveSkill* f : WaveSkill::GetAll()) {
			if (f->InWave(pos)) {
				jump = JUMP_TIME;
				anim.Ensure(AnimLib::NPC_1_JUMP, false);
			}
		}

		if (jump > 0) {
			jump -= dt;
			if (jump <= 0) {
				jump = -1;
			}
			dir = vec::Zero;
			anim.Update(dt);
		}

		if (dir != vec::Zero) {
			dir.Normalize();
			vel = dir * speed * speed_multiplier;

			UpdateAnim(dir);

			if (vel != vec::Zero) {
				vec oldPos = pos;
				pos.x += vel.x * dt;
				for (const Building* b : Building::GetAll()) {
					if (Collide(this, b)) {
						pos.x = oldPos.x;
						vel.x = 0;
					}
				}
				pos.y += vel.y * dt;
				for (const Building* b : Building::GetAll()) {
					if (Collide(this, b)) {
						pos.y = oldPos.y;
						vel.y = 0;
					}
				}
				if (pos != oldPos) {
					anim.Update(dt);
				}
			}
		}

		// vel = direction * speed * speed_multiplier;
		// pos += vel * dt;
		next_point.DebugDraw();

		if (this->pos.DistanceSq(next_point) < 5) {
			Waypoint* current = next_waypoint;
			if (current->links.size() == 1) {
				// If there's only one link, I have to take it even if it goes back
				next_waypoint = current->links[0];
			}
			else {
				// Do not go back to previous waypoint
				do {
					next_waypoint = current->GetRandomNext();
				} while (next_waypoint == old_waypoint);
			}
			old_waypoint = current;
			next_point = next_waypoint->GetRandomPoint();
		}

		panic_left -= dt;

		if (panic_left <= 0) {
			in_panic = false;
			speed_multiplier = 1.0f;
		}
	}


	Window::PartialDraw Draw() const
	{
		if (!alive) {
			float scale = std::min(timerdead/2.f, 0.6f);
			Window::Draw(Assets::bloodTexture, pos + vec(0, 20))
				.withOrigin(Assets::bloodTexture->w / 2, Assets::bloodTexture->h / 2)
				.withScale(int(pos.y*100)%2? scale : -scale, int(pos.x*100)%2? scale : -scale);

		}

		ClickBounds().DebugDraw(0, 0, 255);

		vec jumpoffset = vec::Zero;

		if (jump > 0) {
			jumpoffset.y = -70*sin(Mates::map(jump, JUMP_TIME, 0.f, 0.f, 3.1416f));
		}

		const GPU_Rect& rect = anim.CurrentFrameRect();
		return Window::PartialDraw(Assets::npcTexture, pos - vec(0, 80*scale) + jumpoffset)
			.withRect(rect)
			.withOrigin(rect.w / 2, rect.h / 2)
			.withScale(goingLeft? -scale : scale, scale);

	}

	void UpdateAnim(vec direction) {
		if (abs(direction.x) > 0.5) {
			anim.Ensure(AnimLib::NPC_1_LEFT);
			goingLeft = (direction.x < 0);
		}
		else if (direction.y != 0) {
			if (direction.y < 0) {
				anim.Ensure(AnimLib::NPC_1_UP);
			}
			else {
				anim.Ensure(AnimLib::NPC_1_DOWN);
			}
		}
	}

};
