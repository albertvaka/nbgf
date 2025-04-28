#pragma once

#include "stats_tracker.h"
#include "entity.h"
#include "animation.h"
#include "rand.h"
#include "collide.h"
#include "mates.h"
#include "debug.h"
#include <set>
#include "assets.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "stats_tracker.h"

const float kVel = 50;
const float kMenjarDecreasePerSecond = 1;
const float kMaxMenjar = 90;
const float kMenjarBubbleDuration = 3.0f;
static int doggo_ids = 0;

struct Doggo : BoxEntity, SelfRegister<Doggo>
{
	float menjar = kMaxMenjar;
	float menjar_bubble_timer = 0.0f;
	Animation anim;
	int id = doggo_ids++;
	int nextPoint;
	bool forward;
	int destination = 0;
	bool wantFood = false;

	static void PlaySound() {
		switch (Rand::roll(3)) {
		case 0:
			Assets::soundDoggo1.Play();
			break;
		case 1:
			Assets::soundDoggo2.Play();
			break;
		case 2:
			Assets::soundDoggo3.Play();
			break;
		default:
			assert(false);
		}
	}

	static std::set<int>& AllDestinations() {
		static std::set<int> all_destinations = { 2 };
		return all_destinations;
	}

	static void ResetDestinations() {
		AllDestinations().clear();
		AllDestinations().insert(2);
	}

	Doggo()
		: BoxEntity(vec(), vec(28,28))
		, anim(DOGGO_RIGHT)
	{
		++StatsTracker::doggos_in_game;
		
		if (AllDestinations().size() == TiledEntities::waypoint.size()) {
			ResetDestinations();
		}

		while (true) {
			destination = Rand::roll(TiledEntities::waypoint.size());
			if (AllDestinations().find(destination) == AllDestinations().end()) {
				AllDestinations().insert(destination);
				break;
			}
		}

		if (Rand::OnceEvery(2)) {
			pos = TiledEntities::doggo_begin;
			nextPoint = 0;
			forward = true;
		}
		else {
			pos = TiledEntities::doggo_end;
			nextPoint = TiledEntities::waypoint.size()-1;
			forward = false;
		}
		PlaySound();
	}

	void Update(float dt)
	{

		anim.Update(dt * 1000);

		if (wantFood) {
			menjar -= kMenjarDecreasePerSecond * dt;

			menjar_bubble_timer -= dt;
			if (menjar_bubble_timer < 0) {
				anim.Ensure(DOGGO_IDLE);
			}
		}
		else {
			vec dest = TiledEntities::waypoint[nextPoint];
			vec diff = dest - pos;
			if (diff.x > 0) {
				anim.Ensure(DOGGO_RIGHT);
			}
			else {
				anim.Ensure(DOGGO_LEFT);
			}
			if (diff.Length() < kVel * dt) {
				pos += diff;
				if (nextPoint == destination) {
					anim.Ensure(DOGGO_IDLE);
					wantFood = true;
					PlaySound();
				}
				else {
					if (forward) {
						nextPoint++;
					}
					else {
						nextPoint--;
					}
				}
			}
			else {
				pos += diff.Normalized() * kVel * dt;
			}
		}
#ifdef _IMGUI
      ImGui::Begin((std::string("doggo") + ::std::to_string(id)).c_str());
	  ImGui::SliderFloat("menjar", &menjar, 0.f, kMaxMenjar);
	  ImGui::End();
#endif
	}

  void Draw() const
  {
    pos.DebugDraw();
	Window::Draw(Assets::doggoTexture, pos)
		.withRect(anim.CurrentFrameRect())
		.withOrigin(size/2.f);
    
		if (Debug::Draw) {
      Bounds().DebugDraw();
    }

    if (menjar_bubble_timer > 0.0f && int(menjar_bubble_timer) != 1) {
	  Window::Draw(Assets::plantTexture, pos + vec(17.0f, -15.f))
		  .withRect(16,0,18,23);
	}

	if (wantFood) {
		
		// Draw bottom status
		Window::Draw(Assets::plantTexture, pos + vec(0.0f, 18.0f))
			.withOrigin(17/2.f, 0)
			.withRect(52,32,17,16);
		
		// Draw face
		int happiness = std::min(int(menjar / (kMaxMenjar / 5)), 4);
		Window::Draw(Assets::plantTexture, pos + vec(0.0f, 21.0f))
			.withOrigin(6, 0)
			.withRect(106, 52 - 12 * happiness, 13, 12);

	}
  }

	void Feed() {
		menjar = kMaxMenjar;
		menjar_bubble_timer = kMenjarBubbleDuration;
		anim.Ensure(DOGGO_EAT);
		++StatsTracker::doggos_fed;
		PlaySound();
	}

};