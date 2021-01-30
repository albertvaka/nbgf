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
#include "camera.h"


struct Person : CircleEntity, SelfRegister<Person>
{
	int velo;
	vec next_point;
	Waypoint* next_waypoint;
	Waypoint* old_waypoint;
	Person(const vec& position)
		: CircleEntity(pos, 10)
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
		/*
		vec oldPos = pos;
		if(Input::IsPressedAnyPlayer(GameKeys::UP)) {
			pos.y -= velo *dt; 
		}
		if(Input::IsPressedAnyPlayer(GameKeys::RIGHT)) {
			pos.x += velo *dt; 
		}
		if(Input::IsPressedAnyPlayer(GameKeys::LEFT)) {
			pos.x -= velo *dt; 
		}
		if(Input::IsPressedAnyPlayer(GameKeys::DOWN)) {
			pos.y += velo *dt; 
		}
		for (const Building* b : Building::GetAll()) {
			if (Collide(this, b)) {
				pos = oldPos;
			}
		}*/
		vec direction = next_point - pos;
		direction.Normalize();
		pos += direction * velo * dt;
		if (this->pos.Distance(next_point) < 1) {
			old_waypoint = next_waypoint;
			next_waypoint = next_waypoint->GetRandomNext();
			next_point = next_waypoint->GetRandomPoint();
		}
		for (const Building* b : Building::GetAll()) {
			if (Collide(this, b)) {
				next_waypoint = old_waypoint;
				next_point = next_waypoint->GetRandomPoint();		
			}
		}
	}

	void Draw() const
	{
		
	}
};