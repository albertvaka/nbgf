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
	Person(const vec& position)
		: BoxEntity(pos, vec(150, 150)*scale)
		, anim(AnimLib::NPC_1_DOWN)
	{
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

	void Update(float dt)
	{
		vel = vec::Zero;
		/*

		if(Input::IsPressed(0, GameKeys::UP)) {
			vel.y = -speed;
		}
		if(Input::IsPressed(0, GameKeys::RIGHT)) {
			vel.x = speed;
		}
		if(Input::IsPressed(0, GameKeys::LEFT)) {
			vel.x = -speed;
		}
		if(Input::IsPressed(0, GameKeys::DOWN)) {
			vel.y = speed;
		}

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
		*/		
		anim.Update(dt);
		vec direction = next_point - pos;
		direction.Normalize();
		vel = direction * speed;
		pos += vel * dt;
		next_point.DebugDraw();

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

		if (this->pos.Distance(next_point) < 1) {
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
		for (const Building* b : Building::GetAll()) {
			if (Collide(this, b)) {
				SetNearestWaypoint();
				next_point = next_waypoint->GetRandomPoint();		
			}
		}
	}

	void Draw() const
	{
		const GPU_Rect& rect = anim.CurrentFrameRect();
		Window::Draw(Assets::npcTexture, pos - vec(0, 80*scale))
			.withRect(rect)
			.withOrigin(rect.w / 2, rect.h / 2)
			.withScale(goingLeft? -scale : scale, scale);

	}

	static void DumbDraw(EntityUpdate* entity) {
		
		int sprite = entity->sprite;
		bool mirror = false;
		if (sprite < 0) {
			sprite = -sprite;
			mirror = true;
		}
		int sheet_x = sprite;
		int sheet_y = 0;

		GPU_Rect rect = { sheet_x * AnimLib::frameSize, sheet_y * AnimLib::frameSize, AnimLib::frameSize, AnimLib::frameSize };
		Window::Draw(Assets::npcTexture, entity->x, entity->y - 80 * scale)
			.withRect(rect)
			.withOrigin(rect.w / 2, rect.h / 2)
			.withScale(mirror ? -scale : scale, scale);
	}

};