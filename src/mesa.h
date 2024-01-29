#pragma once

#include <vector>
#include "rand.h"

#include "entity.h"
#include "lever.h"
#include "extremity.h"

struct Mesa;

struct Collector : SortedDrawable, SelfRegister<Collector>
{
	ExtremityType type;
	Mesa* mesa = nullptr;
	Extremity* extremity = nullptr;

	bool mesa_was_empty = true;

	Collector(vec position, ExtremityType et)
	{
		type = et;
		pos = position;
		anim.Ensure(AnimationType::CAPSULE_CLOSED);
		anim.loopable = false;
	}

	void Update(int dt) override;

	void Draw() override
	{
		Window::Draw(Assets::sprite, pos).withRect(anim.CurrentFrame()).withOrigin(8,8);
	}

};

struct Mesa : SortedDrawable, SelfRegister<Mesa>, Buttonable
{
	Cadaver* cadaver = nullptr;
	Lever* lever = nullptr;

	ExtremityType type;
	Collector* collector = nullptr;

	Mesa(vec position, ExtremityType et)
	{
		type = et;
		pos = position;

		anim.Ensure(AnimationType::POKEMON);
		lever = new Lever(pos-vec(16, 42));
		lever->ent_connected = this;
	}

	void Update(int dt) override
	{
		lever->is_connected = (collector->extremity && cadaver && !cadaver->HasExtremity(type) && collector->extremity->type == type) || (!collector->extremity && cadaver && cadaver->HasExtremity(type));
	}

	void Draw() override
	{
		Window::Draw(Assets::sprite, vec(pos.x - 8.75f, pos.y - 18.5f)).withRect(anim.CurrentFrame()).withScale(1.25f).withOrigin(8,8);
	}

	void Operate() override
	{
		if (cadaver)
		{
			if (collector->extremity)
			{
				if (!cadaver->HasExtremity(type))
				{
					cadaver->AttachExtremity(collector->extremity);
					collector->extremity = nullptr;
				}
			}
			else
			{
				if (cadaver->HasExtremity(type))
				{
					Extremity* e = cadaver->DeatachExtremity(type, collector->pos);
					collector->extremity = e;
				}
			}
		}
	}

};


inline void Collector::Update(int dt)
{
	Collector* c = this;

	bool mesa_empty = !c->mesa->cadaver;

	if (c->mesa_was_empty && (!mesa_empty || c->extremity))
	{
		c->anim.Ensure(AnimationType::CAPSULE_OPENING);
		c->anim.loopable = false;
	}

	if (!c->mesa_was_empty && (mesa_empty && !c->extremity))
	{
		c->anim.Ensure(AnimationType::CAPSULE_CLOSING);
		c->anim.loopable = false;
	}

	c->mesa_was_empty = (mesa_empty && !c->extremity);
}
