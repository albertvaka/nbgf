#pragma once

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "input.h"
#include "steering_behavior.h"
#include "stroke.h"

const float deceleration_coef = 0.3;
const float max_speed = 220.f;
const float timeBetweenStrokeSegments = 0.06f;

extern float mainClock;

class Ship : public Entity {
public:

    Ship() {
		stroke.SetMaxJoints(50);
		SDL_Color foamColor = { Uint8(0.7 * 255), Uint8(0.8 * 255), Uint8(0.9 * 255), 255 };
		stroke.SetInnerColor(foamColor);
		stroke.SetOuterColor(foamColor);
		stroke.SetEndThickness(0.0f);
	}

	void Reset() {
		pos = vec::Zero;
		vel = vec::Zero;
		heading = vec(1,0);
		timer = 0.f;
		stroke.Clear();
	}

	void Update(float dt) {
		float speed = vel.Length();

		// The correct way to add friction
		float frictionImpulse = speed * deceleration_coef;
		speed -= frictionImpulse * dt;

        if (Input::IsPressed(0, GameKeys::UP)) {
			speed += 90*dt;
		}
		if (Input::IsPressed(0, GameKeys::DOWN)) {
			speed -= 90*dt;
		}
		if (Input::IsPressed(0, GameKeys::LEFT)) {
			heading = heading.RotatedAroundOriginRads(-0.0035*dt*(speed+25));
			heading.Normalize();
		}
		if (Input::IsPressed(0, GameKeys::RIGHT)) {
			heading = heading.RotatedAroundOriginRads(0.0035*dt*(speed+25));
			heading.Normalize();
		}

		vel = speed*heading;
		vel.Truncate(max_speed);
		pos += vel * dt;

		stroke.Calculate(dt);

		timer += dt;
		if (timer > timeBetweenStrokeSegments) {
			timer -= timeBetweenStrokeSegments;
			stroke.AddJoint(pos, 10.f + sin(mainClock * 8.f) * 2.f);
		}

	}

	void Draw() {
		float angle = AngleDegs(heading);
		Window::Draw(Assets::shipTexture, pos)
			.withOrigin(Assets::shipTexture->w / 2, Assets::shipTexture->h / 2)
			.withRotationDegs(angle)
			.withScale(0.5f);
	}
	
	void DrawStroke() {
		stroke.Draw();
	}

private:
	float timer;
	vec heading;
	Stroke stroke;
};
