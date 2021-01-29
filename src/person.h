#pragma once

#include "entity.h"
#include "collide.h"
#include "input.h"
#include "anim_lib.h"
#include "building.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "camera.h"


struct Person : CircleEntity, SelfRegister<Person>
{
	int velo;
	Person(const vec& position)
		: CircleEntity(pos, 10)
	{
		pos = position;
		velo = 100;
	}

	void Update(float dt)
	{
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
				/*vec away = this->pos - b->pos;
				away.Normalize();
				float awayDistance = sqrt(-this->Bounds().DistanceSq(b->Bounds()));
				pos += away*awayDistance;*/
				pos = oldPos;
			}
		}
	}

	void Draw() const
	{
		
	}
};