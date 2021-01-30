#pragma once

#include "entity.h"
#include "collide.h"
#include "input.h"
#include "anim_lib.h"
#include "building.h"
#include "waypoint.h"
#include "selfregister.h"
#include "assets.h"
#include "net.h"
#include "window.h"
#include "animation.h"
#include "camera.h"

static const float scale = 0.3f;
static const float speed = 100.f;

struct Person : BoxEntity, SelfRegister<Person>
{
	vec next_point;
	Waypoint* next_waypoint;
	Waypoint* old_waypoint;
	bool goingLeft;
	Animation anim;
	int player_id;
	bool alive;
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
		if (!alive) { return; }
		vec dir = vec::Zero;
		if (Input::IsPressed(player_id, GameKeys::UP)) {
			dir.y = -1;
		}
		if (Input::IsPressed(player_id, GameKeys::DOWN)) {
			dir.y = 1;
		}
		if (Input::IsPressed(player_id, GameKeys::RIGHT)) {
			dir.x = 1;
		}
		if (Input::IsPressed(player_id, GameKeys::LEFT)) {
			dir.x = -1;
		}

		if (dir != vec::Zero) {
			dir.Normalize();
			vel = dir * speed;

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
	}
	void UpdateNpc(float dt)
	{
		if (!alive) { return; }

		anim.Update(dt);
		vec direction = next_point - pos;
		direction.Normalize();
		vel = direction * speed;
		pos += vel * dt;
		next_point.DebugDraw();


		UpdateAnim(direction);

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
	}

	Window::PartialDraw Draw() const
	{
		if(!alive) {
			Window::DrawPrimitive::Circle(pos, 10, 5, 255, 0, 0);
		}

		const GPU_Rect& rect = anim.CurrentFrameRect();
		return Window::PartialDraw(Assets::npcTexture, pos - vec(0, 80*scale))
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

	/*
	EntityUpdate Serialize() {
		int frame = anim.CurrentFrameRect().x / AnimLib::frameSize;
		return {
			pos.x,
			pos.y,
			id,
			goingLeft ? -frame : frame,
		};
	}

	void Kill() {
		alive = false;
	}

	static Window::PartialDraw DumbDraw(EntityUpdate* entity) {
		
		int sprite = entity->sprite;
		bool mirror = false;
		if (sprite < 0) {
			sprite = -sprite;
			mirror = true;
		}
		int sheet_x = sprite;
		int sheet_y = 0;

		GPU_Rect rect = { sheet_x * AnimLib::frameSize, sheet_y * AnimLib::frameSize, AnimLib::frameSize, AnimLib::frameSize };
		return Window::PartialDraw(Assets::npcTexture, entity->x, entity->y - 80 * scale)
			.withRect(rect)
			.withOrigin(rect.w / 2, rect.h / 2)
			.withScale(mirror ? -scale : scale, scale);
	}
	*/
};