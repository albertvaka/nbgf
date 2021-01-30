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

static const float scale = 0.3f;
static const float speed = 100.f;

struct Person : BoxEntity, SelfRegister<Person>
{
	int velo;
	vec next_point;
	Waypoint* next_waypoint;
	Waypoint* old_waypoint;
	mutable bool goingLeft;
	mutable Animation anim;
	Person(const vec& position)
		: BoxEntity(pos, vec(200, 400)*scale)
		, anim(AnimLib::NPC_1_DOWN)
	{
		pos = position;
		velo = 30;
		SetNearestWaypoint();
		old_waypoint = next_waypoint;
	}

	void SetNearestWaypoint() {
		next_waypoint = Waypoint::GetAll()[0];
		for (auto w : Waypoint::GetAll()) {
			float w_distance = this->pos.Distance(w->pos);
			float current_distance = this->pos.Distance(next_waypoint->pos);
			if (w_distance < current_distance) {
				next_waypoint = w;
				next_point = w->GetRandomPoint();
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
		vel = direction * velo;
		pos += vel * dt;
		if (this->pos.Distance(next_point) < 1) {
			old_waypoint = next_waypoint;
			next_waypoint = next_waypoint->GetRandomNext();
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
		if (vel.x != 0) {
			anim.Ensure(AnimLib::NPC_1_LEFT);
			goingLeft = (vel.x < 0);
		} else if (vel.y != 0) {
			if (vel.y < 0) {
				anim.Ensure(AnimLib::NPC_1_UP);
			} else {
				anim.Ensure(AnimLib::NPC_1_DOWN);
			}
		}

		const GPU_Rect& rect = anim.CurrentFrameRect();
		Window::Draw(Assets::npcTexture, pos)
			.withRect(rect)
			.withOrigin(rect.w / 2, rect.h / 2)
			.withScale(goingLeft? -scale : scale, scale);

	}
};