#include "entity.h"
#include "animation.h"
#include "window.h"
#include "rand.h"
#include "cleaner.h"

struct Taca : Cintable, SelfRegister<Taca>
{
	//Note: need to re-define some attrs since it's not an entity
	bool alive = true;
	vec pos;
	vec speed;
	SDL_Color m_color = {200,0,0, 255};
	vec m_offset;
	float counter;
	bool onCinta = false;

	Entity* roomba_absorbing = nullptr;
	int timer_absorb = 0;
	int TIMER_ABSORB_MAX = 1000;

	Taca(vec position, EntityDirection dir)
	{
		prevCintaDirection = dir;
		counter = 7500 + Rand::roll(5000);
		pos = position;

		m_offset = vec::FromAngleDegs(Rand::roll(0, 360))*Rand::rollf(0, 5);
		m_color.r += Rand::roll(0, 50);
		m_color.g += Rand::roll(0, 50);
		m_color.b += Rand::roll(0, 50);
	}

	vec positionPlz() override { return onCinta ? pos : getFinalPos(); }
	vec sizePlz() override { return onCinta ? vec(16, 16) : vec(1,1); }

	void Draw()
	{
		//BoxBounds(positionPlz(),sizePlz(), true).Draw(window);
		vec posfinal = pos;

		float progress = float(timer_absorb) / float(TIMER_ABSORB_MAX);

		if (roomba_absorbing)
		{
			posfinal = pos + (roomba_absorbing->pos - pos) * progress;
		}

		posfinal += m_offset;

		posfinal.x = int(posfinal.x);
		posfinal.y = int(posfinal.y);

		if (roomba_absorbing)
		{
			m_color.a = (1.0f - progress) * 255;
		}

		float sc = 1;
		if (roomba_absorbing)
		{
			sc = 1.25f + float(timer_absorb / TIMER_ABSORB_MAX)*1.5f;
		}
		Window::DrawPrimitive::Rectangle(posfinal, posfinal+vec(1,1)*sc, -1, m_color);
	}

	void AbsorbByRoomba(Entity* roomba)
	{
		roomba_absorbing = roomba;
	}

	void Update(int dt)
	{
		if (roomba_absorbing)
		{
			timer_absorb += dt;
			if (timer_absorb > TIMER_ABSORB_MAX)
			{
				alive = false;
			}
		}
		else
		{
			onCinta = SetSpeedWithCinta(speed);
			speed = speed * 0.8f;
			if (onCinta) {
				counter -= dt;
				if (counter < 0)
				{
					alive = false;
				}
			}
		}


		pos += speed * dt;
		speed = vec::Zero;
		if (pos.y > Window::GAME_HEIGHT/Window::GAME_ZOOM) {
			alive = false;
		}
	}

	vec getFinalPos()
	{
		return pos + m_offset;
	}

};
